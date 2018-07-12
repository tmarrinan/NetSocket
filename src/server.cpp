#include "netsocket/server.h"

NetSocket::Server::Server(uint16_t port, NetSocket::ServerOptions& options) :
    acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
    context(asio::ssl::context::sslv23),
    secure(false),
    use_callbacks(options.flags & NetSocket::ServerFlags::UseCallbacks),
    connect_callback(NULL),
    disconnect_callback(NULL)
{
    if (options.server_cert)
    {
        // To generate keys:
        // `openssl req -new -newkey rsa:2048 -nodes -out ca.csr -keyout ca.key`
        // `openssl x509 -trustout -signkey ca.key -days 365 -req -in ca.csr -out ca.crt`
        // `cat ca.crt ca.key > server_crt_key.pem`
        // To generate dh file:
        // `openssl dhparam -out dh.pem 2048`
        if (options.encrypted_key_passwd)
        {
            key_passwd = std::string(options.encrypted_key_passwd);
        }
        else
        {
            key_passwd = std::string("test");
        }
        long ssl_opts = asio::ssl::context::default_workarounds | asio::ssl::context::single_dh_use;
        if (options.flags & NetSocket::ServerFlags::NoSslV2) ssl_opts |= asio::ssl::context::no_sslv2;
        if (options.flags & NetSocket::ServerFlags::NoSslV3) ssl_opts |= asio::ssl::context::no_sslv3;
        if (options.flags & NetSocket::ServerFlags::NoTlsV1) ssl_opts |= asio::ssl::context::no_tlsv1;
        if (options.flags & NetSocket::ServerFlags::NoTlsV1_1) ssl_opts |= asio::ssl::context::no_tlsv1_1;
        if (options.flags & NetSocket::ServerFlags::NoTlsV1_2) ssl_opts |= asio::ssl::context::no_tlsv1_2;
        context.set_options(ssl_opts);
        context.set_password_callback(std::bind(&Server::GetPassword, this));
        context.use_certificate_chain_file(options.server_cert);
        context.use_private_key_file(options.server_cert, asio::ssl::context::pem);
        context.use_tmp_dh_file(options.dh_file);
        const char *ciphers = "ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-AES256-"
                              "GCM-SHA384:DHE-RSA-AES128-GCM-SHA256:DHE-DSS-AES128-GCM-SHA256:kEDH+AESGCM:ECDHE-RSA-AES128-SHA256:ECDHE-"
                              "ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA:ECDHE-ECDSA-AES128-SHA:ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-"
                              "AES256-SHA384:ECDHE-RSA-AES256-SHA:ECDHE-ECDSA-AES256-SHA:DHE-RSA-AES128-SHA256:DHE-RSA-AES128-SHA:DHE-"
                              "DSS-AES128-SHA256:DHE-RSA-AES256-SHA256:DHE-DSS-AES256-SHA:DHE-RSA-AES256-SHA:!aNULL:!eNULL:!EXPORT:!DES:"
                              "!RC4:!3DES:!MD5:!PSK";
        if (SSL_CTX_set_cipher_list(context.native_handle(), ciphers) != 1)
        {
            fprintf(stderr, "Error: could not set cipher list\n");
        }
        secure = true;
    }

    StartAccept();
    std::cout << "[NetSocket::Server] Now listening at " << acceptor.local_endpoint() << std::endl;
}

NetSocket::Server::Event NetSocket::Server::WaitForNextEvent()
{
    NetSocket::Server::Event event;
    if (!event_queue.empty())
    {
        event = event_queue.front();
        event_queue.pop_front();
    }
    else
    {
        do
        {
            io_service.run_one();
            io_service.reset();
        } while (event_queue.empty());
        event = event_queue.front();
        event_queue.pop_front();
    }
    return event;
}

NetSocket::Server::Event NetSocket::Server::PollForNextEvent()
{
    NetSocket::Server::Event event;
    if (!event_queue.empty())
    {
        event = event_queue.front();
        event_queue.pop_front();
    }
    else
    {
        io_service.poll();
        if (!event_queue.empty())
        {
            event = event_queue.front();
            event_queue.pop_front();
        }
        else
        {
            event.type = NetSocket::Server::EventType::None;
            event.client = NULL;
            event.string_data = "";
            event.binary_data = NULL;
            event.data_length = 0;
        }
    }
    return event;
}

bool NetSocket::Server::Alive()
{
    return true;
}

int16_t NetSocket::Server::NumClients()
{
    return static_cast<uint16_t>(clients.size());
}

void NetSocket::Server::Run()
{
    io_service.run();
}

void NetSocket::Server::Poll()
{
    io_service.poll();
}

std::string NetSocket::Server::GetPassword()
{
    return key_passwd;
}

void NetSocket::Server::StartAccept()
{
    ClientConnection::Pointer new_connection;
    if (secure)
    {
        new_connection = ClientConnection::Create(this, acceptor.get_io_service(), context, use_callbacks);
    }
    else
    {
        new_connection = ClientConnection::Create(this, acceptor.get_io_service(), use_callbacks);
    }
    new_connection->Socket()->AsyncAccept(acceptor, std::bind(&Server::HandleAccept, this, new_connection, std::placeholders::_1));
    //acceptor.async_accept(new_connection->Socket(), std::bind(&Server::HandleAccept, this, new_connection, std::placeholders::_1));
}

void NetSocket::Server::HandleAccept(NetSocket::ClientConnection::Pointer new_connection, const asio::error_code& error)
{
    if (!error)
    {
        new_connection->Socket()->AsyncHandshake(SslSocket::server, std::bind(&Server::HandleHandshake, this, new_connection, std::placeholders::_1));
    }

    StartAccept();
}

void NetSocket::Server::HandleHandshake(NetSocket::ClientConnection::Pointer new_connection, const asio::error_code& error)
{
    if (!error)
    {
        new_connection->DisconnectCallback(std::bind(&Server::HandleDisconnect, this, std::placeholders::_1));

        clients[new_connection->Endpoint()] = new_connection;
        if (use_callbacks && connect_callback)
        {
            connect_callback(*this, new_connection);
        }
        else if (!use_callbacks)
        {
            NetSocket::Server::Event event = {NetSocket::Server::EventType::Connect, new_connection, std::string(""), NULL, 0};
            event_queue.push_back(event);
        }

        new_connection->Receive();
    }
}

void NetSocket::Server::HandleDisconnect(NetSocket::ClientConnection::Pointer connection)
{
    clients.erase(connection->Endpoint());
    if (use_callbacks && disconnect_callback)
    {
        disconnect_callback(*this, connection);
    }
    else if (!use_callbacks)
    {
        NetSocket::Server::Event event = {NetSocket::Server::EventType::Disconnect, connection, std::string(""), NULL, 0};
        event_queue.push_back(event);
    }
}

void NetSocket::Server::Broadcast(std::string message)
{
    for (auto const& it : clients)
    {
        it.second->Send(message);
    }
}

void NetSocket::Server::Broadcast(const void *message, uint32_t length, CopyMode mode)
{
    for (auto const& it : clients)
    {
        it.second->Send(message, length, mode);
    }
}

void NetSocket::Server::ConnectCallback(std::function<void(Server&, ClientConnection::Pointer)> callback)
{
    connect_callback = callback;
}

void NetSocket::Server::DisconnectCallback(std::function<void(Server&, ClientConnection::Pointer)> callback)
{
    disconnect_callback = callback;
}

void NetSocket::Server::ClientSendIsComplete(ClientConnection::Pointer connection, void *data, uint32_t length)
{
    if (!use_callbacks)
    {
        NetSocket::Server::Event event = {NetSocket::Server::EventType::SendFinished, connection, std::string(""), data, length};
        event_queue.push_back(event);
    }
}

void NetSocket::Server::ClientReceivedStringData(ClientConnection::Pointer connection, std::string data)
{
    if (!use_callbacks)
    {
        NetSocket::Server::Event event = {NetSocket::Server::EventType::ReceiveString, connection, data, NULL, static_cast<uint32_t>(data.length())};
        event_queue.push_back(event);
    }
}

void NetSocket::Server::ClientReceivedBinaryData(ClientConnection::Pointer connection, void *data, uint32_t length)
{
    if (!use_callbacks)
    {
        NetSocket::Server::Event event = {NetSocket::Server::EventType::ReceiveBinary, connection, std::string(""), data, length};
        event_queue.push_back(event);
    }
}

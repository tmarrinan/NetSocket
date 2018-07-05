#include "netsocket/server.h"

NetSocket::Server::Server(uint16_t port, const char *tls_cert, const char *dh) :
    acceptor(io_service, tcp::endpoint(tcp::v4(), port)),
    context(asio::ssl::context::sslv23),
    secure(false),
    connect_callback(NULL),
    disconnect_callback(NULL)
{
    if (tls_cert != NULL)
    {
        // To generate keys:
        // `openssl req -new -newkey rsa:2048 -nodes -out ca.csr -keyout ca.key`
        // `openssl x509 -trustout -signkey ca.key -days 365 -req -in ca.csr -out ca.crt`
        // `cat ca.crt ca.key > server_crt_key.pem`
        // To generate dh file:
        // `openssl dhparam -out dh.pem 2048`
        context.set_options(asio::ssl::context::default_workarounds |
                            asio::ssl::context::no_sslv2 |
                            //asio::ssl::context::no_sslv3 |
                            //asio::ssl::context::no_tlsv1 |
                            asio::ssl::context::single_dh_use);
        context.set_password_callback(std::bind(&Server::GetPassword, this));
        context.use_certificate_chain_file(tls_cert);
        context.use_private_key_file(tls_cert, asio::ssl::context::pem);
        context.use_tmp_dh_file(dh);
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
    return "test";
}

void NetSocket::Server::StartAccept()
{
    ClientConnection::Pointer new_connection;
    if (secure)
    {
        new_connection = ClientConnection::Create(this, acceptor.get_io_service(), context);
    }
    else
    {
        new_connection = ClientConnection::Create(this, acceptor.get_io_service());
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
        if (connect_callback) connect_callback(*this, new_connection);

        new_connection->Receive();
    }
}

void NetSocket::Server::HandleDisconnect(std::string endpoint)
{
    clients.erase(endpoint);
    if (disconnect_callback) disconnect_callback(*this, endpoint);
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

void NetSocket::Server::DisconnectCallback(std::function<void(Server&, std::string)> callback)
{
    disconnect_callback = callback;
}
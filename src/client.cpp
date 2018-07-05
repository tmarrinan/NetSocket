#include "netsocket/client.h"

NetSocket::Client::Client(std::string host, uint16_t port, bool secure) :
    context(asio::ssl::context::sslv23),
    connect_callback(NULL),
    disconnect_callback(NULL),
    receive_string_callback(NULL),
    receive_binary_callback(NULL)
{
    if (secure)
    {
        socket = new NetSocket::SecureSocket(io_service, context);
        //context.load_verify_file(ca);
        //socket = new NetSocket::SecureSocket(io_service, context, std::bind(&Client::HandleVerify, this, std::placeholders::_1, std::placeholders::_2));
    }
    else
    {
        socket = new NetSocket::BasicSocket(io_service);
    }

    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host, std::to_string(port), tcp::resolver::query::canonical_name);
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    socket->AsyncConnect(endpoint_iterator, std::bind(&Client::HandleConnect, this, std::placeholders::_1, std::placeholders::_2));
    //asio::connect(socket, endpoint_iterator);
}

bool NetSocket::Client::HandleVerify(bool preverified, asio::ssl::verify_context& ctx)
{
    char subject_name[256];
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout << "Verifying: " << subject_name << std::endl;
    printf("verify: %d\n", preverified);
    return preverified;
}

void NetSocket::Client::HandleConnect(const asio::error_code& error, tcp::resolver::iterator iterator)
{
    if (!error)
    {
        socket->AsyncHandshake(SslSocket::client, std::bind(&Client::HandleHandshake, this, std::placeholders::_1));
    }
}

void NetSocket::Client::HandleHandshake(const asio::error_code& error)
{
    if (!error)
    {
        if (connect_callback) connect_callback(*this);
        Receive();
    }
    else
    {
        printf("HANDSHAKE ERROR: %s\n", error.message().c_str());
        exit(1);
    }
}

void NetSocket::Client::Run()
{
    io_service.run();
}

void NetSocket::Client::Poll()
{
    io_service.poll();
}

void NetSocket::Client::Send(std::string message)
{
    uint32_t length = message.length() + 1;
    uint8_t *buffer = new uint8_t[5 + length];
    buffer[0] = static_cast<uint8_t>(DataType::String);
    uint32_t size = htonl(static_cast<uint32_t>(length));
    memcpy(buffer + 1, &size, 4);
    memcpy(buffer + 5, message.c_str(), length);

    asio::const_buffer data = asio::buffer(const_cast<const uint8_t*>(buffer), 5 + length);

    send_queue.push_back(data);
    if (send_queue.size() == 1)
    {
        socket->AsyncWrite(data, std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
        //asio::async_write(socket, data, std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
    }
}

void NetSocket::Client::Send(const void *message, uint32_t length, CopyMode mode)
{
    if (mode == CopyMode::MemCopy)
    {
        uint8_t *buffer = new uint8_t[5 + length];
        buffer[0] = static_cast<uint8_t>(DataType::Binary);
        uint32_t size = htonl(length);
        memcpy(buffer+1, &size, 4);
        memcpy(buffer+5, message, length);

        asio::const_buffer data = asio::buffer(const_cast<const uint8_t*>(buffer), 5 + length);

        send_queue.push_back(data);
        if (send_queue.size() == 1)
        {
            socket->AsyncWrite(data, std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
            //asio::async_write(socket, asio::buffer(buffer, 5 + length), std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
        }
    }
    else
    {
        uint8_t *buffer = new uint8_t[5];
        buffer[0] = static_cast<uint8_t>(DataType::Binary);
        uint32_t size = htonl(length);
        memcpy(buffer+1, &size, 4);

        asio::const_buffer data1 = asio::buffer(const_cast<const uint8_t*>(buffer), 5);
        asio::const_buffer data2 = asio::buffer(const_cast<const void*>(message), length);

        send_queue.push_back(data1);
        send_queue.push_back(data2);
        if (send_queue.size() == 2)
        {
            socket->AsyncWrite(data1, std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
            //asio::async_write(socket, data, std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
        }
    }
}

void NetSocket::Client::HandleSend(const asio::error_code& error, size_t bytes_transferred, uint8_t *send_buffer)
{
    // write complete
    send_queue.pop_front();
    if (send_queue.size() > 0)
    {
        socket->AsyncWrite(send_queue.front(), std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, (uint8_t*)send_queue.front().data()));
    }
    delete[] send_buffer;
}

void NetSocket::Client::Receive()
{
    asio::mutable_buffer data = asio::buffer(receive_header, 5);
    socket->AsyncRead(data, std::bind(&Client::HandleReceiveHeader, this, std::placeholders::_1, std::placeholders::_2));
    //asio::async_read(socket, asio::buffer(receive_header, 5), std::bind(&Client::HandleReceiveHeader, this, std::placeholders::_1, std::placeholders::_2));
}

void NetSocket::Client::HandleReceiveHeader(const asio::error_code& error, size_t bytes_transferred)
{
    //if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
    if (error)
    {
        if (disconnect_callback) disconnect_callback(*this);
    }
    else
    {
        DataType receive_type = static_cast<DataType>(receive_header[0]);
        receive_size = ntohl(*((uint32_t*)(receive_header + 1)));
        receive_data = new uint8_t[receive_size];
        if (receive_type == DataType::String)
        {
            asio::mutable_buffer data = asio::buffer(receive_data, receive_size);
            socket->AsyncRead(data, std::bind(&Client::HandleReceiveStringData, this, std::placeholders::_1, std::placeholders::_2));
            //asio::async_read(socket, asio::buffer(receive_data, receive_size), std::bind(&Client::HandleReceiveStringData, this, std::placeholders::_1, std::placeholders::_2));
        }
        else
        {
            asio::mutable_buffer data = asio::buffer(receive_data, receive_size);
            socket->AsyncRead(data, std::bind(&Client::HandleReceiveBinaryData, this, std::placeholders::_1, std::placeholders::_2));
            //asio::async_read(socket, asio::buffer(receive_data, receive_size), std::bind(&Client::HandleReceiveBinaryData, this, std::placeholders::_1, std::placeholders::_2));
        }
    }
}

void NetSocket::Client::HandleReceiveStringData(const asio::error_code& error, size_t bytes_transferred)
{
    //if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
    if (error)
    {
        delete[] receive_data;
        if (disconnect_callback) disconnect_callback(*this);
    }
    else
    {
        std::string receive_string = std::string(reinterpret_cast<char*>(receive_data), receive_size);
        delete[] receive_data;
        if (receive_string_callback) receive_string_callback(*this, receive_string);

        Receive();
    }
}

void NetSocket::Client::HandleReceiveBinaryData(const asio::error_code& error, size_t bytes_transferred)
{
    //if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
    if (error)
    {
        delete[] receive_data;
        if (disconnect_callback) disconnect_callback(*this);
    }
    else
    {
        if (receive_binary_callback) receive_binary_callback(*this, receive_data, receive_size);
        else delete[] receive_data;

        Receive();
    }
}

void NetSocket::Client::ConnectCallback(std::function<void(Client&)> callback)
{
    connect_callback = callback;
}

void NetSocket::Client::DisconnectCallback(std::function<void(Client&)> callback)
{
    disconnect_callback = callback;
}

void NetSocket::Client::ReceiveStringCallback(std::function<void(Client&, std::string)> callback)
{
    receive_string_callback = callback;
}

void NetSocket::Client::ReceiveBinaryCallback(std::function<void(Client&, void*, uint32_t)> callback)
{
    receive_binary_callback = callback;
}

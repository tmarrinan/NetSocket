#include "netsocket/clientconnection.h"

NetSocket::ClientConnection::ClientConnection(Server *server, asio::io_service& io_service) :
    host(server),
    receive_string_callback(NULL),
    receive_binary_callback(NULL),
    disconnect_callback(NULL)
{
    socket = new NetSocket::BasicSocket(io_service);
}

NetSocket::ClientConnection::ClientConnection(Server *server, asio::io_service& io_service, asio::ssl::context& context) :
    host(server),
    receive_string_callback(NULL),
    receive_binary_callback(NULL),
    disconnect_callback(NULL)
{
    socket = new NetSocket::SecureSocket(io_service, context);
}

NetSocket::ClientConnection::Pointer NetSocket::ClientConnection::Create(Server *server, asio::io_service& io_service)
{
    return Pointer(new ClientConnection(server, io_service));
}

NetSocket::ClientConnection::Pointer NetSocket::ClientConnection::Create(Server *server, asio::io_service& io_service, asio::ssl::context& context)
{
    return Pointer(new ClientConnection(server, io_service, context));
}

NetSocket::Socket* NetSocket::ClientConnection::Socket()
{
    return socket;
}

std::string NetSocket::ClientConnection::Endpoint()
{
    return socket->GetRemoteEndpoint();
}

std::string NetSocket::ClientConnection::IpAddress()
{
    return socket->GetRemoteAddress();
}

uint16_t NetSocket::ClientConnection::Port()
{
    return socket->GetRemotePort();
}

void NetSocket::ClientConnection::Send(std::string message)
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
        socket->AsyncWrite(data, std::bind(&ClientConnection::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
        //asio::async_write(socket, data, std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
    }
}

void NetSocket::ClientConnection::Send(const void *message, uint32_t length, CopyMode mode)
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
            socket->AsyncWrite(data, std::bind(&ClientConnection::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
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
            socket->AsyncWrite(data1, std::bind(&ClientConnection::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
            //asio::async_write(socket, data, std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
        }
    }
}

void NetSocket::ClientConnection::HandleSend(const asio::error_code& error, size_t bytes_transferred, uint8_t *send_buffer)
{
    // write complete
    send_queue.pop_front();
    if (send_queue.size() > 0)
    {
        socket->AsyncWrite(send_queue.front(), std::bind(&ClientConnection::HandleSend, this, std::placeholders::_1, std::placeholders::_2, (uint8_t*)send_queue.front().data()));
    }
	delete[] send_buffer;
}

void NetSocket::ClientConnection::Receive()
{
    asio::mutable_buffer data = asio::buffer(receive_header, 5);
    socket->AsyncRead(data, std::bind(&ClientConnection::HandleReceiveHeader, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    //asio::async_read(socket.NativeSocket(), asio::buffer(receive_header, 5), std::bind(&ClientConnection::HandleReceiveHeader, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void NetSocket::ClientConnection::HandleReceiveHeader(const asio::error_code& error, size_t bytes_transferred)
{
    //if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
    if (error)
    {
        if (disconnect_callback) disconnect_callback(Endpoint());
    }
    else
    {
        DataType receive_type = static_cast<DataType>(receive_header[0]);
        receive_size = ntohl(*((uint32_t*)(receive_header + 1)));
        receive_data = new uint8_t[receive_size];
        if (receive_type == DataType::String)
        {
            asio::mutable_buffer data = asio::buffer(receive_data, receive_size);
            socket->AsyncRead(data, std::bind(&ClientConnection::HandleReceiveStringData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
            //asio::async_read(socket.NativeSocket(), asio::buffer(receive_data, receive_size), std::bind(&ClientConnection::HandleReceiveStringData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
        else {
            asio::mutable_buffer data = asio::buffer(receive_data, receive_size);
            socket->AsyncRead(data, std::bind(&ClientConnection::HandleReceiveBinaryData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
            //asio::async_read(socket.NativeSocket(), asio::buffer(receive_data, receive_size), std::bind(&ClientConnection::HandleReceiveBinaryData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
    }
}

void NetSocket::ClientConnection::HandleReceiveStringData(const asio::error_code& error, size_t bytes_transferred)
{
    //if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
    if (error)
    {
        delete[] receive_data;
        if (disconnect_callback) disconnect_callback(Endpoint());
    }
    else
    {
        std::string receive_string = std::string(reinterpret_cast<char*>(receive_data), receive_size);
        delete[] receive_data;
        if (receive_string_callback) receive_string_callback(*host, shared_from_this(), receive_string);

        Receive();
    }
}

void NetSocket::ClientConnection::HandleReceiveBinaryData(const asio::error_code& error, size_t bytes_transferred)
{
    //if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
    if (error)
    {
        delete[] receive_data;
        if (disconnect_callback) disconnect_callback(Endpoint());
    }
    else
    {
        if (receive_binary_callback) receive_binary_callback(*host, shared_from_this(), receive_data, receive_size);
        else delete[] receive_data;

        Receive();
    }
}

void NetSocket::ClientConnection::ReceiveStringCallback(std::function<void(Server&, Pointer, std::string)> callback)
{
    receive_string_callback = callback;
}

void NetSocket::ClientConnection::ReceiveBinaryCallback(std::function<void(Server&, Pointer, void*, uint32_t)> callback)
{
    receive_binary_callback = callback;
}

void NetSocket::ClientConnection::DisconnectCallback(std::function<void(std::string)> callback)
{
    disconnect_callback = callback;
}
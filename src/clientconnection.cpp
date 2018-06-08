#include "netsocket/clientconnection.h"

NetSocket::ClientConnection::ClientConnection(Server *server, asio::io_service& io_service) :
    host(server),
    socket(io_service),
    receive_string_callback(NULL),
    receive_binary_callback(NULL),
    disconnect_callback(NULL)
{
}

NetSocket::ClientConnection::Pointer NetSocket::ClientConnection::Create(Server *server, asio::io_service& io_service)
{
    return Pointer(new ClientConnection(server, io_service));
}

asio::ip::tcp::socket& NetSocket::ClientConnection::Socket()
{
    return socket;
}

std::string NetSocket::ClientConnection::Endpoint()
{
    return IpAddress() + ":" + std::to_string(Port());
}

std::string NetSocket::ClientConnection::IpAddress()
{
    return socket.remote_endpoint().address().to_string();
}

uint16_t NetSocket::ClientConnection::Port()
{
    return socket.remote_endpoint().port();
}

void NetSocket::ClientConnection::Send(std::string message)
{
    asio::async_write(socket, asio::buffer(message), std::bind(&ClientConnection::HandleSend, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void NetSocket::ClientConnection::Send(const void *message, uint32_t length)
{
    asio::async_write(socket, asio::buffer(message, length), std::bind(&ClientConnection::HandleSend, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void NetSocket::ClientConnection::HandleSend(const asio::error_code& error, size_t bytes_transferred)
{
    // write complete
    std::cout << "Write complete" << std::endl;
}

void NetSocket::ClientConnection::Receive()
{
    asio::async_read(socket, asio::buffer(receive_header, 5), std::bind(&ClientConnection::HandleReceiveHeader, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void NetSocket::ClientConnection::HandleReceiveHeader(const asio::error_code& error, size_t bytes_transferred)
{
    if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
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
            asio::async_read(socket, asio::buffer(receive_data, receive_size), std::bind(&ClientConnection::HandleReceiveStringData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
        else {
            asio::async_read(socket, asio::buffer(receive_data, receive_size), std::bind(&ClientConnection::HandleReceiveBinaryData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
    }
}

void NetSocket::ClientConnection::HandleReceiveStringData(const asio::error_code& error, size_t bytes_transferred)
{
    if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
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
    if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
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
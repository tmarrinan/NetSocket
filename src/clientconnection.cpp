#include "netsocket/clientconnection.h"

NetSocket::ClientConnection::ClientConnection(Server *server, asio::io_service& io_service) :
    host(server),
    socket(io_service),
    read_callback(NULL),
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

void NetSocket::ClientConnection::Write(std::string message)
{
    asio::async_write(socket, asio::buffer(message), std::bind(&ClientConnection::HandleWrite, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void NetSocket::ClientConnection::HandleWrite(const asio::error_code& error, size_t bytes_transferred)
{
    // write complete
    std::cout << "Write complete" << std::endl;
}

void NetSocket::ClientConnection::Read()
{
    asio::async_read(socket, asio::buffer(&read_size, 4), std::bind(&ClientConnection::HandleReadHeader, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void NetSocket::ClientConnection::HandleReadHeader(const asio::error_code& error, size_t bytes_transferred)
{
    if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
    {
        if (disconnect_callback) disconnect_callback(Endpoint());
    }
    else
    {
        read_size = ntohl(read_size);
        read_data = new uint8_t[read_size];
        asio::async_read(socket, asio::buffer(read_data, read_size), std::bind(&ClientConnection::HandleReadData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
    }
}

void NetSocket::ClientConnection::HandleReadData(const asio::error_code& error, size_t bytes_transferred)
{
    if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
    {
        if (disconnect_callback) disconnect_callback(Endpoint());
    }
    else
    {
        if (read_callback) read_callback(*host, shared_from_this(), read_data, read_size);
        else delete[] read_data;

        Read();
    }
}

void NetSocket::ClientConnection::ReadCallback(std::function<void(Server&, Pointer, void*, uint32_t)> callback)
{
    read_callback = callback;
}

void NetSocket::ClientConnection::DisconnectCallback(std::function<void(std::string)> callback)
{
    disconnect_callback = callback;
}
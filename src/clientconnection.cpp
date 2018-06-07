#include "netsocket/clientconnection.h"

NetSocket::ClientConnection::ClientConnection(asio::io_service& io_service) :
    socket(io_service),
    read_callback(NULL)
{
}

NetSocket::ClientConnection::pointer NetSocket::ClientConnection::Create(asio::io_service& io_service)
{
    return pointer(new ClientConnection(io_service));
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
    read_size = ntohl(read_size);
    read_data = new uint8_t[read_size];
    asio::async_read(socket, asio::buffer(read_data, read_size), std::bind(&ClientConnection::HandleReadData, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void NetSocket::ClientConnection::HandleReadData(const asio::error_code& error, size_t bytes_transferred)
{
    if (read_callback) read_callback(shared_from_this(), read_data, read_size);
    else delete[] read_data;

    Read();
}

void NetSocket::ClientConnection::ReadCallback(void (*callback)(pointer connection, void *data, uint32_t length))
{
    read_callback = callback;
}
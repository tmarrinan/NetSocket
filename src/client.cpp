#include "netsocket/client.h"

NetSocket::Client::Client(std::string host, uint16_t port) :
    socket(io_service),
    receive_string_callback(NULL),
    receive_binary_callback(NULL),
    disconnect_callback(NULL)
{
    tcp::resolver resolver(io_service);
    tcp::resolver::query query(host, std::to_string(port));
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    asio::connect(socket, endpoint_iterator);

    Receive();
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
    uint8_t header[5];
    header[0] = static_cast<uint8_t>(DataType::String);
    uint32_t length = message.length() + 1;
    uint32_t size = htonl(static_cast<uint32_t>(length));
    memcpy(header + 1, &size, 4);

    uint8_t *buffer = new uint8_t[length];
    memcpy(buffer, message.c_str(), length);

    std::vector<asio::const_buffer> data;
    data.push_back(asio::buffer(header, 5));
    data.push_back(asio::buffer(buffer, length));

    asio::async_write(socket, data, std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
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

        asio::async_write(socket, asio::buffer(buffer, 5  +length), std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
    }
    else
    {
        uint8_t *buffer = new uint8_t[5];
        buffer[0] = static_cast<uint8_t>(DataType::Binary);
        uint32_t size = htonl(length);
        memcpy(buffer+1, &size, 4);
        std::vector<asio::const_buffer> data;
        data.push_back(asio::buffer(buffer, 5));
        data.push_back(asio::buffer(message, length));

        asio::async_write(socket, data, std::bind(&Client::HandleSend, this, std::placeholders::_1, std::placeholders::_2, buffer));
    }
}

void NetSocket::Client::HandleSend(const asio::error_code& error, size_t bytes_transferred, uint8_t *send_buffer)
{
    // write complete
    std::cout << "Write complete" << std::endl;
    delete[] send_buffer;
}

void NetSocket::Client::Receive()
{
    asio::async_read(socket, asio::buffer(receive_header, 5), std::bind(&Client::HandleReceiveHeader, this, std::placeholders::_1, std::placeholders::_2));
}

void NetSocket::Client::HandleReceiveHeader(const asio::error_code& error, size_t bytes_transferred)
{
    if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
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
            asio::async_read(socket, asio::buffer(receive_data, receive_size), std::bind(&Client::HandleReceiveStringData, this, std::placeholders::_1, std::placeholders::_2));
        }
        else {
            asio::async_read(socket, asio::buffer(receive_data, receive_size), std::bind(&Client::HandleReceiveBinaryData, this, std::placeholders::_1, std::placeholders::_2));
        }
    }
}

void NetSocket::Client::HandleReceiveStringData(const asio::error_code& error, size_t bytes_transferred)
{
    if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
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
    if (error == asio::error::eof || error == asio::error::connection_reset) // disconnect
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

void NetSocket::Client::ReceiveStringCallback(std::function<void(Client&, std::string)> callback)
{
    receive_string_callback = callback;
}

void NetSocket::Client::ReceiveBinaryCallback(std::function<void(Client&, void*, uint32_t)> callback)
{
    receive_binary_callback = callback;
}

void NetSocket::Client::DisconnectCallback(std::function<void(Client&)> callback)
{
    disconnect_callback = callback;
}

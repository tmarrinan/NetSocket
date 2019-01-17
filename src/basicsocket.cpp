#include "netsocket/basicsocket.h"

NetSocket::BasicSocket::BasicSocket(asio::io_service& io_service) :
    basic_socket(io_service)
{
}

void NetSocket::BasicSocket::EnableTcpNoDelay(bool tcp_no_delay)
{
    if (tcp_no_delay)
    {
        asio::ip::tcp::no_delay option(true);
        basic_socket.set_option(option);
    }
}

void NetSocket::BasicSocket::SetSendBufferSize(int send_buf_size)
{
    asio::socket_base::send_buffer_size option(send_buf_size);
    basic_socket.set_option(option);

    asio::socket_base::send_buffer_size get_option;
    basic_socket.get_option(get_option);
    int size = get_option.value();
    printf("send_buffer_size: %d\n", size);
}

void NetSocket::BasicSocket::SetRecvBufferSize(int recv_buf_size)
{
    asio::socket_base::receive_buffer_size option(recv_buf_size);
    basic_socket.set_option(option);

    asio::socket_base::receive_buffer_size get_option;
    basic_socket.get_option(get_option);
    int size = get_option.value();
    printf("receive_buffer_size: %d\n", size);
}

std::string NetSocket::BasicSocket::GetRemoteEndpoint()
{
    return GetRemoteAddress() + ":" + std::to_string(GetRemotePort());
}

std::string NetSocket::BasicSocket::GetRemoteAddress()
{
    return basic_socket.remote_endpoint().address().to_string();
}

uint16_t NetSocket::BasicSocket::GetRemotePort()
{
    return basic_socket.remote_endpoint().port();
}

std::string NetSocket::BasicSocket::GetLocalEndpoint()
{
    return GetLocalAddress() + ":" + std::to_string(GetLocalPort());
}

std::string NetSocket::BasicSocket::GetLocalAddress()
{
    return basic_socket.local_endpoint().address().to_string();
}

uint16_t NetSocket::BasicSocket::GetLocalPort()
{
    return basic_socket.local_endpoint().port();
}

void NetSocket::BasicSocket::AsyncConnect(tcp::resolver::iterator endpoint_iterator, std::function<void(const asio::error_code&, tcp::resolver::iterator)> complete)
{
    asio::async_connect(basic_socket, endpoint_iterator, complete);
}

void NetSocket::BasicSocket::AsyncAccept(tcp::acceptor& acceptor, std::function<void(const asio::error_code&)> complete)
{
    acceptor.async_accept(basic_socket, complete);
}

void NetSocket::BasicSocket::AsyncHandshake(asio::ssl::stream_base::handshake_type type, std::function<void(const asio::error_code&)> complete)
{
    asio::error_code err;
    err.assign(0, std::generic_category());
    complete(err);
}

void NetSocket::BasicSocket::AsyncWrite(asio::const_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete)
{
    asio::async_write(basic_socket, buffer, complete);
}

void NetSocket::BasicSocket::AsyncWrite(std::vector<asio::const_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete)
{
    asio::async_write(basic_socket, buffers, complete);
}

void NetSocket::BasicSocket::AsyncRead(asio::mutable_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete)
{
    asio::async_read(basic_socket, buffer, complete);
}

void NetSocket::BasicSocket::AsyncRead(std::vector<asio::mutable_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete)
{
    asio::async_read(basic_socket, buffers, complete);
}

#include "netsocket/socket.h"

NetSocket::Socket::Socket()
{
}

void NetSocket::Socket::EnableTcpNoDelay(bool tcp_no_delay)
{
}

std::string NetSocket::Socket::GetRemoteEndpoint()
{
    return "";
}

std::string NetSocket::Socket::GetRemoteAddress()
{
    return "";
}

uint16_t NetSocket::Socket::GetRemotePort()
{
    return 0;
}

std::string NetSocket::Socket::GetLocalEndpoint()
{
    return "";
}

std::string NetSocket::Socket::GetLocalAddress()
{
    return "";
}

uint16_t NetSocket::Socket::GetLocalPort()
{
    return 0;
}

void NetSocket::Socket::AsyncConnect(tcp::resolver::iterator endpoint_iterator, std::function<void(const asio::error_code&, tcp::resolver::iterator)> complete)
{
}

void NetSocket::Socket::AsyncAccept(tcp::acceptor& acceptor, std::function<void(const asio::error_code&)> complete)
{
}

void NetSocket::Socket::AsyncHandshake(asio::ssl::stream_base::handshake_type type, std::function<void(const asio::error_code&)> complete)
{
}

void NetSocket::Socket::AsyncWrite(asio::const_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete)
{
}

void NetSocket::Socket::AsyncWrite(std::vector<asio::const_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete)
{
}

void NetSocket::Socket::AsyncRead(asio::mutable_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete)
{
}

void NetSocket::Socket::AsyncRead(std::vector<asio::mutable_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete)
{
}

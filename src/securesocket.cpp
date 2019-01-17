#include "netsocket/securesocket.h"

NetSocket::SecureSocket::SecureSocket(asio::io_service& io_service, asio::ssl::context& context) :
    secure_socket(io_service, context)
{
}

NetSocket::SecureSocket::SecureSocket(asio::io_service& io_service, asio::ssl::context& context, std::function<bool(bool, asio::ssl::verify_context&)> verify) :
    secure_socket(io_service, context)
{
    //secure_socket.set_verify_mode(asio::ssl::verify_peer | asio::ssl::verify_fail_if_no_peer_cert);
    secure_socket.set_verify_mode(asio::ssl::verify_none);
    secure_socket.set_verify_callback(verify);
}

void NetSocket::SecureSocket::EnableTcpNoDelay(bool tcp_no_delay)
{
    if (tcp_no_delay)
    {
        asio::ip::tcp::no_delay option(true);
        secure_socket.lowest_layer().set_option(option);
    }
}

void NetSocket::SecureSocket::SetSendBufferSize(int send_buf_size)
{
    asio::socket_base::send_buffer_size option(send_buf_size);
    secure_socket.lowest_layer().set_option(option);

    asio::socket_base::send_buffer_size get_option;
    secure_socket.lowest_layer().get_option(get_option);
    int size = get_option.value();
    printf("send_buffer_size: %d\n", size);
}

void NetSocket::SecureSocket::SetRecvBufferSize(int recv_buf_size)
{
    asio::socket_base::receive_buffer_size option(recv_buf_size);
    secure_socket.lowest_layer().set_option(option);

    asio::socket_base::receive_buffer_size get_option;
    secure_socket.lowest_layer().get_option(get_option);
    int size = get_option.value();
    printf("receive_buffer_size: %d\n", size);
}

std::string NetSocket::SecureSocket::GetRemoteEndpoint()
{
    return GetRemoteAddress() + ":" + std::to_string(GetRemotePort());
}

std::string NetSocket::SecureSocket::GetRemoteAddress()
{
    return secure_socket.lowest_layer().remote_endpoint().address().to_string();
}

uint16_t NetSocket::SecureSocket::GetRemotePort()
{
    return secure_socket.lowest_layer().remote_endpoint().port();
}

std::string NetSocket::SecureSocket::GetLocalEndpoint()
{
    return GetLocalAddress() + ":" + std::to_string(GetLocalPort());
}

std::string NetSocket::SecureSocket::GetLocalAddress()
{
    return secure_socket.lowest_layer().local_endpoint().address().to_string();
}

uint16_t NetSocket::SecureSocket::GetLocalPort()
{
    return secure_socket.lowest_layer().local_endpoint().port();
}

void NetSocket::SecureSocket::AsyncConnect(tcp::resolver::iterator endpoint_iterator, std::function<void(const asio::error_code&, tcp::resolver::iterator)> complete)
{
    asio::async_connect(secure_socket.lowest_layer(), endpoint_iterator, complete);
}

void NetSocket::SecureSocket::AsyncAccept(tcp::acceptor& acceptor, std::function<void(const asio::error_code&)> complete)
{
    acceptor.async_accept(secure_socket.lowest_layer(), complete);
}

void NetSocket::SecureSocket::AsyncHandshake(asio::ssl::stream_base::handshake_type type, std::function<void(const asio::error_code&)> complete)
{
    secure_socket.async_handshake(type, complete);
}

void NetSocket::SecureSocket::AsyncWrite(asio::const_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete)
{
    asio::async_write(secure_socket, buffer, complete);
}

void NetSocket::SecureSocket::AsyncWrite(std::vector<asio::const_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete)
{
    asio::async_write(secure_socket, buffers, complete);
}

void NetSocket::SecureSocket::AsyncRead(asio::mutable_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete)
{
    asio::async_read(secure_socket, buffer, complete);
}

void NetSocket::SecureSocket::AsyncRead(std::vector<asio::mutable_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete)
{
    asio::async_read(secure_socket, buffers, complete);
}

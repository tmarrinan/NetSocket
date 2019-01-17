#ifndef __NETSOCKET_SECURESOCKET_H_
#define __NETSOCKET_SECURESOCKET_H_

#include "netsocket/socket.h"

class NetSocket::SecureSocket : public NetSocket::Socket
{
private:
    SslSocket secure_socket;
public:
    NETSOCKET_EXPORT SecureSocket(asio::io_service& io_service, asio::ssl::context& context);
    NETSOCKET_EXPORT SecureSocket(asio::io_service& io_service, asio::ssl::context& context, std::function<bool(bool, asio::ssl::verify_context&)> verify);
    NETSOCKET_EXPORT void EnableTcpNoDelay(bool tcp_no_delay);
    NETSOCKET_EXPORT void SetSendBufferSize(int send_buf_size);
    NETSOCKET_EXPORT void SetRecvBufferSize(int recv_buf_size);
    NETSOCKET_EXPORT std::string GetRemoteEndpoint();
    NETSOCKET_EXPORT std::string GetRemoteAddress();
    NETSOCKET_EXPORT uint16_t GetRemotePort();
    NETSOCKET_EXPORT std::string GetLocalEndpoint();
    NETSOCKET_EXPORT std::string GetLocalAddress();
    NETSOCKET_EXPORT uint16_t GetLocalPort();
    NETSOCKET_EXPORT void AsyncConnect(tcp::resolver::iterator endpoint_iterator, std::function<void(const asio::error_code&, tcp::resolver::iterator)> complete);
    NETSOCKET_EXPORT void AsyncAccept(tcp::acceptor& acceptor, std::function<void(const asio::error_code&)> complete);
    NETSOCKET_EXPORT void AsyncHandshake(asio::ssl::stream_base::handshake_type type, std::function<void(const asio::error_code&)> complete);
    NETSOCKET_EXPORT void AsyncWrite(asio::const_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete);
    NETSOCKET_EXPORT void AsyncWrite(std::vector<asio::const_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete);
    NETSOCKET_EXPORT void AsyncRead(asio::mutable_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete);
    NETSOCKET_EXPORT void AsyncRead(std::vector<asio::mutable_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete);
};

#endif // __NETSOCKET_SECURESOCKET_H_
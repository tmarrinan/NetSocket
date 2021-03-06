#ifndef __NETSOCKET_BASICSOCKET_H_
#define __NETSOCKET_BASICSOCKET_H_

#include "netsocket/socket.h"

class NetSocket::BasicSocket : public NetSocket::Socket
{
private:
    tcp::socket basic_socket;
public:
    NETSOCKET_EXPORT BasicSocket(asio::io_service& io_service);
    NETSOCKET_EXPORT std::string GetRemoteEndpoint();
    NETSOCKET_EXPORT std::string GetRemoteAddress();
    NETSOCKET_EXPORT uint16_t GetRemotePort();
    NETSOCKET_EXPORT void AsyncConnect(tcp::resolver::iterator endpoint_iterator, std::function<void(const asio::error_code&, tcp::resolver::iterator)> complete);
    NETSOCKET_EXPORT void AsyncAccept(tcp::acceptor& acceptor, std::function<void(const asio::error_code&)> complete);
    NETSOCKET_EXPORT void AsyncHandshake(asio::ssl::stream_base::handshake_type type, std::function<void(const asio::error_code&)> complete);
    NETSOCKET_EXPORT void AsyncWrite(asio::const_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete);
    NETSOCKET_EXPORT void AsyncWrite(std::vector<asio::const_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete);
    NETSOCKET_EXPORT void AsyncRead(asio::mutable_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete);
    NETSOCKET_EXPORT void AsyncRead(std::vector<asio::mutable_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete);
};

#endif // __NETSOCKET_BASICSOCKET_H_
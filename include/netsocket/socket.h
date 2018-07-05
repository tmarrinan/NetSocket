#ifndef __NETSOCKET_SOCKET_H_
#define __NETSOCKET_SOCKET_H_

#include "netsocket.h"

class NetSocket::Socket
{
public:
    NETSOCKET_EXPORT Socket();
    NETSOCKET_EXPORT virtual std::string GetRemoteEndpoint();
    NETSOCKET_EXPORT virtual std::string GetRemoteAddress();
    NETSOCKET_EXPORT virtual uint16_t GetRemotePort();
    NETSOCKET_EXPORT virtual void AsyncConnect(tcp::resolver::iterator endpoint_iterator, std::function<void(const asio::error_code&, tcp::resolver::iterator)> complete);
    NETSOCKET_EXPORT virtual void AsyncAccept(tcp::acceptor& acceptor, std::function<void(const asio::error_code&)> complete);
    NETSOCKET_EXPORT virtual void AsyncHandshake(asio::ssl::stream_base::handshake_type type, std::function<void(const asio::error_code&)> complete);
    NETSOCKET_EXPORT virtual void AsyncWrite(asio::const_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete);
    NETSOCKET_EXPORT virtual void AsyncWrite(std::vector<asio::const_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete);
    NETSOCKET_EXPORT virtual void AsyncRead(asio::mutable_buffer& buffer, std::function<void(const asio::error_code&, size_t)> complete);
    NETSOCKET_EXPORT virtual void AsyncRead(std::vector<asio::mutable_buffer>& buffers, std::function<void(const asio::error_code&, size_t)> complete);
};


#endif // __NETSOCKET_SOCKET_H_
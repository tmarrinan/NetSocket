#ifndef __NETSOCKET_CLIENTCONNECTION_H__
#define __NETSOCKET_CLIENTCONNECTION_H__

#include <arpa/inet.h>
#include "netsocket.h"

class NetSocket::ClientConnection : public std::enable_shared_from_this<NetSocket::ClientConnection>
{
public:
    typedef std::shared_ptr<NetSocket::ClientConnection> pointer;

private:
    tcp::socket socket;
    uint32_t read_size;
    uint8_t *read_data;
    void (*read_callback)(pointer connection, void *data, uint32_t length);

    ClientConnection(asio::io_service& io_service);
    void HandleWrite(const asio::error_code& error, size_t bytes_transferred);
    void HandleReadHeader(const asio::error_code& error, size_t bytes_transferred);
    void HandleReadData(const asio::error_code& error, size_t bytes_transferred);

public:
    static pointer Create(asio::io_service& io_service);
    tcp::socket& Socket();
    std::string Endpoint();
    std::string IpAddress();
    uint16_t Port();
    void Write(std::string message);
    void Read();
    void ReadCallback(void (*callback)(pointer connection, void *data, uint32_t length));
};

#endif // __NETSOCKET_CLIENTCONNECTION_H__
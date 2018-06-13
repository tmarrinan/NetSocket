#ifndef __NETSOCKET_CLIENTCONNECTION_H__
#define __NETSOCKET_CLIENTCONNECTION_H__

#ifdef _WIN32
    #include <Winsock2.h>
#else
    #include <arpa/inet.h>
#endif
#include "netsocket.h"

class NetSocket::ClientConnection : public std::enable_shared_from_this<NetSocket::ClientConnection>
{
public:
    typedef std::shared_ptr<NetSocket::ClientConnection> Pointer;

private:
    Server *host;
    tcp::socket socket;
    uint8_t receive_header[5];
    uint32_t receive_size;
    uint8_t *receive_data;
    std::function<void(Server&, Pointer, std::string)> receive_string_callback;
    std::function<void(Server&, Pointer, void*, uint32_t)> receive_binary_callback;
    std::function<void(std::string)> disconnect_callback;

    ClientConnection(Server *server, asio::io_service& io_service);
    void HandleSend(const asio::error_code& error, size_t bytes_transferred, uint8_t *send_buffer);
    void HandleReceiveHeader(const asio::error_code& error, size_t bytes_transferred);
    void HandleReceiveStringData(const asio::error_code& error, size_t bytes_transferred);
    void HandleReceiveBinaryData(const asio::error_code& error, size_t bytes_transferred);

public:
    NETSOCKET_EXPORT static Pointer Create(Server *server, asio::io_service& io_service);
    NETSOCKET_EXPORT tcp::socket& Socket();
    NETSOCKET_EXPORT std::string Endpoint();
    NETSOCKET_EXPORT std::string IpAddress();
    NETSOCKET_EXPORT uint16_t Port();
    NETSOCKET_EXPORT void Send(std::string message);
    NETSOCKET_EXPORT void Send(const void *message, uint32_t length);
    NETSOCKET_EXPORT void Receive();
    NETSOCKET_EXPORT void ReceiveStringCallback(std::function<void(Server&, Pointer, std::string)> callback);
    NETSOCKET_EXPORT void ReceiveBinaryCallback(std::function<void(Server&, Pointer, void*, uint32_t)> callback);
    NETSOCKET_EXPORT void DisconnectCallback(std::function<void(std::string)> callback);
};

#endif // __NETSOCKET_CLIENTCONNECTION_H__
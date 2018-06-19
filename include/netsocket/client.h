#ifndef __NETSOCKET_CLIENT_H_
#define __NETSOCKET_CLIENT_H_

#ifdef _WIN32
    #include <Winsock2.h>
#else
    #include <arpa/inet.h>
#endif
#include "netsocket.h"

class NetSocket::Client {
private:
    asio::io_service io_service;
    tcp::socket socket;
    Protocol network_protocol;
    uint8_t receive_header[5];
    uint32_t receive_size;
    uint8_t *receive_data;
    std::function<void(Client&, std::string)> receive_string_callback;
    std::function<void(Client&, void*, uint32_t)> receive_binary_callback;
    std::function<void(Client&)> disconnect_callback;

    void HandleSend(const asio::error_code& error, size_t bytes_transferred, uint8_t *send_buffer);
    void HandleReceiveHeader(const asio::error_code& error, size_t bytes_transferred);
    void HandleReceiveStringData(const asio::error_code& error, size_t bytes_transferred);
    void HandleReceiveBinaryData(const asio::error_code& error, size_t bytes_transferred);

public:
    NETSOCKET_EXPORT Client(std::string host, uint16_t port, Protocol protocol);
    NETSOCKET_EXPORT void Run();
    NETSOCKET_EXPORT void Poll();
    NETSOCKET_EXPORT void Send(std::string message);
    NETSOCKET_EXPORT void Send(const void *message, uint32_t length, CopyMode mode);
    NETSOCKET_EXPORT void Receive();
    NETSOCKET_EXPORT void ReceiveStringCallback(std::function<void(Client&, std::string)> callback);
    NETSOCKET_EXPORT void ReceiveBinaryCallback(std::function<void(Client&, void*, uint32_t)> callback);
    NETSOCKET_EXPORT void DisconnectCallback(std::function<void(Client&)> callback);
};

#endif // __NETSOCKET_CLIENT_H_
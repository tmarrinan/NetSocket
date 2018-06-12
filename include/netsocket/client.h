#ifndef __NETSOCKET_CLIENT_H_
#define __NETSOCKET_CLIENT_H_

#include "netsocket.h"

class NetSocket::Client {
private:
    asio::io_service io_service;
    tcp::socket socket;
    uint8_t receive_header[5];
    uint32_t receive_size;
    uint8_t *receive_data;
    std::function<void(Client&, std::string)> receive_string_callback;
    std::function<void(Client&, void*, uint32_t)> receive_binary_callback;
    std::function<void(Client&)> disconnect_callback;

    void HandleReceiveHeader(const asio::error_code& error, size_t bytes_transferred);
    void HandleReceiveStringData(const asio::error_code& error, size_t bytes_transferred);
    void HandleReceiveBinaryData(const asio::error_code& error, size_t bytes_transferred);

public:
    Client(std::string host, uint16_t port);
    void Run();
    void Poll();
    void Receive();
    void ReceiveStringCallback(std::function<void(Client&, std::string)> callback);
    void ReceiveBinaryCallback(std::function<void(Client&, void*, uint32_t)> callback);
    void DisconnectCallback(std::function<void(Client&)> callback);
};

#endif // __NETSOCKET_CLIENT_H_
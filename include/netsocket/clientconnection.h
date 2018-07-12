#ifndef __NETSOCKET_CLIENTCONNECTION_H__
#define __NETSOCKET_CLIENTCONNECTION_H__

#ifdef _WIN32
    #include <Winsock2.h>
#else
    #include <arpa/inet.h>
#endif
#include "netsocket.h"
#include "netsocket/basicsocket.h"
#include "netsocket/securesocket.h"

class NetSocket::ClientConnection : public std::enable_shared_from_this<NetSocket::ClientConnection>
{
public:
    typedef std::shared_ptr<NetSocket::ClientConnection> Pointer;

private:
    typedef struct SendData {
        asio::const_buffer data;
        bool delete_on_completion;
    } SendData;

    Server *host;
    NetSocket::Socket *socket;
    bool use_callbacks;
    uint8_t receive_header[5];
    uint32_t receive_size;
    uint8_t *receive_data;
    std::list<SendData> send_queue;
    std::function<void(Server&, Pointer, std::string)> receive_string_callback;
    std::function<void(Server&, Pointer, void*, uint32_t)> receive_binary_callback;
    std::function<void(Pointer)> disconnect_callback;

    ClientConnection(Server *server, asio::io_service& io_service, bool callbacks);
    ClientConnection(Server *server, asio::io_service& io_service, asio::ssl::context& context, bool callbacks);
    void HandleSend(const asio::error_code& error, size_t bytes_transferred, SendData& send_data);
    void HandleReceiveHeader(const asio::error_code& error, size_t bytes_transferred);
    void HandleReceiveStringData(const asio::error_code& error, size_t bytes_transferred);
    void HandleReceiveBinaryData(const asio::error_code& error, size_t bytes_transferred);

public:
    NETSOCKET_EXPORT static Pointer Create(Server *server, asio::io_service& io_service, bool callbacks);
    NETSOCKET_EXPORT static Pointer Create(Server *server, asio::io_service& io_service, asio::ssl::context& context, bool callbacks);
    NETSOCKET_EXPORT NetSocket::Socket* Socket();
    NETSOCKET_EXPORT std::string Endpoint();
    NETSOCKET_EXPORT std::string IpAddress();
    NETSOCKET_EXPORT uint16_t Port();
    NETSOCKET_EXPORT void Send(std::string message);
    NETSOCKET_EXPORT void Send(const void *message, uint32_t length, CopyMode mode);
    NETSOCKET_EXPORT void Receive();
    NETSOCKET_EXPORT void ReceiveStringCallback(std::function<void(Server&, Pointer, std::string)> callback);
    NETSOCKET_EXPORT void ReceiveBinaryCallback(std::function<void(Server&, Pointer, void*, uint32_t)> callback);
    NETSOCKET_EXPORT void DisconnectCallback(std::function<void(Pointer)> callback);
};

#endif // __NETSOCKET_CLIENTCONNECTION_H__
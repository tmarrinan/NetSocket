#ifndef __NETSOCKET_SERVER_H_
#define __NETSOCKET_SERVER_H_

#include <map>
#include "netsocket.h"
#include "netsocket/clientconnection.h"

class NetSocket::Server {
public:
    enum EventType : uint8_t {None, Connect, Disconnect, ReceiveString, ReceiveBinary, SendFinished};
    typedef struct Event {
        EventType type;
        ClientConnection::Pointer client;
        std::string string_data;
        void *binary_data;
        uint32_t data_length;
    } Event;
private:
    asio::io_service io_service;
    tcp::acceptor acceptor;
    asio::ssl::context context;
    std::string key_passwd;
    bool secure;
    bool use_callbacks;
    bool tcp_no_delay;
    int send_buf_size;
    int recv_buf_size;
    std::list<Event> event_queue;
    std::map<std::string, ClientConnection::Pointer> clients;
    std::function<void(Server&, ClientConnection::Pointer)> connect_callback;
    std::function<void(Server&, ClientConnection::Pointer)> disconnect_callback;

    std::string GetPassword();
    void StartAccept();
    void HandleAccept(ClientConnection::Pointer new_connection, const asio::error_code& error);
    void HandleHandshake(ClientConnection::Pointer new_connection, const asio::error_code& error);
    void HandleDisconnect(ClientConnection::Pointer connection);
public:
    NETSOCKET_EXPORT Server(uint16_t port, ServerOptions& options);
    NETSOCKET_EXPORT Event WaitForNextEvent();
    NETSOCKET_EXPORT Event PollForNextEvent();
    NETSOCKET_EXPORT bool Alive();
    NETSOCKET_EXPORT int16_t NumClients();
    NETSOCKET_EXPORT void Run();
    NETSOCKET_EXPORT void Poll();
    NETSOCKET_EXPORT void Broadcast(std::string message);
    NETSOCKET_EXPORT void Broadcast(const void *message, uint32_t length, CopyMode mode);
    NETSOCKET_EXPORT void ConnectCallback(std::function<void(Server&, ClientConnection::Pointer)> callback);
    NETSOCKET_EXPORT void DisconnectCallback(std::function<void(Server&, ClientConnection::Pointer)> callback);

    NETSOCKET_EXPORT void ClientSendIsComplete(ClientConnection::Pointer connection, void *data, uint32_t length);
    NETSOCKET_EXPORT void ClientReceivedStringData(ClientConnection::Pointer connection, std::string data);
    NETSOCKET_EXPORT void ClientReceivedBinaryData(ClientConnection::Pointer connection, void *data, uint32_t length);
};

#endif // __NETSOCKET_SERVER_H_
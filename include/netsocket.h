#ifndef __NETSOCKET_H_
#define __NETSOCKET_H_

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <asio.hpp>
#include <asio/ssl.hpp>

#ifdef _WIN32
    #define NETSOCKET_EXPORT __declspec(dllexport)
#else
    #define NETSOCKET_EXPORT
#endif

namespace NetSocket {
    using namespace asio::ip;

    typedef struct ServerOptions {
        const char *server_cert;
        const char *dh_file;
        const char *encrypted_key_passwd;
        uint16_t flags;
        int send_buf_size;
        int recv_buf_size;
    } ServerOptions;
    typedef struct ClientOptions {
        bool secure;
        uint16_t flags;
        int send_buf_size;
        int recv_buf_size;
    } ClientOptions;
    typedef asio::ssl::stream<asio::ip::tcp::socket> SslSocket;

    enum DataType : uint8_t {String, Binary};
    enum CopyMode : uint8_t {MemCopy, ZeroCopy};
    enum GeneralFlags: uint16_t {None = 0x00, UseCallbacks = 0x01, TcpNoDelay = 0x02};
    enum ServerFlags : uint16_t {NoSslV2 = 0x04, NoSslV3 = 0x08, NoTlsV1 = 0x10, NoTlsV1_1 = 0x20, NoTlsV1_2 = 0x40};
    enum ClientFlags : uint16_t {VerifyPeer = 0x04};

    class Server;
    class Client;
    class ClientConnection;
    class Socket;
    class BasicSocket;
    class SecureSocket;

    NETSOCKET_EXPORT ServerOptions CreateServerOptions();
    NETSOCKET_EXPORT ClientOptions CreateClientOptions();
}

#endif // __NETSOCKET_H_
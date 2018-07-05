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

    typedef asio::ssl::stream<asio::ip::tcp::socket> SslSocket;

    enum class DataType : uint8_t {String, Binary};
    enum class CopyMode : uint8_t {MemCopy, ZeroCopy};

    class Server;
    class Client;
    class ClientConnection;
    class Socket;
    class BasicSocket;
    class SecureSocket;
}

#endif // __NETSOCKET_H_
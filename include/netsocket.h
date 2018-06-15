#ifndef __NETSOCKET_H_
#define __NETSOCKET_H_

#include <iostream>
#include <string>
#include <vector>
#include <asio.hpp>

#ifdef _WIN32
    #define NETSOCKET_EXPORT __declspec(dllexport)
#else
    #define NETSOCKET_EXPORT
#endif

namespace NetSocket {
    using namespace asio::ip;
    
    enum class DataType : uint8_t {String, Binary};
    enum class CopyMode : uint8_t {MemCopy, ZeroCopy};

    class Server;
    class Client;
    class ClientConnection;
}

#endif // __NETSOCKET_H_
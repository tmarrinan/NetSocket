#ifndef __NETSOCKET_H_
#define __NETSOCKET_H_

#include <iostream>
#include <string>
#include <asio.hpp>

namespace NetSocket {
    using namespace asio::ip;

    namespace DataType {
        enum DataType {String, Binary};
    }
    
    class Server;
    class Client;
    class ClientConnection;
}

#endif // __NETSOCKET_H_
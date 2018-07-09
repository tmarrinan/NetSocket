#include "netsocket.h"

NetSocket::ServerOptions NetSocket::CreateServerOptions()
{
    NetSocket::ServerOptions options = {NULL, NULL, NULL, 0};
    return options;
}

NetSocket::ClientOptions NetSocket::CreateClientOptions()
{
    ClientOptions options = {false, 0};
    return options;
}
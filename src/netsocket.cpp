#include "netsocket.h"

NetSocket::ServerOptions NetSocket::CreateServerOptions()
{
    NetSocket::ServerOptions options = {NULL, NULL, NULL, NetSocket::ServerFlags::UseCallbacks};
    return options;
}

NetSocket::ClientOptions NetSocket::CreateClientOptions()
{
    ClientOptions options = {false, 0};
    return options;
}
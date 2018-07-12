#include "netsocket.h"

NetSocket::ServerOptions NetSocket::CreateServerOptions()
{
    NetSocket::ServerOptions options = {NULL, NULL, NULL, NetSocket::GeneralFlags::None};
    return options;
}

NetSocket::ClientOptions NetSocket::CreateClientOptions()
{
    ClientOptions options = {false, NetSocket::GeneralFlags::None};
    return options;
}
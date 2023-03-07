#include <socket_wrapper/socket_class.h>
#include <socket_wrapper/socket_headers.h>

#include <utility>

#ifdef _WIN32
    constexpr auto close_type = SD_BOTH;
#   define close_socket closesocket
#else
    constexpr auto close_type = SHUT_RDWR;
#   define close_socket ::close
#endif


namespace socket_wrapper
{

Socket::Socket(int domain, int type, int protocol) : socket_descriptor_(INVALID_SOCKET)
{
    open(domain, type, protocol);
}


Socket::Socket(SocketDescriptorType socket_descriptor) : socket_descriptor_(socket_descriptor)
{
}


Socket::Socket(Socket &&s)
{
    socket_descriptor_ = s.socket_descriptor_;
    s.socket_descriptor_ = INVALID_SOCKET;
}


Socket &Socket::operator=(Socket &&s)
{
    if (&s == this) return *this;

    if (opened()) close();
    std::swap(socket_descriptor_, s.socket_descriptor_);

    return *this;
}


Socket::~Socket()
{
    if (opened()) close();
}


bool Socket::opened() const
{
    return socket_descriptor_ != INVALID_SOCKET;
}


void Socket::open(int domain, int type, int protocol)
{
    if (opened()) close();
    socket_descriptor_ = socket(domain, type, protocol);
}


int Socket::close()
{
    int status = 0;

    status = close_socket(socket_descriptor_);
    socket_descriptor_ = INVALID_SOCKET;

    return status;

}

}


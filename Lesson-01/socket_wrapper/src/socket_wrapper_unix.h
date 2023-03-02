#pragma once

#include <cerrno>
#include <cstring>
#include <string>

#include "socket_wrapper_impl.h"


namespace socket_wrapper
{

class SocketWrapperImpl : ISocketWrapperImpl
{
public:
  const size_t err_buffer_size = 256;

public:
    void initialize() {}
    bool initialized() const { return true; }
    void deinitialize() {}
    int get_last_error_code() const { return errno; }
    std::string get_last_error_string() const
    {
		std::string buffer(err_buffer_size, '\0');
#if (_POSIX_C_SOURCE >= 200112L) && ! _GNU_SOURCE
        ::strerror_r(get_last_error_code(), &buffer[0], buffer.size());
        return buffer;
#else
        return ::strerror_r(get_last_error_code(), &buffer[0], buffer.size());
#endif
    };
};

}


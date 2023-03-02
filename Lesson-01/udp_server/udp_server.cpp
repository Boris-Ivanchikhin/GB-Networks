#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <cstring>

#include "socket_wrapper/socket_headers.h"
#include "socket_wrapper/socket_wrapper.h"
#include "socket_wrapper/socket_class.h"

#include "handler.h"

// Trim from end (in place).
static inline std::string& rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int c) { return !std::isspace(c); }).base(), s.end());
    return s;
} // rtrim

// Function called at shutdown
void end_function()
{
    // Output of the shutdown message
    std::cout << "\nthe work is completed!" << std::endl;
} // end_function

// Main
int main(int argc, char const *argv[])
{
    // Russian language in the console
    setlocale (LC_ALL, "");

    // Registration of the function to be called during normal shutdown
    if (atexit (end_function))
    {
        // Checking the registration of the endFunction
        puts ("post- function registration error!");
    }

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    // queue of command handlers
    IHandler *handlers_queue = new DefaultHandler;
    handlers_queue = new ExitChecker(handlers_queue);


    socket_wrapper::SocketWrapper sock_wrap;
    const int port { std::stoi(argv[1]) };

    socket_wrapper::Socket sock = {AF_INET, SOCK_DGRAM, IPPROTO_UDP};

    std::cout << "Starting echo server on the port " << port << "...\n";

    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    sockaddr_in addr =
    {
        .sin_family = PF_INET,
        .sin_port = htons(port),
    };

    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)) != 0)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        // Socket will be closed in the Socket destructor.
        return EXIT_FAILURE;
    }

    char buffer[256];
    char name_buffer[NI_MAXHOST];

    // socket address used to store client address
    struct sockaddr_in client_address = {0};
    socklen_t client_address_len = sizeof(sockaddr_in);
    ssize_t recv_len = 0;

    std::cout << "Running echo server...\n" << std::endl;
    char client_address_buf[INET_ADDRSTRLEN];

    while (true)
    {
        // Read content into buffer from an incoming client.
        recv_len = recvfrom(sock, buffer, sizeof(buffer) - 1, 0,
                            reinterpret_cast<sockaddr *>(&client_address),
                            &client_address_len);

        if (recv_len > 0)
        {
            buffer[recv_len] = '\0';

            // *** Course: Network programming in C++
            // *** Lesson 01. Networking. Sockets and UDP.

            // 1. Дополните реализованный сервер обратным резолвом имени клиента.

            if (getnameinfo(reinterpret_cast<sockaddr *>(&client_address),
                            client_address_len, name_buffer, NI_MAXHOST,
                            nullptr, 0, NI_NAMEREQD))
                strcpy(name_buffer, "UNKNOWN");

            std::cout
                << "Client ["<< name_buffer << "] with address "
                << inet_ntop(AF_INET, &client_address.sin_addr, client_address_buf, sizeof(client_address_buf) / sizeof(client_address_buf[0]))
                << ":" << ntohs(client_address.sin_port)
                << " sent datagram "
                << "[length = "
                << recv_len
                << "]:\n'''\n"
                << buffer
                << "\n'''"
                << std::endl;
            //if ("exit" == command_string) run = false;
            //send(sock, &buf, readden, 0);

            // Send same content back to the client ("echo").
            sendto(sock, buffer, recv_len, 0, reinterpret_cast<const sockaddr *>(&client_address),
                   client_address_len);

            // *** Course: Network programming in C++
            // *** Lesson 01. Networking. Sockets and UDP.

            // 2. Дополните реализованный сервер так,
            //    чтобы он принимал команду “exit” и при её получении завершал работу.

            std::string command_string = {buffer, 0, (size_t)recv_len};
            rtrim(command_string);
            //std::cout << command_string << std::endl;

            if (handlers_queue->handle(command_string))
                break;

        }

        std::cout << std::endl;

    }

    // garbage collector
    delete handlers_queue;

    return EXIT_SUCCESS;
} // main


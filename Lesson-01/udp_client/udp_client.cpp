// *** Course: Network programming in C++
// *** Lesson 01. Networking. Sockets and UDP.

//  4. Реализуйте UDP-клиент для работы с сервером.
//     Client side implementation of UDP client-server model.

#include <bits/stdc++.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "socket_wrapper/socket_headers.h"
#include "socket_wrapper/socket_wrapper.h"
#include "socket_wrapper/socket_class.h"

#define MAXLINE 1024

// Function called at shutdown
void end_function()
{
    // Output of the shutdown message
    std::cout << "\nthe work is completed!" << std::endl;
} // end_function

// Driver code
int main(int argc, char const *argv[])
{
    // Russian language in the console
    setlocale (LC_ALL, "");

    // Registration of the function to be called during normal shutdown
    if (atexit (end_function))
    {
        // Checking the registration of the endFunction
        std::cout << "\npost- function registration error!" << std::endl;
    }

    if (argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        return EXIT_FAILURE;
    }

    socket_wrapper::SocketWrapper sock_wrap;
    const int port { std::stoi(argv[1]) };

    socket_wrapper::Socket sock = {AF_INET, SOCK_DGRAM, IPPROTO_UDP};
    if (!sock)
    {
        std::cerr << sock_wrap.get_last_error_string() << std::endl;
        return EXIT_FAILURE;
    }

    // Filling server information
    sockaddr_in servaddr =
    {
        .sin_family = PF_INET, //IPv4
        .sin_port = htons(port),
    };

    servaddr.sin_addr.s_addr = INADDR_ANY;

    //memset(&servaddr, 0, sizeof(servaddr));

    char buffer[MAXLINE];
    socklen_t len;
    std::string line;

    std::cout << ">";
    while (std::getline(std::cin, line))
    {
        if (line.length())
        {
            sendto(sock, line.c_str(), line.length(), 0,
                   reinterpret_cast<const sockaddr *>(&servaddr),
                   sizeof(servaddr));
            std::cout<<"message sent."<<std::endl;

            int n = recvfrom(sock, (char *)buffer, MAXLINE, 0,
                         reinterpret_cast<sockaddr *>(&servaddr), &len);
            if (n)
            {
                buffer[n] = '\0';
                std::cout << "Server :" << buffer << std::endl;
            }

        }

        std::cout << ">";
    }

    // return
    return EXIT_SUCCESS;
} // main


//
// *** Course: Network programming in C++
// *** Lesson 03. Network architectures. TCP sockets.
//
//  1. Переделайте сервер на TCP-сокеты.
//
//      P.S.
//      При получении сообщения от TCP-клиента определяется IP адрес и порт отпраителя.
//      Также производится попытка разрешить hostname.
//

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

#include <cstdlib>
#include <iostream>
#include <thread>
#include <utility>
#include <optional>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

using boost::asio::ip::tcp;
namespace po = boost::program_options;


// Declare a boost::error_info typedef that holds the stacktrace:
typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace> traced;
// Write a helper class for throwing any exception with stacktrace
template <class E>
void throw_with_trace(const E& e)
{
    throw boost::enable_error_info(e)
            << traced(boost::stacktrace::stacktrace());
}


const int max_length = 1024;

// Trim from end (in place).
static inline std::string& rtrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(),
            [](int c) { return !std::isspace(c); }).base(), s.end());
    return s;
} // rtrim

void session(tcp::socket sock)
{
    try
    {
        sock.set_option(tcp::socket::reuse_address(true));
        for (;;)
        {
            char data[max_length];

            boost::system::error_code error;
            size_t length = sock.read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
            //std::string msg {data, length};
            //rtrim(msg);

            auto sender_ep = sock.remote_endpoint();
            std::string sender_ip = sender_ep.address().to_string();
            auto sender_port = sender_ep.port();

            // Get host name from sender_ip
            boost::asio::io_context io_context;
            boost::asio::ip::tcp::resolver resolver(io_context);
            boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(sender_ep);

            std::string sender_name{};
            // Если список имен хостов не пустой, выводим первое имя
            if (iter != boost::asio::ip::tcp::resolver::iterator())
            {
                sender_name = iter->host_name();
            }
            else
            {
                sender_name = "error resolve";
            }

            std::cout << "Incoming message from " << sender_ip
                        << " (" << sender_name <<")"
                        << ", port " << sender_port << ".\n";
            std::cout << "length " << " bytes received." << std::endl;

            //std::cout << "Incoming message, " << length << " bytes received;"<<std::endl;
            boost::asio::write(sock, boost::asio::buffer(data, length));
            std::cout << "message forwarded." <<  std::endl;
        }
    }

    catch (std::exception& e)
    {
        std::cerr << "Exception in thread: " << e.what() << "\n";
        const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
        if (st)
        {
            std::cout << *st << '\n';
        }
    }

}

void server(boost::asio::io_context& io_context, unsigned short port)
{
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port));
    for (;;)
    {
        std::thread(session, a.accept()).detach();
    }
}

std::optional<po::variables_map> on_start (int argc, char* argv[], int port)
{
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("port", po::value<int>(&port)->default_value(port), "port number")
            ;

    po::positional_options_description pos_desc;
    pos_desc.add("port", -1);

    po::variables_map vm;
    try
    {
        //po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
        auto parsed = po::command_line_parser(argc, argv).
                                            options(desc).positional(pos_desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);
    }

    catch (po::error& e)
    {
        std::cout << e.what() << std::endl;
        std::cout << desc << std::endl;

        const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
        if (st)
        {
            std::cout << *st << '\n';
        }
        // return with error
        return std::nullopt;
    }

    if (vm.count("help") || argc != 2)
    {
        std::cout << desc << "\n";
    }

    // return success
    return vm;
}

int main(int argc, char* argv[])
{
    unsigned short port{1230};

    auto options = on_start (argc, argv, port);
    if (!options)
        return EXIT_FAILURE;

    auto vm = options.value();

    if (vm.count("help"))
        return EXIT_SUCCESS;

    if (vm.count("port"))
        port = vm["port"].as<int>();

    std::cout << "Port number was set to " << port << ".\n";

    boost::asio::io_context io_context;

    try
    {
        std::cout << "listening ...\n\n";
        server(io_context, port);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
        const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);
        if (st)
        {
            std::cout << *st << '\n';
        }
        // return with error
        return EXIT_FAILURE;
    }

    // return with success
    return EXIT_SUCCESS;
}

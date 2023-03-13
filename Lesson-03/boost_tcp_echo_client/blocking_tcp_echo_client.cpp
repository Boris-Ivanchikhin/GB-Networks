//
// *** Course: Network programming in C++
// *** Lesson 03. Network architectures. TCP sockets.
//
//  2. Переделайте клиент, отправляющий команду, на TCP.
//      Если на первом уроке вы не реализовывали UDP клиент,
//      реализуйте TCP клиент сейчас.
//
//  3. Если не было сделано ранее, добавьте в клиент возможность адресовать
//      сервер, как по адресу IPv4 и IPv6, так и по доменному имени.
//

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

//#define BOOST_BIND_GLOBAL_PLACEHOLDERS

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>
#include <boost/bind/bind.hpp>

using boost::asio::ip::tcp;
using boost::system::error_code;
namespace po = boost::program_options;

enum { max_length = 1024 };

size_t read_complete(char * buf, const error_code & err, size_t bytes)
{
    if ( err) return 0;
    bool found = std::find(buf, buf + bytes, '\n') < buf + bytes;
    // we read one-by-one until we get to enter, no buffering
    return found ? 0 : 1;
}

void msg_echo (tcp::socket &sock, std::string msg)
{
    using namespace boost::placeholders;

    msg += "\n";

    sock.write_some(boost::asio::buffer(msg));
    char buf[max_length]{'\0'};
    int bytes = read(sock, boost::asio::buffer(buf), boost::bind(read_complete,buf,_1,_2));
    std::string copy(buf, bytes - 1);
    msg.resize(msg.size() - 1);
    //msg = msg.substr(0, msg.size() - 1);
    std::cout << "server echoed our " << msg << ": "
              << (copy == msg ? "OK" : "FAIL") << std::endl;
}

std::optional<po::variables_map> on_start (int argc, char* argv[])
{
    // Declare the supported options.
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help", "produce help message")
            ("host,h", po::value<std::string>()->default_value("127.0.0.1"), "<host>")
            ("port,p", po::value<std::string>()->default_value("1230"), "<port>")
            ;

    //po::positional_options_description pos_desc;
    //pos_desc.add("port", -1);

    po::variables_map vm;
    try
    {
        //po::store(po::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
        auto parsed = po::command_line_parser(argc, argv).
                options(desc).allow_unregistered().run();
        po::store(parsed, vm);
        po::notify(vm);
    }

    catch (po::error& e)
    {
        std::cout << e.what() << std::endl;
        std::cout << desc << std::endl;

        // return with error
        return std::nullopt;
    }

    if (vm.count("help") || argc != 3)
    {
        std::cout << desc << "\n";
    }

    // return success
    return vm;
}

int main(int argc, char* argv[])
{

      std::string port{};
      std::string host{};

      auto options = on_start (argc, argv);
      if (!options)
          return EXIT_FAILURE;

      auto vm = options.value();

      if (vm.count("help"))
          return EXIT_SUCCESS;

      if (vm.count("host"))
        host = vm["host"].as<std::string>();
      if (vm.count("port"))
          port = vm["port"].as<std::string>();

      std::cout << "Host was set to " << host << ".\n";
      std::cout << "Port number was set to " << port << ".\n";

      boost::asio::io_context io_context;

      try
      {
          tcp::socket sock(io_context);
          tcp::resolver resolver(io_context);
          boost::asio::connect(sock, resolver.resolve(host, port));

          sock.set_option(boost::asio::ip::tcp::no_delay (true));

          while (true)
          {
              std::string request{};
              std::cout << "Enter message: " << std::flush;
              if (!std::getline(std::cin, request) || !request.length())
                  break;

              msg_echo (sock,request);
          }

          sock.close();
          std::cout << "\nSocket closed." << std::flush;

      }

      catch (std::exception& e)
      {
          std::cerr << "Exception: " << e.what() << "\n";
      }

  return EXIT_SUCCESS;
}

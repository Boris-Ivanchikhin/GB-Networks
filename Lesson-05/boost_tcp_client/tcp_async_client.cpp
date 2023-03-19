//
// *** Course: Network programming in C++
// *** Lesson 05. C++ and the network. Libraries. HTTP, Websocket.
//
//  2. Сделайте реализацию TCP-клиента из предыдущих уроков,
//      используя одну из разобранных библиотек, например, Qt или Boost.
//

/*
    Этот код реализует простого клиента TCP-сокета с использованием библиотеки Boost::Asio.
    Клиент подключается к серверу с указанным адресом и портом, отправляет путь к файлу,
    который он хочет получить от сервера, и получает содержимое файла в виде потока байтов,
    записывая его в локальный файл с указанным путем.

    Основной класс tcp_client имеет четыре основных метода:
    • start(), который инициирует асинхронное подключение к серверу;
    • write_file_path(), который отправляет путь к файлу на сервер;
    • read_file_content(), который асинхронно читает содержимое файла, возвращаемое сервером;
    • write_file_content(), который записывает содержимое файла в локальный файл.

    В функции main(), клиент создается с использованием переданных в командной строке аргументов,
    инициализируется с помощью io_service, запускается метод start() и выполняется цикл
    обработки событий ввода-вывода с помощью service.run().
*/

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

#include <iostream>
#include <fstream>
#include <boost/asio.hpp>
#include <boost/filesystem.hpp>

using namespace boost::asio;
using namespace boost::asio::ip;

class tcp_client
{
private:

    tcp::socket socket_;
    tcp::endpoint endpoint_;
    std::string file_path_;
//    std::ofstream file_;
    boost::asio::streambuf streambuf_;

public:

    tcp_client(io_service& service, const std::string& server_address,
               unsigned short server_port, const std::string& file_path)
    : socket_(service), file_path_(file_path)
    //, file_(file_path_, std::ios_base::binary)
    {
        tcp::resolver resolver(service);
        tcp::resolver::query query(server_address, std::to_string(server_port));
        endpoint_ = *resolver.resolve(query);
    };

    void start()
    {
        socket_.async_connect(endpoint_, [this](boost::system::error_code ec)
            {
                if (!ec)
                {
                    std::cout << "Connected to server" << std::endl;
                    write_file_path();
                }
                else
                {
                    std::cout << "Connection failed: " << ec.message() << std::endl;
                }
            });
    };

private:

    void write_file_path()
    {
        std::cout << "Sending file path: " << file_path_ << std::endl;
        async_write(socket_, buffer(file_path_ + "\n"), [this](boost::system::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    read_file_content();
                }
                else
                {
                    std::cout << "Write failed: " << ec.message() << std::endl;
                }
            });
    };

    void read_file_content()
    {
        async_read_until(socket_, streambuf_, "\n", [this](boost::system::error_code ec, std::size_t)
            {
                if (!ec)
                {
                    std::cout << "Received file content" << std::endl;
                    write_file_content();
                }
                else
                {
                    std::cout << "Read failed: " << ec.message() << std::endl;
                }
            });
    };

    void write_file_content()
    {
        std::cout << "Writing file content" << std::endl;
        std::ofstream output(file_path_, std::ios::binary | std::ios::trunc);
        if (output.is_open())
        {
            std::istream input(&streambuf_);
            output << input.rdbuf();;
            output.close();
            std::cout << "File saved to " << file_path_ << std::endl;
        }
        else
        {
            std::cerr << "Unable to open file." << std::endl;
        }
        socket_.close();
    };

};

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << boost::filesystem::path (argv[0]).filename() << " <server_address> <server_port> <file_path>" << std::endl;
        return EXIT_FAILURE;
    }
    try
    {
        io_service service;
        tcp_client client(service, argv[1], std::stoi(argv[2]), argv[3]);
        client.start();
        service.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

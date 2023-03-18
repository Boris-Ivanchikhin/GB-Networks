//
// *** Course: Network programming in C++
// *** Lesson 04. Work under load. Optimization of working with multiple clients.
//
//  2. Переработайте TCP-сервер так, чтобы внутри потоков обрабатывающих запросы клиента,
//      использовался Asio, либо Boost.Asio, т.е. работа с клиентскими запросами внутри потока велась асинхронно.
//      В итоге, каждый поток сервера мог обрабатывать несколько клиентов в асинхронном режиме.
//

/*
    Этот код является реализацией простого асинхронного сервера для TCP соединений с использованием библиотеки Boost::Asio.
    Клиент устанавливает соединение с сервером и передает имя файла, сервер в ответ отправляет клиенту его содержимое.

    Краткое описание классов и функций:
    • session - класс, который представляет отдельное соединение. В конструкторе создается сокет,
                который связывается с контекстом ввода-вывода. Метод start инициирует чтение из сокета,
                а когда данные получены, вызывается handle_read.
    • server  - класс, который управляет принятием соединений. В конструкторе создается экземпляр tcp::acceptor,
                который связывается с контекстом ввода-вывода и указанным портом.
                В методе start_accept создается новый объект session, который используется для принятия
                нового соединения через async_accept. Когда соединение установлено, вызывается handle_accept,
                который вызывает метод start объекта session.
    • main    - функция, которая создает экземпляр server и запускает контекст ввода-вывода.

    Таким образом, при запуске программы сервер начинает слушать входящие соединения на указанном порту,
    принимает имена файлов и начинает отправлять обратно их содержимое.
    При возникновении ошибки соединение закрывается.

 */

#ifdef WIN32
#define _WIN32_WINNT 0x0501
#include <stdio.h>
#endif

#define BOOST_FILESYSTEM_VERSION 3

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>
#include <boost/filesystem.hpp>

// Declare a boost::error_info typedef that holds the stacktrace:
typedef boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace> traced;

// Write a helper class for throwing any exception with stacktrace
template <class E>
void throw_with_trace(const E& e)
{
    throw boost::enable_error_info(e)
            << traced(boost::stacktrace::stacktrace());
}


using boost::asio::ip::tcp;

// Класс session представляет сеанс связи с клиентом.
class session
{
private:

    tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];

public:

    // Конструктор принимает объект boost::asio::io_context,
    // который используется для управления асинхронными операциями.
    session(boost::asio::io_context& io_context)
    : socket_(io_context)
    {}; // session()

    // Метод socket() возвращает ссылку на объект tcp::socket.
    tcp::socket& socket()
    {
        return socket_;
    }; // socket()

    // Метод start() начинает асинхронное чтение данных из сокета клиента, используя метод async_read_some().
    void start()
    {
        socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                boost::bind(&session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
    }; // start()

private:

    // Метод handle_read() вызывается при завершении асинхронного чтения данных из сокета.
    // Если нет ошибок при чтении, то вызывается метод async_write(), который отправляет файл клиенту.
    // Если произошла ошибка, то объект session удаляется.
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (!error)
        {
            std::string filename(data_, bytes_transferred);
            if (boost::filesystem::exists(filename))
            {
                std::ifstream file(filename, std::ios::binary);
                if (file.is_open())
                {
                    file.seekg(0, std::ios::end);
                    const auto file_size = file.tellg();
                    file.seekg(0, std::ios::beg);

                    std::vector<char> file_data(file_size);
                    file.read(file_data.data(), file_size);

                    /* Вариант отдачи БЕЗ ОЧЕРЕДИ

                    boost::asio::async_write(socket_,
                                             boost::asio::buffer(file_data),
                                             boost::bind(&session::handle_write, this,
                                                         boost::asio::placeholders::error));
                    */

                    // После создания объекта std::vector<char> с копией file_data, мы создаем shared_ptr data, чтобы передать его в задачу.
                    // Затем мы используем boost::asio::post для постановки задачи на выполнение в контексте io_context.
                    // Задача вызывает boost::asio::async_write, передавая объект socket_ и data, и обрабатывает результат в handle_write.

                    // Внимание!
                    // data передается как shared_ptr, а не как простой указатель!
                    // Это необходимо для того, чтобы data не был освобожден до тех пор, пока задача не будет завершена.
                    auto data = std::make_shared<std::vector<char>>(file_data);
                    boost::asio::post(socket_.get_executor(), [this, data]()
                                        {
                                        boost::asio::async_write(socket_, boost::asio::buffer(*data),
                                                 [this, data](const boost::system::error_code& error, std::size_t bytes_transferred)
                                                 {
                                                     this->handle_write(error);
                                                 });
                    });

                    file.close();
                    return;
                }
                else
                {
                    std::cout << "Unable to open file " << filename << std::endl;
                } // if (file.is_open())
            }
            else
            {
                std::cout << "File " << filename << " does not exist" << std::endl;
            } // if (boost::filesystem::exists(filename))

        }
        else
        {
            std::cout << "Error during file sending: " << error.message() << std::endl;
            delete this;
        } // if (!error)

    }; // handle_read()

    // Метод handle_write() вызывается при завершении асинхронной отправки данных клиенту.
    // Если нет ошибок при отправке данных, то вызывается метод async_read_some() для чтения новых данных от клиента.
    // Если произошла ошибка, то объект session удаляется.
    void handle_write(const boost::system::error_code& error)
    {
        if (!error)
        {
            socket_.async_read_some(boost::asio::buffer(data_, max_length),
                                    boost::bind(&session::handle_read, this,
                                                boost::asio::placeholders::error,
                                                boost::asio::placeholders::bytes_transferred));
        }
        else
        {
            delete this;
        }

    }; // handle_write()

};

// Класс server инкапсулирует объект tcp::acceptor, который слушает новые соединения
// и создает новые объекты session для каждого нового соединения.
class server
{
private:

    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;

public:

    // Конструктор принимает объект boost::asio::io_context и порт, на котором сервер будет слушать новые соединения.
    // В конструкторе создается объект tcp::endpoint для указанного порта и привязывается к объекту tcp::acceptor.
    server(boost::asio::io_context& io_context, short port)
    : io_context_(io_context),
      acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
      {
        start_accept();
      };

private:

    // Метод start_accept() создает новый объект session и вызывает метод async_accept() объекта tcp::acceptor.
    // При завершении вызывается метод handle_accept().
    void start_accept()
    {
        session* new_session = new session(io_context_);
        acceptor_.async_accept(new_session->socket(),
                               boost::bind(&server::handle_accept, this, new_session,
                                           boost::asio::placeholders::error));
    };

    // Метод handle_accept() вызывается при завершении асинхронного принятия нового соединения.
    // Если нет ошибок при принятии, то вызывается метод start() нового объекта session.
    // Если произошла ошибка, то объект session удаляется.
    // Затем вызывается метод start_accept() для принятия следующего соединения.
    void handle_accept(session* new_session, const boost::system::error_code& error)
    {
        if (!error)
        {
            new_session->start();
        }
        else
        {
            delete new_session;
        }

        start_accept();
    };

};

/*
    Функция main() создает экземпляр сервера и запускает его в бесконечном цикле:
    • Создается объект boost::asio::io_context, который является главным объектом библиотеки Boost::Asio
        и используется для управления асинхронными операциями в программе.
    • Затем создается объект класса server. Сервер принимает экземпляр boost::asio::io_context и номер порта
        (на котором сервер должен принимать подключения).
    • Наконец, вызывается метод run() объекта boost::asio::io_context, который запускает
        бесконечный цикл обработки асинхронных операций.
    • Метод start_accept() класса server создает новый объект session и запускает асинхронную операцию accept на объекте acceptor_.
        Эта операция ждет подключения клиента и при его появлении вызывает метод handle_accept().
    • Метод handle_accept() класса server вызывается, когда клиент подключается к серверу.
        Если при подключении не произошло ошибок, то метод start() класса session вызывается для начала обработки данных от клиента.
        Если произошла ошибка, то объект session удаляется.
    • Методы handle_read() и handle_write() класса session обрабатывают завершение операций чтения и записи в сокет.
        Если операция завершилась успешно, то продолжается чтение/запись данных. Если произошла ошибка, то объект session удаляется.
    • В итоге, сервер остается в бесконечном цикле обработки асинхронных операций до тех пор, пока не будет остановлен вручную.
        Когда клиент подключается к серверу, сервер создает новый объект session и начинает
        асинхронно обрабатывать данные от клиента до тех пор, пока клиент не закроет соединение.
 */
int main(int argc, char* argv[])
{
      try
      {
          if (argc != 2)
          {
              std::cout << "Usage: " << boost::filesystem::path (argv[0]).filename() << " <port>\n";
              return EXIT_FAILURE;
          }

          boost::asio::io_context io_context;

          using namespace std; // For atoi.
          server s(io_context, atoi(argv[1]));

          io_context.run();
      } // try

      catch (std::exception& e)
      {
          std::cerr << "Exception: " << e.what() << "\n";

          const boost::stacktrace::stacktrace* st = boost::get_error_info<traced>(e);

          if (st)
          {
              std::cout << *st << '\n';
          }

          return EXIT_FAILURE;
      } // catch

      return EXIT_SUCCESS;
}

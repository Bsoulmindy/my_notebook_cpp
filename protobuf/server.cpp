#include <iostream>
#include "request.pb.h"
#include <boost/asio.hpp>
#include <string>
#include <boost/array.hpp>

using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8083));
    tcp::socket socket(io_context);
    std::cout << "Starting listening for incoming connections..." << std::endl;
    acceptor.accept(socket);

    std::cout << "Client connected : " << socket.remote_endpoint().address() << std::endl;
    int quantity = 0;
    while (true)
    {
        Request request;
        boost::array<char, 128> buf;
        boost::system::error_code error;
        size_t len = socket.read_some(boost::asio::buffer(buf), error);

        if (error == boost::asio::error::eof)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }
        else if (error)
            throw boost::system::system_error(error);
        else if (len == 0)
        {
            std::cout << "Error occured when listening for packets" << std::endl;
            return -1;
        }

        if (!request.ParseFromArray(buf.elems, len))
        {
            std::cout << "Error : Failed to get a valid Request from the client" << std::endl;
            return -1;
        }

        if (request.operation() == "update")
        {
            quantity += request.quantity();
            std::cout << "Changing current quantity to "
                      << quantity
                      << "("
                      << (request.quantity() >= 0 ? "+" : "")
                      << request.quantity()
                      << ")"
                      << std::endl;
        }
        else
        {
            std::cout << "Invalid operation : " << request.operation() << std::endl;
        }
    }

    return 0;
}
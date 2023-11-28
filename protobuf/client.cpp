#include <iostream>
#include "request.pb.h"
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main()
{
    boost::asio::io_context io_context;
    tcp::resolver resolver(io_context);
    tcp::resolver::query query("localhost", "8083");
    tcp::resolver::results_type endpoints = resolver.resolve("localhost", "8083");
    tcp::socket socket(io_context);
    boost::asio::connect(socket, endpoints);

    while (true)
    {
        std::cout << "Please enter a number: ";
        int number;
        std::cin >> number;

        Request request;
        request.set_operation("update");
        request.set_quantity(number);
        std::cout << "Sending request to the server" << std::endl;

        boost::system::error_code error;
        std::string buf = request.SerializeAsString();
        size_t len = boost::asio::write(socket, boost::asio::buffer(buf), error);

        if (error)
        {
            std::cout << "Failed to send request to the server : " << error << std::endl;
        }
    }
}
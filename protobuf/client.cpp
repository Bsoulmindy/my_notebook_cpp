#include <iostream>
#include "request.pb.h"
#include <boost/asio.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

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
        spdlog::info("Sending request to the server");

        boost::system::error_code error;
        std::string buf = request.SerializeAsString();
        size_t len = boost::asio::write(socket, boost::asio::buffer(buf), error);

        if (error)
        {
            spdlog::error("Failed to send request to the server : {}", error.message());
        }
    }
}
#include <iostream>
#include "request.pb.h"
#include <boost/asio.hpp>
#include <string>
#include <boost/array.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using boost::asio::ip::tcp;

int main(int argc, char *argv[])
{
    boost::asio::io_context io_context;
    tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8083));
    tcp::socket socket(io_context);
    spdlog::info("Starting listening for incoming connections...");
    acceptor.accept(socket);

    spdlog::info("Client connected : {}", socket.remote_endpoint().address().to_string());
    int quantity = 0;
    while (true)
    {
        Request request;
        boost::array<char, 128> buf;
        boost::system::error_code error;
        size_t len = socket.read_some(boost::asio::buffer(buf), error);

        if (error == boost::asio::error::eof)
        {
            spdlog::error("Client disconnected");
            break;
        }
        else if (error)
            throw boost::system::system_error(error);
        else if (len == 0)
        {
            spdlog::error("Error occured when listening for packets");
            return -1;
        }

        if (!request.ParseFromArray(buf.elems, len))
        {
            spdlog::error("Error : Failed to get a valid Request from the client");
            return -1;
        }

        if (request.operation() == "update")
        {
            quantity += request.quantity();
            spdlog::info("Changing current quantity to {} ({}{})",
                         quantity,
                         request.quantity() >= 0 ? "+" : "",
                         request.quantity());
        }
        else
        {
            spdlog::error("Invalid operation : {}", request.operation());
        }
    }

    return 0;
}
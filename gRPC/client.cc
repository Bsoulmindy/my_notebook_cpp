#include "queue.grpc.pb.h"
#include "queue.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>

class OfficeRemote
{
public:
    OfficeRemote(std::shared_ptr<grpc::ChannelInterface> channel)
        : stub_(TicketSystem::Office::NewStub(channel)) {}

    void Reserve(const std::string name)
    {
        TicketSystem::Request request;
        request.set_name(name);

        grpc::ClientContext context;
        TicketSystem::Response response;

        auto status = stub_->Reserve(&context, request, &response);
        if (!status.ok())
        {
            std::cout << "Error occured when reserving a ticket : " << status.error_code() << std::endl;
            return;
        }

        std::cout << "Successfully reserved a ticket for "
                  << response.name()
                  << ", seat number is "
                  << response.id()
                  << std::endl;
    }

private:
    std::unique_ptr<TicketSystem::Office::Stub> stub_;
};

int main()
{
    auto channel = grpc::CreateChannel("localhost:8084", grpc::InsecureChannelCredentials());
    OfficeRemote remote(channel);

    while (true)
    {
        std::cout << "Type your name : ";
        std::string inputName;
        getline(std::cin, inputName);
        int max_iter;
        std::cout << "Type number of iterations : ";
        std::cin >> max_iter;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        for (int i = 1; i <= max_iter; i++)
        {
            remote.Reserve(inputName + std::to_string(i));
        }
    }
}
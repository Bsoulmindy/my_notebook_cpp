#include "queue.grpc.pb.h"
#include "queue.pb.h"
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <chrono>
#include <thread>

// All implementations should be thread-safe
class OfficeImpl final : public TicketSystem::Office::Service
{
    std::atomic<int> _a_id{1};

    ::grpc::Status Reserve(::grpc::ServerContext *context, const ::TicketSystem::Request *request, ::TicketSystem::Response *response) override
    {
        response->set_name(request->name());
        response->set_id(_a_id++);
        // Test for multi-thread
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        return grpc::Status::OK;
    }
};

int RunServer()
{
    std::string server_address("0.0.0.0:8084");
    OfficeImpl office;

    grpc::ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&office);
    auto server_ptr = builder.BuildAndStart();

    if (server_ptr == nullptr)
    {
        std::cout << "Error while creating the server!" << std::endl;
        return -1;
    }

    std::cout << "Listening to income requests..." << std::endl;
    server_ptr->Wait();
    return 0;
}

int main()
{
    return RunServer();
}
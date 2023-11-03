#include <iostream>

#if defined(__cpp_impl_coroutine)
#include <coroutine>
#include <chrono>
#include <thread>
#include <future>

auto send_request()
{
    struct awaitable
    {
        std::future<std::string> p;

        // Method to check if the awaitable is ready
        // It's called initially, and if it returns false, then it suspend the await first
        // before jumping to the final method await_resume()
        //
        // In general should be false if you want to execute await_suspend() and do some logic
        bool await_ready() { return false; }

        // Main method to execute some logic
        // It can access to the handle of the coroutine which invoked "co_await" to this object
        // If the return value is void or true, then we give the control to the caller of the coroutine
        // in other words, we exit the coroutine which invoked "co_await"
        // but if the return value is false, then the coroutine is simply resumed
        //
        // Remember, if the coroutine is resumed by any means, it will call await_resume() to get the final result
        bool await_suspend(std::coroutine_handle<> h)
        {
            p.wait();
            // return true; // This will give the control back to the main() method, which will simply destroy everything

            // This will give the control back to the coroutine, which will execute await_resume() to get the final result
            // then continuing it's logic
            return false;
        }

        // Method to return the final result to the caller
        std::string await_resume()
        {
            if (!p.valid())
                throw std::runtime_error("Promise is invalid!");

            std::string response = p.get();
            return response;
        }
    };

    std::future<std::string> p = std::async([]()
                                            {
            std::cout << "Sending Request, waiting for response" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(3));
            return std::string("Hello world!"); });
    return awaitable{std::move(p)};
}

struct task
{
    // Declaring coroutine handler with the name "handle_type"
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    handle_type h_;

    struct promise_type
    {
        // Method executed initially to create an instance of Promise object
        // We must associate it to the handler to track it
        task get_return_object() { return task(handle_type::from_promise(*this)); }

        // Controls if the coroutine will start executing eagerly or lazely (manually by controlling this object)
        std::suspend_never initial_suspend() { return {}; }
        // Desactivating self-destruction
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}

        void return_void() {}

        // Destructor of the promise object
        ~promise_type()
        {
            std::cout << "Destroyed!" << std::endl;
        }
    };

    task(handle_type h) : h_(h){};
    // Since we disabled self-destruction, we must destoy the object manually
    // Not doing so will result in a memory leak
    ~task()
    {
        h_.destroy();
    }
};

task coroutine_send_request()
{
    auto responseFuture = send_request();

    std::cout << "Doing other stuff..." << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "Finished my stuff. Time to get the response" << std::endl;
    std::string response = co_await responseFuture;

    std::cout << "Response : " << response << std::endl;
}

int main()
{
    coroutine_send_request();
}

#else
int main()
{
    std::cout << "Coroutines are not supported in this compiler!" << std::endl;
}
#endif
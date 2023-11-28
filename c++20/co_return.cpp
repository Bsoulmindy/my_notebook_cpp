#include <iostream>

#if defined(__cpp_impl_coroutine)
#include <coroutine>

template <typename T>
struct task
{
    // Declaring coroutine handler with the name "handle_type"
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    handle_type h_;

    // The mandatory Promise Object
    // This object will be created in the coroutine initially,
    // when the coroutune suspends (or finishs), it will call initial_suspend() (or final_suspend())
    // which will destroy the promise object if std::suspend_never, or not if std::suspend_always
    // So, in order to track it, we must associate it with the handler defined above
    struct promise_type
    {
        // A value
        T value_;

        // Method executed initially to create an instance of Promise object
        // We must associate it to the handler to track it
        task get_return_object() { return task(handle_type::from_promise(*this)); }

        // Method executed when co_return occurs
        void return_value(T ans)
        {
            value_ = ans;
        }

        // Controls if the coroutine will start executing eagerly or lazely (manually by controlling this object)
        std::suspend_never initial_suspend() { return {}; }

        // The promie object won't destroy itself when coroutine returns object
        // It's important in order to get the co_return value
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}

        // Destructor of the promise object
        ~promise_type()
        {
            value_ = 0;
            std::cout << "Destroyed!" << std::endl;
        }
    };

    task(handle_type h) : h_(h){};
    // Once the object goes out of scope, and since we disabled auto destroying
    // it's our responsibility now to destroy manually the promise
    // otherwise, a memory leak will be issued
    ~task()
    {
        h_.destroy();
    }
};

task<int> async_value(int value)
{
    co_return value * 2;
}

int main()
{
    auto result = async_value(42);

    std::cout << result.h_.promise().value_ << std::endl;
}

#else
int main()
{
    std::cout << "Coroutines are not supported in this compiler!" << std::endl;
}
#endif
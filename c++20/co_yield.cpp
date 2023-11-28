#include <iostream>

#if defined(__cpp_impl_coroutine)
#include <coroutine>

template <class T>
struct generator
{
    // Declaring coroutine handler with the name "handle_type"
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;

    handle_type h_;

    struct promise_type
    {
        T value_;

        // Method executed initially to create an instance of Promise object
        // We must associate it to the handler to track it
        generator<T> get_return_object() { return generator<T>(handle_type::from_promise(*this)); }

        // Controls if the coroutine will start executing eagerly or lazely (manually by controlling this object)
        std::suspend_always initial_suspend() { return {}; }
        // Desactivating self-destruction
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() {}

        // Method that will be executed when "co_yield" occurs
        // We made std::suspend_always the return object of this method, since we want to suspend
        // coroutine execution to print the value
        std::suspend_always yield_value(T val)
        {
            value_ = val;
            return {};
        }

        void return_void() {}

        // Destructor of the promise object
        ~promise_type()
        {
            std::cout << "Destroyed!" << std::endl;
        }
    };

    generator(handle_type h) : h_(h){};
    // Since we disabled self-destruction, we must destoy the object manually
    // Not doing so will result in a memory leak
    ~generator()
    {
        h_.destroy();
    }

    // Custom method to resume the coroutine to generate the next value
    T getNextValue()
    {
        if (!h_.done())
            h_.resume();
        return h_.promise().value_;
    }
};

generator<int> generate_factorial_until_max(int max_value)
{
    int i = 1;
    int prod = 1;
    while (i <= max_value)
    {
        co_yield prod *= i++;
    }
}

int main()
{
    auto gen = generate_factorial_until_max(10);

    for (int i = 1; i <= 15; i++)
        std::cout << "i = " << i << ", value = " << gen.getNextValue() << std::endl;
}

#else
int main()
{
    std::cout << "Coroutines are not supported in this compiler!" << std::endl;
}
#endif
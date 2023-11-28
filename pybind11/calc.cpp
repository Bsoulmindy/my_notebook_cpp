#include <pybind11/pybind11.h>
#include <iostream>

// A worker who lived his whole life close to metal
class NativeWorker
{
public:
    NativeWorker(const std::string &name) : m_name{name} {}

    int heavy_work(int n)
    {
        std::cout << "Hello! my name is " << m_name << std::endl;
        int s = 0;
        for (int i = 0; i <= n; i++)
        {
            if (i % 2 == 0)
                s++;
            else
                s--;
        }
        return s;
    }

private:
    std::string m_name;
};

PYBIND11_MODULE(customcpp, m)
{
    pybind11::class_<NativeWorker>(m, "NativeWorker")
        .def(pybind11::init<const std::string &>())
        .def("heavy_work", &NativeWorker::heavy_work);
}
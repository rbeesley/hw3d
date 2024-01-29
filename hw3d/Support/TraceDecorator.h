#include <debugapi.h>
#include <functional>
#include <string>

template <class Tfunc>
class TraceDecorator
{
public:
    TraceDecorator(Tfunc func) : m_func(func) {}

    template <class... Targs>
    auto operator()(Targs&&... args) -> decltype(auto)
    {
        std::string msg = "Calling " + std::string(__FUNCTION__) + " with ";
        for (const auto& arg : { args... })
        {
            msg += std::to_string(arg) + " ";
        }
        msg += "\n";
        OutputDebugString(msg.c_str());

        auto result = m_func(std::forward<Targs>(args)...);

        msg = "Called " + std::string(__FUNCTION__) + "\n";
        OutputDebugString(msg.c_str());

        return result;
    }

private:
    Tfunc m_func;
};

// A test function
int add(int a, int b)
{
    return a + b;
}

int main()
{
    // Create a decorated function
    TraceDecorator<decltype(add)> trace_add(add);

    // Call the decorated function
    int c = trace_add(3, 4);

    return 0;
}

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <initializer_list>

void end()
{
    std::cout << "hello world" << std::endl;
    // ↑ 每次都会flush
    std::cout << "hello world" << '\n' << std::flush;
}

template <typename... T>
void print(const T&... t)
{
    // ,解包 逗号表达式中可以有多个语句
    std::initializer_list<int> l{ (std::cout << t, std::cout << '\n', 0)... };
}

template <typename T>
void to_string_impl(std::stringstream& ss, const T& t)
{
    ss << t;
}

template <typename T, typename... Args>
void to_string_impl(std::stringstream& ss, const T& t, Args... args)
{
    ss << t;
    to_string_impl(ss, args...);
}

template <typename... Args>
std::string to_string(const Args&... args)
{
    std::stringstream ss;
    to_string_impl(ss, args...);

    {
        // lambda实现
        const auto to_string_impl_x = [&ss](const auto& t) {
            ss << t;
            return ss.str();
        };

        std::vector v{ to_string_impl_x(args)... };
        (void)std::initializer_list<int>{ (to_string_impl_x(args), 0)... };
    }

    return ss.str();
}

int main()
{

    std::cout << to_string("hello", 1.9, 26, "xx", true) << "\n";

    print("hello", "world", 1, 2, 1.0, 2.50);
    return 0;
}
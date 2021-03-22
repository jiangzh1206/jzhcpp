#include <iostream>
#include <numeric>
#include <type_traits>

template <typename T>
constexpr bool is_both()
{
    if constexpr (std::is_integral_v<T> && !std::is_same_v<bool, T>) {
        if constexpr (std::numeric_limits<T>::max() < 1000) {
            return true;
        }
    }
    return false;
}

template <typename T>
auto print_type_info(const T& t)
{
    if constexpr (is_both<T>()) {
        std::cout << "is integral\n";
        return t + 1;
    }
    else if constexpr (std::is_floating_point<T>::value) {
        std::cout << "is not integral\n";
        return t + 0.1;
    }
    else {
        std::cout << "else...\n";
        return t;
    }
}

int main()
{
    print_type_info(5);
    print_type_info(5000);
    print_type_info(5.f);
    print_type_info(true);
    print_type_info("a");

    return 0;
}
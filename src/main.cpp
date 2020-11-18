#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <boost/integer/common_factor_rt.hpp>

#include <iostream>

using json = nlohmann::json;

struct Foo {
    int i = 0;
    int j = 0;

    auto operator<=>(const Foo& rhs) const = default;
};

int main(int argc, char* argv[]) {
    std::cout << "Hello world\n";
    fmt::print("Hello fmt!\n");
    spdlog::info("Welcome to spdlog!");

    auto j = R"(
        {
            "happy": true,
            "pi": 3.141
        }
    )"_json;
    std::cout << "JSON:\n" << j.dump(4) << "\n";
    std::cout << "lcm(5, 6) = " << boost::integer::lcm(5, 6) << "\n";

    Foo f1{1, 2};
    Foo f2{2, 1};
    Foo f3{2, 2};

    std::cout << "f1 < f2: " << (f1 < f2) << '\n';
    std::cout << "f1 > f2: " << (f1 > f2) << '\n';
    std::cout << "f2 < f3: " << (f2 < f3) << '\n';
    std::cout << "f2 > f3: " << (f2 > f3) << '\n';
    std::cout << "f1 == f2: " << (f1 == f2) << '\n';
    std::cout << "f1 != f2: " << (f1 != f2) << '\n';
    std::cout << "f2 == f3: " << (f2 == f3) << '\n';
    std::cout << "f2 != f3: " << (f2 != f3) << '\n';

    return 0;
}

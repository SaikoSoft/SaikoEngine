#include <fmt/core.h>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <boost/integer/common_factor_rt.hpp>

#include <iostream>

using json = nlohmann::json;

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

    return 0;
}

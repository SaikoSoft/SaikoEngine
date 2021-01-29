#include <catch2/catch.hpp>

TEST_CASE("First test") {
    int i = 5;
    ++i;
    REQUIRE(i == 6);
}

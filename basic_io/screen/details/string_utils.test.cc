#include <catch2/catch_test_macros.hpp>

#include "string_utils.hpp"

TEST_CASE("my_is_space checks", "string_utils")
{
    for (uint32_t idx = 0; idx < 256; ++idx)
    {
        const char character{static_cast<char>(idx)};
        const bool expected{character == ' ' || character == '\t' ? true : false};
        REQUIRE(screen::details::my_is_blank(character) == expected);
    }
}

TEST_CASE("determine_page_breaks case 1", "string_utils")
{
    const char *test_str{"012 456 8"};
    const auto result = screen::details::determine_page_breaks(test_str, 3);

    REQUIRE(result.size() == 2);
}

TEST_CASE("determine_page_breaks case 2", "string_utils")
{
    const char *test_str{"01"};
    const auto result = screen::details::determine_page_breaks(test_str, 3);

    REQUIRE(result.size() == 0);
}

TEST_CASE("determine_page_breaks case 3", "string_utils")
{
    const char *test_str3{"Collect chips to get past the chip socket. Use keys to open doors."};
    const auto result3 = screen::details::determine_page_breaks(test_str3, 20);
    const std::array<uint8_t, 3> expectation{21, 14, 20}; /* TODO the last element fails */

    REQUIRE(result3.size() == std::size(expectation));

    for (size_t ii = 0; ii < std::size(expectation); ++ii)
    {
        REQUIRE(expectation[ii] == static_cast<int>(result3[ii]));
    }
}
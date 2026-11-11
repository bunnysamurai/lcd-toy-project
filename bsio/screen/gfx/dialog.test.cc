#include <catch2/catch_test_macros.hpp>

#include "dialog.hpp"

TEST_CASE("[dialog] ALWAYS", "dialog")
{
    SUCCEED();
}

TEST_CASE("[dialog] NEVER","dialog" )
{
    FAIL();
}
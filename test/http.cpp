#include "http.hpp"
#include <stdexcept>
#include "gtest/gtest.h"

using namespace http;

TEST(http, split1)
{
    const char* ins = "GET / HTTP/1.1\r\n";
    auto res = split(ins, strlen(ins));
    ASSERT_EQ(res.size(), 3);
    ASSERT_EQ(res[0], "GET");
    ASSERT_EQ(res[1], "/");
    ASSERT_EQ(res[2], "HTTP/1.1");
}
TEST(http, split2)
{
    const char* ins = "  GET / HTTP/1.1\r\n  ";
    auto res = split(ins, strlen(ins));
    ASSERT_EQ(res.size(), 3);
    ASSERT_EQ(res[0], "GET");
    ASSERT_EQ(res[1], "/");
    ASSERT_EQ(res[2], "HTTP/1.1");
}
TEST(http, split3)
{
    const char* ins = "  ";
    auto res = split(ins, strlen(ins));
    ASSERT_EQ(res.size(), 0);
}

TEST(http, split4)
{
    const char* ins = "";
    auto res = split(ins, strlen(ins));
    ASSERT_EQ(res.size(), 0);
}

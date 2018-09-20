#include "path.hpp"
#include <stdexcept>
#include "gtest/gtest.h"

void test_path(const Url& url, const std::vector<std::string>& names)
{
    int idx = 0;
    ASSERT_EQ(url.path.size(), names.size());
    for (const auto& n : url.path)
    {
        ASSERT_EQ(n, names[idx]);
        idx++;
    }
}
TEST(path, parse_valid1)
{
    auto res = parseUrl("/");
    test_path(res, {});
}

TEST(path, parse_valid2)
{
    auto res = parseUrl("/abc");
    test_path(res, {"abc"});
}

TEST(path, parse_valid3)
{
    auto res = parseUrl("/abc/def/ggg");
    test_path(res, {"abc", "def", "ggg"});
}

TEST(path, parse_valid4)
{
    auto res = parseUrl("/abc/def");
    test_path(res, {"abc", "def"});
}

TEST(path, parse_invalid)
{
    ASSERT_THROW(parseUrl(""), std::invalid_argument);
    ASSERT_THROW(parseUrl("/ab/./d"), std::invalid_argument);
}

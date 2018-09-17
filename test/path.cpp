#include "path.hpp"
#include <stdexcept>
#include "gtest/gtest.h"

void test_path(const Path& path, const std::vector<std::string>& names)
{
    int idx = 0;
    ASSERT_EQ(path.size(), names.size());
    for (const auto& n : path)
    {
        ASSERT_EQ(n.name(), names[idx]);
        idx++;
    }
}
TEST(path, parse_valid1)
{
    auto res = Path("/");
    test_path(res, {});
}

TEST(path, parse_valid2)
{
    auto res = Path("/abc");
    test_path(res, {"abc"});
}

TEST(path, parse_valid3)
{
    auto res = Path("/abc/def/ggg");
    test_path(res, {"abc", "def", "ggg"});
}

TEST(path, parse_valid4)
{
    auto res = Path("abc/def");
    test_path(res, {"abc", "def"});
}

TEST(path, parse_invalid)
{
    ASSERT_THROW(Path(""), std::invalid_argument);
    ASSERT_THROW(Path("/ab/./d"), std::invalid_argument);
}

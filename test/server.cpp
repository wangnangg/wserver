#include "server.hpp"
#include <stdexcept>
#include "gtest/gtest.h"

TEST(server, parse_ip_valid)
{
    std::string ip_str = "1.2.3.4";
    auto ip = parseIpAddr(ip_str);
    ASSERT_EQ(ip, 0x01020304);
}
TEST(server, parse_port_valid)
{
    std::string port_str = "324";
    auto port = parsePort(port_str);
    ASSERT_EQ(port, 324);
}

TEST(server, parse_port_invalid)
{
    std::string port_str = "324444";
    ASSERT_THROW(parsePort(port_str), std::invalid_argument);
}

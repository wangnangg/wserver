#include "option.hpp"
#include <stdexcept>
#include <vector>
#include "gtest/gtest.h"

TEST(option, valid_case1)
{
    optind = 0;
    CmdOption opt;
    std::vector<std::string> arg_vec = {
        {"exe_name"}, {"--ip"}, {"192.168.1.1"}, {"--port"}, {"456"}};
    int argc = arg_vec.size();
    char* argv[argc];
    for (int i = 0; i < argc; i++)
    {
        argv[i] = (char*)arg_vec[i].c_str();
    }
    parseArg(argc, argv, opt);
    ASSERT_EQ(opt.ipv4_addr, arg_vec[2].c_str());
    ASSERT_EQ(opt.port, arg_vec[4].c_str());
}
TEST(option, valid_case2)
{
    optind = 0;
    CmdOption opt;
    std::vector<std::string> arg_vec = {
        {"exe_name"}, {"-i"}, {"192.168.1.1"}, {"-p"}, {"456"}};
    int argc = arg_vec.size();
    char* argv[argc];
    for (int i = 0; i < argc; i++)
    {
        argv[i] = (char*)arg_vec[i].c_str();
    }
    parseArg(argc, argv, opt);
    ASSERT_EQ(opt.ipv4_addr, arg_vec[2].c_str());
    ASSERT_EQ(opt.port, arg_vec[4].c_str());
}
TEST(option, valid_case3)
{
    optind = 0;
    CmdOption opt;
    std::vector<std::string> arg_vec = {
        {"exe_name"}, {"-i"}, {"192.168.1.1"}};
    int argc = arg_vec.size();
    char* argv[argc];
    for (int i = 0; i < argc; i++)
    {
        argv[i] = (char*)arg_vec[i].c_str();
    }
    parseArg(argc, argv, opt);
    ASSERT_EQ(opt.ipv4_addr, arg_vec[2].c_str());
}

TEST(option, invalid_case1)
{
    optind = 0;
    CmdOption opt;
    std::vector<std::string> arg_vec = {{"exe_name"}, {"-p"}};
    int argc = arg_vec.size();
    char* argv[argc];
    for (int i = 0; i < argc; i++)
    {
        argv[i] = (char*)arg_vec[i].c_str();
    }
    ASSERT_THROW(parseArg(argc, argv, opt), std::invalid_argument);
}

TEST(option, invalid_case2)
{
    optind = 0;
    CmdOption opt;
    std::vector<std::string> arg_vec = {{"exe_name"}, {"xyz"}};
    int argc = arg_vec.size();
    char* argv[argc];
    for (int i = 0; i < argc; i++)
    {
        argv[i] = (char*)arg_vec[i].c_str();
    }
    ASSERT_THROW(parseArg(argc, argv, opt), std::invalid_argument);
}

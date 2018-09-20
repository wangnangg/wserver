#pragma once
#include <string>

struct CmdOption
{
    std::string ipv4_addr;
    std::string port;
    std::string web_dir;
};

void parseArg(int argc, char* argv[], CmdOption& option);

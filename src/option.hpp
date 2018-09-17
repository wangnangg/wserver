#pragma once
#include <string>

struct CmdOption
{
    std::string ipv4_addr;
    std::string port;
};


void parseArg(int argc, char* argv[], CmdOption& option);







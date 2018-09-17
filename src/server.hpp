#pragma once
#include <sys/types.h>
#include <unistd.h>
#include <cstdint>
#include <string>

struct Ipv4Address
{
    uint32_t ip;
    uint16_t port;
};

int listenOn(Ipv4Address addr, int wait_queue_len);
uint32_t parseIpAddr(const std::string& ip_str);
uint16_t parsePort(const std::string& port_str);

struct ClientInfo
{
    Ipv4Address addr;
    int fd;
};

ClientInfo acceptClient(int sock);

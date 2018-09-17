#include "server.hpp"
#include <netinet/ip.h>
#include <sys/socket.h>
#include <stdexcept>
#include <system_error>

int listenOn(Ipv4Address addr, int wait_queue_len)
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0)
    {
        throw std::system_error(errno, std::system_category());
    }
    int opt = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        throw std::system_error(errno, std::system_category());
    }
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(addr.ip);
    address.sin_port = htons(addr.port);
    if (bind(sockfd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        throw std::system_error(errno, std::system_category());
    }
    if (listen(sockfd, wait_queue_len) < 0)
    {
        throw std::system_error(errno, std::system_category());
    }
    return sockfd;
}

uint32_t parseIpAddr(const std::string& ip_str)
{
    const char* start = ip_str.c_str();
    char* end;
    uint32_t ip = 0;
    int offset = 32;
    for (long i = std::strtol(start, &end, 10); start != end;
         i = std::strtol(start, &end, 10))
    {
        if (i < 0 || i > 255)
        {
            throw std::invalid_argument(
                "invalid ip address: not in [0, 255].");
        }
        offset -= 8;
        if (offset < 0)
        {
            throw std::invalid_argument(
                "invalid ip address: more than four integers.");
        }
        ip = ip | (i << offset);

        while (end != &*ip_str.end() && !isdigit(*end))
        {
            end += 1;
        }
        start = end;
    }
    if (offset != 0)
    {
        throw std::invalid_argument(
            "invalid ip address: less than four integers.");
    }
    if (end != &*ip_str.end())
    {
        throw std::invalid_argument(
            "invalid ip address: extra unrecognized characters.");
    }
    return ip;
}

uint16_t parsePort(const std::string& port_str)
{
    char* end;
    int port = std::strtol(port_str.c_str(), &end, 10);
    if (port_str.c_str() == end)
    {
        throw std::invalid_argument("invalid port number.");
    }
    if (port < 0 || port > 65535)
    {
        throw std::invalid_argument("port number not in [0, 65535].");
    }
    return (uint16_t)port;
}

ClientInfo acceptClient(int sock)
{
    Ipv4Address addr;
    int fd;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    if ((fd = accept(sock, (struct sockaddr*)&address,
                     (socklen_t*)&addrlen)) < 0)
    {
        throw std::system_error(errno, std::system_category());
    }
    addr.ip = ntohl(address.sin_addr.s_addr);
    addr.port = ntohs(address.sin_port);
    return ClientInfo{addr, fd};
}

#include <error.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <stdexcept>
#include "http.hpp"
#include "http_handler.hpp"
#include "option.hpp"
#include "path.hpp"
#include "reader.hpp"
#include "server.hpp"
#include "writer.hpp"

int headerSize(char* buff, int size)
{
    if (size < 4)
    {
        return 0;
    }
    for (int i = 0; i < size - 3; i++)
    {
        if (std::strncmp(buff + i, "\r\n\r\n", 4) == 0)
        {
            return i + 4;
        }
    }
    return 0;
}

// read until \r\n\r\n is encountered, return the size of the content.
int readHeader(BufferedReader& reader, char* buff, int buff_size)
{
    int total = 0;
    int header_size;
    do
    {
        int read_size = reader.get(buff + total, buff_size - total);
        if (read_size == 0)
        {
            throw http::client_error(
                "socket closed before request is completed");
        }
        total += read_size;
        if (total >= buff_size)
        {
            throw http::client_error("request header too long.");
        }
        header_size = headerSize(buff, total);
    } while (header_size == 0);
    reader.put(buff + header_size, total - header_size);
    return header_size;
}

void setTimeout(int sock, int time_sec)
{
    struct timeval timeout;
    timeout.tv_sec = time_sec;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
}

static const int max_request = 1;
static const int sock_timeout_sec = 10;
static const int max_worker = 100;
int main(int argc, char** argv)
{
    CmdOption option;
    option.ipv4_addr = "0.0.0.0";
    option.port = "80";
    int listen_sock;
    try

    {
        parseArg(argc, argv, option);
        Ipv4Address addr;
        addr.ip = parseIpAddr(option.ipv4_addr);
        addr.port = parsePort(option.port);
        listen_sock = listenOn(addr, 100);
    }
    catch (std::exception& err)
    {
        std::cout << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    ClientInfo client;
    int worker_count = 0;
    while (true)
    {
        // before blocking, try collecting all zombies
        while (worker_count > 0)
        {
            int wstatus;
            int cpid = waitpid(-1, &wstatus, WNOHANG);
            if (cpid < 0)
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            if (cpid == 0)
            {
                // no child exits for now
                break;
            }
            worker_count -= 1;
        }
        try
        {
            client = acceptClient(listen_sock);
        }
        catch (std::system_error& err)
        {
            std::cout << err.what() << std::endl;
            continue;
        }
        while (worker_count >= max_worker)
        {
            int wstatus;
            int cpid = waitpid(-1, &wstatus, 0);
            if (cpid < 0)
            {
                perror("waitpid");
                exit(EXIT_FAILURE);
            }
            worker_count -= 1;
        }
        auto pid = fork();
        if (pid < 0)
        {
            perror("fork");
            close(client.fd);
            continue;
        }
        else if (pid == 0)
        {
            break;
        }
        worker_count += 1;
        // parent part
        close(client.fd);
    }

    // child part
    bool keep_alive = true;
    int buff_size = 1 << 12;  // 4kb
    char header_buff[buff_size];
    setTimeout(client.fd, sock_timeout_sec);
    BufferedReader reader(client.fd, buff_size);
    int wfd = dup(client.fd);
    if (wfd < 0)
    {
        perror("dup");
        exit(EXIT_FAILURE);
    }
    Writer writer(wfd);
    int request_count = 0;
    try
    {
        while (keep_alive)
        {
            int header_size = readHeader(reader, header_buff, buff_size);

            auto header_raw = std::string(header_buff, header_size);

            auto rq = http::parseRequest(header_buff, header_size);

            std::cout << "http: " << http::methodString(rq.method);
            std::cout << " " << pathString(rq.url) << std::endl;

            if (rq.version == http::Version::v1_0)
            {
                keep_alive = false;
            }
            else
            {
                keep_alive = true;
            }
            if (rq.headers.find("Connection") != rq.headers.end())
            {
                if (rq.headers.at("Connection") == "keep-alive")
                {
                    keep_alive = true;
                }
                else
                {
                    keep_alive = false;
                }
            }
            request_count += 1;
            if (request_count >= max_request)
            {
                keep_alive = false;
            }
            http::Response resp;
            resp.version = http::Version::v1_1;
            resp.headers["Connection"] = keep_alive ? "keep-alive" : "close";
            resp.headers["Server"] = "wangnangg's private server";
            rootHandler(std::move(rq), reader, std::move(resp), writer);
        }
    }
    catch (std::exception& err)
    {
        std::cout << "error in process request: " << err.what() << std::endl;
        exit(EXIT_FAILURE);
    }
    return EXIT_SUCCESS;
}

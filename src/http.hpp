#pragma once

#include <cstdio>
#include <string>
#include <unordered_map>
#include <vector>
#include "path.hpp"

namespace http
{
enum class Method
{
    Get,
    Post
};

enum class Version
{
    v1_0,
    v1_1
};

struct Request
{
    Version version;
    Method method;
    Path url;
    std::unordered_map<std::string, std::string> headers;
};

enum class StatusCode
{
    OK = 200,
    NotFound = 404
};

struct Response
{
    Version version;
    StatusCode status;
    std::string reason;
    std::unordered_map<std::string, std::string> headers;
};

std::string dump(const Response& resp);

class client_error : public std::exception
{
    std::string msg;

public:
    client_error(std::string msg) : msg(msg) {}
    virtual const char* what() const noexcept { return msg.c_str(); }
};

std::vector<std::string> split(const char* buff, int size);
Request parseRequest(const char* buff, int size);

}  // namespace http

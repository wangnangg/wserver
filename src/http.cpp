#include "http.hpp"
#include <errno.h>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <vector>
#include "string.h"

namespace http
{
bool isWhite(char c)
{
    if (c == ' ' || c == '\r' || c == '\n')
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::string methodString(Method m)
{
    switch (m)
    {
        case Method::Get:
            return "Get";
        default:
            return "Post";
    }
}

std::vector<std::string> split(const char* buff, int size)
{
    std::vector<std::string> res;
    int start = 0;
    int curr;
    int end = size;
    while (true)
    {
        while (start < end && isWhite(buff[start]))
        {
            start += 1;
        }
        curr = start;
        while (curr < end && !isWhite(buff[curr]))
        {
            curr += 1;
        }
        if (start < curr)
        {
            res.emplace_back(buff + start, curr - start);
        }
        else
        {
            break;
        }
        start = curr;
    }
    return res;
}

Method parseMethod(const std::string& mstr)
{
    if (mstr == "GET")
    {
        return Method::Get;
    }
    else if (mstr == "POST")
    {
        return Method::Post;
    }
    else
    {
        throw client_error("Unsupported http method: " + mstr);
    }
}

Version parseVersion(const std::string& ver)
{
    if (ver == "HTTP/1.0")
    {
        return Version::v1_0;
    }
    else if (ver == "HTTP/1.1")
    {
        return Version::v1_1;
    }
    else
    {
        throw client_error("Unsupported http version: " + ver);
    }
}

std::tuple<const char*, const char*> strip(const char* start, const char* end)
{
    while (start < end && isWhite(*start))
    {
        start += 1;
    }
    while (start < end && isWhite(*(end - 1)))
    {
        end -= 1;
    }
    return {start, end};
}

int lineSize(const char* buff, int size)
{
    if (size < 2)
    {
        return 0;
    }
    for (int i = 0; i < size - 1; i++)
    {
        if (buff[i] == '\r' && buff[i + 1] == '\n')
        {
            return i + 2;
        }
    }
    return 0;
}

// line buff should be ended with \r\n\r\n
Request parseRequest(const char* buff, int size)
{
    assert(std::strncmp(buff + size - 4, "\r\n\r\n", 4) == 0);
    const char* line_buff = buff;
    int line_size = lineSize(line_buff, size);
    size -= line_size;
    buff += line_size;
    auto words = split(line_buff, line_size);
    if (words.size() != 3)
    {
        throw client_error("first line in request needs exactly three words");
    }
    auto method = parseMethod(words[0]);
    auto url = parseUrl(words[1]);
    auto version = parseVersion(words[2]);
    auto headers = std::unordered_map<std::string, std::string>();
    while (size > 0)
    {
        line_buff = buff;
        line_size = lineSize(line_buff, size);
        assert(line_size > 0);
        size -= line_size;
        buff += line_size;
        if (line_size > 2)
        {
            const char* colon_pos = strchr(line_buff, ':');
            if (colon_pos == NULL)
            {
                throw client_error("Invalid http header encountered.");
            }
            auto [key_start, key_end] = strip(line_buff, colon_pos);
            auto [val_start, val_end] =
                strip(colon_pos + 1, line_buff + line_size);
            auto key = std::string(key_start, key_end - key_start);
            auto val = std::string(val_start, val_end - val_start);
            if (key.length() == 0 || val.length() == 0)
            {
                throw client_error("key or val is empty in request header");
            }
            headers[key] = val;
        }
    }
    return {version, method, url, headers};
}

std::string dump(const Response& resp)
{
    std::stringstream os;
    const char* lend = "\r\n";
    switch (resp.version)
    {
        case http::Version::v1_0:
            os << "HTTP/1.0 ";
        case http::Version::v1_1:
        default:
            os << "HTTP/1.1 ";
    }
    os << (int)resp.status << " ";
    os << resp.reason << lend;
    for (auto [key, val] : resp.headers)
    {
        os << key << ": " << val << lend;
    }
    os << lend;
    return os.str();
}

}  // namespace http

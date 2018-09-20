#pragma once
#include <string>
#include <unordered_map>
#include <vector>

struct KeyVal
{
    std::string key;
    std::string val;
};

struct Url
{
    std::vector<std::string> path;
    std::vector<KeyVal> params;
};

Url parseUrl(const std::string& url_str);
std::string urlString(const Url& url);
std::string pathString(const std::vector<std::string>& path);

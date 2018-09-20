#include "path.hpp"
#include <sstream>
#include <stdexcept>

int search(const std::string& str, int start, int end, char c)
{
    for (int i = start; i < end; i++)
    {
        if (str[i] == c)
        {
            return i;
        }
    }
    return end;
}

bool isGoodChar(char c)
{
    if (c >= '0' && c <= '9')
    {
        return true;
    }
    if (c >= 'a' && c <= 'z')
    {
        return true;
    }
    if (c >= 'A' && c <= 'Z')
    {
        return true;
    }
    if (c == '_')
    {
        return true;
    }
    return false;
}

bool isGoodIdentifier(const std::string& name)
{
    if (name.size() == 0)
    {
        return false;
    }
    for (char c : name)
    {
        if (!isGoodChar(c))
        {
            return false;
        }
    }
    return true;
}

bool isValidPathName(const std::string& name)
{
    if (name.size() == 0)
    {
        return false;
    }
    if (name == "." || name == "..")
    {
        return false;
    }
    for (char c : name)
    {
        if (!isGoodChar(c) && c != '.')
        {
            return false;
        }
    }
    return true;
}

bool isValidKeyName(const std::string& key) { return isGoodIdentifier(key); }
bool isValidValName(const std::string& val) { return isGoodIdentifier(val); }

bool parseKeyVal(const std::string& str, KeyVal& res)
{
    int len = (int)str.length();
    int eq_pos = search(str, 0, len, '=');
    if (eq_pos == len)
    {
        return false;
    }
    res.key = std::string(str, 0, eq_pos);
    res.val = std::string(str, eq_pos + 1, len - eq_pos - 1);
    return isValidKeyName(res.key) && isValidValName(res.val);
}

Url parseUrl(const std::string& url_str)
{
    if (url_str.length() == 0 || url_str[0] != '/')
    {
        throw std::invalid_argument("invalid url: " + url_str);
    }
    Url url;
    int qpos = search(url_str, 0, url_str.length(), '?');
    int start = 1, end;

    while (start < qpos)
    {
        end = search(url_str, start, qpos, '/');
        if (start == end)
        {
            throw std::invalid_argument("invalid url: " + url_str);
        }
        std::string name = std::string(url_str, start, end - start);
        if (!isValidPathName(name))
        {
            throw std::invalid_argument("invalid url: " + url_str);
        }
        url.path.push_back(std::move(name));
        start = end + 1;
    }

    start = qpos + 1;
    int len = (int)url_str.length();
    while (start < len)
    {
        end = search(url_str, start, len, '&');
        if (start == end)
        {
            throw std::invalid_argument("invalid url: " + url_str);
        }
        KeyVal keyval;
        if (!parseKeyVal(std::string(url_str, start, end - start), keyval))
        {
            throw std::invalid_argument("invalid url: " + url_str);
        }
        url.params.push_back(keyval);
        start = end + 1;
    }
    return url;
}

std::string urlString(const Url& url)
{
    std::stringstream ss;
    ss << "/" << pathString(url.path);
    if (url.params.size() > 0)
    {
        ss << "?";
        for (size_t i = 0; i < url.params.size() - 1; i++)
        {
            ss << url.params[i].key << "=" << url.params[i].val << "&";
        }
        ss << url.params.back().key << "=" << url.params.back().val;
    }
    return ss.str();
}

std::string pathString(const std::vector<std::string>& path)
{
    std::stringstream ss;
    if (path.size() > 0)
    {
        for (size_t i = 0; i < path.size() - 1; i++)
        {
            ss << path[i] << "/";
        }
        ss << path.back();
    }
    return ss.str();
}

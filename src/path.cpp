#include "path.hpp"
#include <stdexcept>

int search(const std::string& str, int start, char c)
{
    for (size_t i = start; i < str.length(); i++)
    {
        if (str[i] == c)
        {
            return i;
        }
    }
    return str.length();
}

bool isValidPathName(const std::string& name)
{
    if (name.size() == 0)
    {
        return false;
    }
    if (name[0] == '.')
    {
        return false;
    }
    return true;
}

void Path::parsePath(const std::string& path)
{
    if (path.length() == 0)
    {
        throw std::invalid_argument("invalid path: " + path);
    }

    int start, end;
    if (path[0] == '/')
    {
        start = 1;
    }
    else
    {
        start = 0;
    }

    std::string name;
    while (start < (int)path.length())
    {
        end = search(path, start, '/');
        if (start == end)
        {
            throw std::invalid_argument("invalid path: " + path);
        }
        name = std::string(path, start, end - start);
        if (!isValidPathName(name))
        {
            throw std::invalid_argument("invalid path: " + path);
        }
        _nodes.push_back(Path::Node(std::move(name)));
        start = end + 1;
    }
}

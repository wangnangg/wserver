#include "reader.hpp"
#include <sys/socket.h>
#include <algorithm>
#include <stdexcept>
#include <system_error>

int BufferedReader::get(char* buff, int size)
{
    if (_buff.size() > 0)
    {
        int read_size = std::min(size, (int)_buff.size());
        auto first = &_buff.back();
        for (int i = 0; i < read_size; i++)
        {
            buff[i] = *first;
            first--;
        }
        _buff.resize(_buff.size() - read_size);
        return read_size;
    }

    int read_size = read(_fd, buff, size);
    if (read_size < 0)
    {
        throw std::system_error(errno, std::system_category());
    }
    return read_size;
}

void BufferedReader::put(char* buff, int size)
{
    if ((int)_buff.size() + size > _max_size)
    {
        throw std::runtime_error("BufferedReader buffed too much");
    }
    _buff.resize(_buff.size() + size);
    auto first = &_buff.back();
    for (int i = 0; i < size; i++)
    {
        *first = buff[i];
        first--;
    }
}

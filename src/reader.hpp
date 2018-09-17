#pragma once
#include <unistd.h>
#include <vector>

class BufferedReader
{
public:
    BufferedReader() : _fd(-1), _max_size(0) {}
    BufferedReader(int fd, int buff_max_size)
        : _fd(fd), _max_size(buff_max_size)
    {
    }
    ~BufferedReader()
    {
        if (_fd >= 0)
        {
            close(_fd);
        }
    }
    BufferedReader(const BufferedReader&) = delete;
    BufferedReader& operator=(const BufferedReader&) = delete;
    BufferedReader(BufferedReader&& reader)
    {
        _fd = reader._fd;
        _max_size = reader._max_size;
        _buff = std::move(reader._buff);
        reader._fd = -1;
    }
    BufferedReader& operator=(BufferedReader&& reader)
    {
        _fd = reader._fd;
        _max_size = reader._max_size;
        _buff = std::move(reader._buff);
        reader._fd = -1;
        return *this;
    }

    int get(char* buff, int size);
    void put(char* buff, int size);

private:
    std::vector<char> _buff;
    int _fd;
    int _max_size;
};

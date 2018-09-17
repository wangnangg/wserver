#pragma once

class Writer
{
    int _fd;

public:
    Writer(int fd) : _fd(fd) {}
    ~Writer();
    Writer(const Writer&) = delete;
    Writer& operator=(const Writer&) = delete;

    void put(const char* buff, int size);
};

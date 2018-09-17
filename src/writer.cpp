#include "writer.hpp"
#include <unistd.h>
#include <system_error>

void Writer::put(const char* buff, int size)
{
    while (size > 0)
    {
        int write_size = write(_fd, buff, size);
        if (write_size == 0)
        {
            throw std::runtime_error("0 byte is written to the fd");
        }
        if (write_size < 0)
        {
            throw std::system_error(errno, std::system_category());
        }
        size -= write_size;
    }
}

Writer::~Writer() { close(_fd); }


#include "reader.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include "gtest/gtest.h"
#include "unistd.h"

TEST(server, BufferedReader)
{
    int fd = open("/dev/random", O_RDONLY);
    BufferedReader reader(fd, 1 << 10);
    char buff[1000];
    int read_size = reader.get(buff, 10);
    ASSERT_LE(read_size, 10);
    char content[] = "123";
    reader.put(content, 3);
    read_size = reader.get(buff, 10);
    ASSERT_EQ(read_size, 3);
    ASSERT_EQ(std::string(buff, 3), std::string(content, 3));
}

#include "http_handler.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <system_error>

void writeAll(Writer& writer, BufferedReader& reader)
{
    int buff_size = 1 << 12;
    char buff[buff_size];
    int read_size;
    do
    {
        read_size = reader.get(buff, buff_size);
        writer.put(buff, read_size);
    } while (read_size > 0);
}

void serveFile(const std::string& path, http::Response resp, Writer& writer)
{
    int fd = open(path.c_str(), O_RDONLY);
    if (fd < 0)
    {
        throw std::system_error(errno, std::system_category());
    }
    struct stat st;
    int err = fstat(fd, &st);
    if (err < 0)
    {
        throw std::system_error(errno, std::system_category());
    }
    int fsize = st.st_size;
    auto reader = BufferedReader(fd, 0);

    resp.headers["Content-Length"] = std::to_string(fsize);
    auto resp_str = dump(resp);
    std::cout << "response:\n " << resp_str << std::endl;
    writer.put(resp_str.c_str(), resp_str.size());
    writeAll(writer, reader);
}

void rootHandler(http::Request rq, BufferedReader& reader,
                 http::Response resp, Writer& writer)
{
    if (rq.url.size() == 0)
    {
        // root request
        resp.status = http::StatusCode::OK;
        resp.reason = "OK";
        serveFile("static/html/index.html", std::move(resp), writer);
        return;
    }

    // unknown place
    resp.status = http::StatusCode::NotFound;
    resp.reason = "Not Found";
    serveFile("static/html/404.html", std::move(resp), writer);
}

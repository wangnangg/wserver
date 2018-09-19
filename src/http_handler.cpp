#include "http_handler.hpp"
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <iostream>
#include <sstream>
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

    std::cout << "serving file: " << path << std::endl;
    resp.headers["Content-Length"] = std::to_string(fsize);
    auto resp_str = dump(resp);
    writer.put(resp_str.c_str(), resp_str.size());
    writeAll(writer, reader);
}

void redirect(const std::string& path, http::Response resp, Writer& writer)
{
    std::cout << "redirecting to " << pathString(path) << std::endl;
    resp.status = http::StatusCode::MovedPermanently;
    resp.reason = "Moved Permanently";
    resp.headers["Location"] = path;
    auto resp_str = dump(resp);
    writer.put(resp_str.c_str(), resp_str.size());
}

bool isReadableFile(const std::string& path)
{
    if (access(path.c_str(), R_OK) != 0)
    {
        return false;
    }
    struct stat st;
    if (stat(path.c_str(), &st) != 0)
    {
        return false;
    }
    if (!S_ISREG(st.st_mode))
    {
        return false;
    }
    return true;
}

void rootHandler(http::Request rq, BufferedReader& reader,
                 http::Response resp, Writer& writer)
{
    const auto url_p = pathString(rq.url);

    if (rq.url.size() == 0)
    {
        // root request
        rq.url = Path("/index.html");
    }

    if (rq.url.begin()->name() == "static")
    {
        if (!isReadableFile(url_p))
        {
            std::cout << "file not found" << std::endl;
            goto out;
        }
        resp.status = http::StatusCode::OK;
        resp.reason = "OK";
        serveFile(pathString(rq.url), std::move(resp), writer);
        return;
    }

out:
    // unknown place
    std::cout << "url not found" << std::endl;
    resp.status = http::StatusCode::NotFound;
    resp.reason = "Not Found";
    serveFile("404.html", std::move(resp), writer);
    return;
}

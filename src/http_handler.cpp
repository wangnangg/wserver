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

static const std::string page404 = "/static/html/404.html";

void serveFile(const std::string& path, http::Response resp, Writer& writer)
{
    std::cout << "serving file: " << path << std::endl;
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
    std::cout << "response:\n" << resp_str << std::endl;
    writer.put(resp_str.c_str(), resp_str.size());
    writeAll(writer, reader);
}

void rootHandler(http::Request rq, BufferedReader& reader,
                 http::Response resp, Writer& writer)
{
    if (rq.url.size() == 0)
    {
        // root request
        resp.status = http::StatusCode::MovedPermanently;
        resp.reason = "Moved Permanently";
        resp.headers["Location"] = "/static/html/index.html";
        auto resp_str = dump(resp);
        writer.put(resp_str.c_str(), resp_str.size());
        return;
    }

    if (rq.url.begin()->name() == "static")
    {
        auto url_p = pathString(rq.url);
        if (access(url_p.c_str(), R_OK) != 0)
        {
            goto out;
        }
        resp.status = http::StatusCode::OK;
        resp.reason = "OK";
        serveFile(pathString(rq.url), std::move(resp), writer);
        return;
    }

out:
    // unknown place
    if (pathString(rq.url) == page404)
    {
        throw std::runtime_error("404 page itself is not found");
    }
    resp.status = http::StatusCode::MovedPermanently;
    resp.reason = "Moved Permanently";
    resp.headers["Location"] = page404;
    auto resp_str = dump(resp);
    writer.put(resp_str.c_str(), resp_str.size());
}

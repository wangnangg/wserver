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

void redirect(const Url& url, http::Response resp, Writer& writer)
{
    std::cout << "redirecting to " << urlString(url) << std::endl;
    resp.status = http::StatusCode::MovedPermanently;
    resp.reason = "Moved Permanently";
    resp.headers["Location"] = urlString(url);
    auto resp_str = dump(resp);
    writer.put(resp_str.c_str(), resp_str.size());
}

bool isRegFile(const std::string& path)
{
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

bool isReadFile(const std::string& path)
{
    if (!isRegFile(path))
    {
        return false;
    }
    if (access(path.c_str(), R_OK) != 0)
    {
        return false;
    }
    return true;
}

bool isExeFile(const std::string& path)
{
    if (!isRegFile(path))
    {
        return false;
    }
    if (access(path.c_str(), X_OK) != 0)
    {
        return false;
    }
    return true;
}

std::string localPath(const std::string& web_dir,
                      const std::vector<std::string>& path)
{
    std::stringstream ss;
    ss << web_dir << "/" << pathString(path);
    return ss.str();
}

bool serveFile(const Url& url, const std::string& web_dir,
               http::Response resp, Writer& writer)
{
    auto local_path = localPath(web_dir, url.path);
    if (!isReadFile(local_path))
    {
        return false;
    }

    int fd = open(local_path.c_str(), O_RDONLY);
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
    writer.put(resp_str.c_str(), resp_str.size());
    writeAll(writer, reader);
    return true;
}

bool serveDynamicFile(const Url& url, const std::string& web_dir,
                      http::Response resp, Writer& writer)
{
    return true;
}

bool rootHandler(http::Request rq, BufferedReader& reader,
                 http::Response resp, Writer& writer,
                 const std::string& web_dir)
{
    if (rq.url.path.size() == 0)
    {
        // root request
        rq.url = parseUrl("/static/src/index.html");
    }
    if (rq.url.path.size() == 1 && rq.url.path[0] == "cv")
    {
        redirect(parseUrl("/static/src/cv.html"), resp, writer);
        return true;
    }
    const auto local_path = localPath(web_dir, rq.url.path);

    if (rq.url.path.front() == "static")
    {
        resp.status = http::StatusCode::OK;
        resp.reason = "OK";
        if (serveFile(rq.url, web_dir, resp, writer))
        {
            return true;
        }
    }
    else if (rq.url.path.front() == "dynamic")
    {
        resp.status = http::StatusCode::OK;
        resp.reason = "OK";
        if (serveDynamicFile(rq.url, web_dir, resp, writer))
        {
            return true;
        }
    }

    // not found
    resp.status = http::StatusCode::NotFound;
    resp.reason = "Not Found";
    auto resp_str = dump(resp);
    writer.put(resp_str.c_str(), resp_str.size());
    return false;
}

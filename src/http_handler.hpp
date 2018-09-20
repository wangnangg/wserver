#pragma once
#include <functional>
#include "http.hpp"
#include "path.hpp"
#include "reader.hpp"
#include "writer.hpp"

void rootHandler(http::Request rq, BufferedReader& reader,
                 http::Response resp, Writer& writer,
                 const std::string& web_dir);

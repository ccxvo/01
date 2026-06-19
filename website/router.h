#pragma once

#include "http.h"

#include <string>
#include <functional>

namespace router {

// Handler type: takes HttpRequest, returns full HTTP response string
using HandlerFunc = std::function<std::string(const http::HttpRequest&)>;

// Initialize all routes (must be called before dispatch)
void init_routes();

// Dispatch request to the matching handler
std::string dispatch(const http::HttpRequest& req);

} // namespace router

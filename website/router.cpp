#include "router.h"
#include "handlers.h"

#include <vector>

namespace router {

struct Route {
    std::string method;         // "GET" or "POST"
    std::string pattern;        // e.g. "/login", "/post/", "/comment/"
    bool prefix_match;          // true for patterns like "/post/" that match "/post/5"
    HandlerFunc handler;
};

static std::vector<Route> g_routes;

void init_routes() {
    g_routes.clear();

    // GET routes
    g_routes.push_back({"GET", "/",                  false, handlers::handle_index});
    g_routes.push_back({"GET", "/register",          false, handlers::handle_register_form});
    g_routes.push_back({"GET", "/login",             false, handlers::handle_login_form});
    g_routes.push_back({"GET", "/logout",            false, handlers::handle_logout});
    g_routes.push_back({"GET", "/publish",           false, handlers::handle_publish_form});
    g_routes.push_back({"GET", "/post/",             true,  handlers::handle_view_post});

    // POST routes
    g_routes.push_back({"POST", "/register",         false, handlers::handle_register_submit});
    g_routes.push_back({"POST", "/login",            false, handlers::handle_login_submit});
    g_routes.push_back({"POST", "/publish",          false, handlers::handle_publish_submit});
    g_routes.push_back({"POST", "/comment/",         true,  handlers::handle_comment_submit});
}

std::string dispatch(const http::HttpRequest& req) {
    for (const auto& route : g_routes) {
        if (route.method != req.method) continue;

        if (route.prefix_match) {
            // Prefix match: "/post/" matches "/post/5", "/post/123", etc.
            if (req.path.size() >= route.pattern.size() &&
                req.path.substr(0, route.pattern.size()) == route.pattern) {
                return route.handler(req);
            }
        } else {
            // Exact match
            if (req.path == route.pattern) {
                return route.handler(req);
            }
        }
    }

    // No route matched — return 404
    return handlers::handle_404(req);
}

} // namespace router

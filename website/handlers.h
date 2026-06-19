#pragma once

#include "http.h"

#include <string>

namespace handlers {

// GET  /                 — Homepage: post list
std::string handle_index(const http::HttpRequest& req);

// GET  /post/{id}        — View single post with comments
std::string handle_view_post(const http::HttpRequest& req);

// GET  /register         — Show registration form
std::string handle_register_form(const http::HttpRequest& req);

// POST /register         — Process registration
std::string handle_register_submit(const http::HttpRequest& req);

// GET  /login            — Show login form
std::string handle_login_form(const http::HttpRequest& req);

// POST /login            — Process login
std::string handle_login_submit(const http::HttpRequest& req);

// GET  /logout           — Destroy session
std::string handle_logout(const http::HttpRequest& req);

// GET  /publish          — Show publish form
std::string handle_publish_form(const http::HttpRequest& req);

// POST /publish          — Create new post
std::string handle_publish_submit(const http::HttpRequest& req);

// POST /comment/{id}     — Add comment to post
std::string handle_comment_submit(const http::HttpRequest& req);

// Catch-all  — 404 page
std::string handle_404(const http::HttpRequest& req);

} // namespace handlers

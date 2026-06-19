#pragma once

#include <string>
#include <string_view>
#include <unordered_map>

namespace http {

struct HttpRequest {
    std::string method;                                       // "GET" or "POST"
    std::string path;                                         // e.g. "/post/5"
    std::string query_string;                                 // everything after '?' (usually empty)
    std::unordered_map<std::string, std::string> headers;     // all headers
    std::unordered_map<std::string, std::string> cookies;     // parsed from Cookie header
    std::unordered_map<std::string, std::string> form_data;   // POST body, URL-decoded

    // Convenience: get cookie value (empty string if not present)
    std::string get_cookie(const std::string& name) const {
        auto it = cookies.find(name);
        return (it != cookies.end()) ? it->second : "";
    }

    // Convenience: get form field value (empty string if not present)
    std::string get_form(const std::string& name) const {
        auto it = form_data.find(name);
        return (it != form_data.end()) ? it->second : "";
    }
};

// Parse raw HTTP request bytes into HttpRequest struct
// Throws std::runtime_error on malformed requests
HttpRequest parse_http_request(const std::string& raw);

// Build a complete HTTP response string (status line + headers + body)
// extra_headers: e.g. {{"Set-Cookie", "session_id=abc; Path=/; HttpOnly"}, {"Location", "/"}}
std::string build_response(
    int status_code,
    const std::string& content_type,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& extra_headers = {});

// Status text lookup
const char* status_text(int code);

} // namespace http

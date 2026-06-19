#include "http.h"
#include "utils.h"

#include <stdexcept>
#include <sstream>
#include <vector>
#include <cstring>

namespace http {

// ========== Helper: split string by delimiter ==========

static std::vector<std::string> split(const std::string& s, const std::string& delim) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = s.find(delim);
    while (end != std::string::npos) {
        result.push_back(s.substr(start, end - start));
        start = end + delim.size();
        end = s.find(delim, start);
    }
    result.push_back(s.substr(start));
    return result;
}

// ========== Helper: trim whitespace ==========

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// ========== Status Text Lookup ==========

const char* status_text(int code) {
    switch (code) {
        case 200: return "OK";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 404: return "Not Found";
        case 500: return "Internal Server Error";
        default:  return "Unknown";
    }
}

// ========== Parse HTTP Request ==========

HttpRequest parse_http_request(const std::string& raw) {
    HttpRequest req;

    // Split into lines
    std::vector<std::string> lines = split(raw, "\r\n");
    if (lines.empty()) {
        throw std::runtime_error("空请求");
    }

    // Parse request line: "METHOD /path HTTP/1.1"
    {
        std::vector<std::string> parts = split(lines[0], " ");
        if (parts.size() < 2) {
            throw std::runtime_error("无效的请求行: " + lines[0]);
        }
        req.method = parts[0];

        // Separate path and query string
        std::string full_path = parts[1];
        size_t qpos = full_path.find('?');
        if (qpos != std::string::npos) {
            req.path = full_path.substr(0, qpos);
            req.query_string = full_path.substr(qpos + 1);
        } else {
            req.path = full_path;
        }
    }

    // Parse headers
    size_t blank_line = 0;
    for (size_t i = 1; i < lines.size(); ++i) {
        if (lines[i].empty()) {
            blank_line = i;
            break;
        }
        size_t colon = lines[i].find(':');
        if (colon != std::string::npos) {
            std::string key = trim(lines[i].substr(0, colon));
            std::string value = trim(lines[i].substr(colon + 1));
            // Normalize header key to lowercase for case-insensitive lookup
            for (char& c : key) { if (c >= 'A' && c <= 'Z') c += ('a' - 'A'); }
            req.headers[key] = value;
        }
    }

    // Parse cookies from Cookie header
    auto cookie_it = req.headers.find("cookie");
    if (cookie_it != req.headers.end()) {
        std::vector<std::string> cookie_pairs = split(cookie_it->second, ";");
        for (const auto& pair : cookie_pairs) {
            size_t eq = pair.find('=');
            if (eq != std::string::npos) {
                std::string key = trim(pair.substr(0, eq));
                std::string value = trim(pair.substr(eq + 1));
                req.cookies[key] = value;
            }
        }
    }

    // Parse POST body (application/x-www-form-urlencoded)
    if (req.method == "POST") {
        // Get body: everything after the blank line
        std::string body;
        if (blank_line > 0 && blank_line + 1 < lines.size()) {
            for (size_t i = blank_line + 1; i < lines.size(); ++i) {
                if (!body.empty()) body += "\r\n";
                body += lines[i];
            }
        }

        // Also check Content-Length to ensure we have the full body
        auto cl_it = req.headers.find("content-length");
        if (cl_it != req.headers.end() && !body.empty()) {
            // Parse form data
            std::vector<std::string> pairs = split(body, "&");
            for (const auto& pair : pairs) {
                if (pair.empty()) continue;
                size_t eq = pair.find('=');
                if (eq != std::string::npos) {
                    std::string key = utils::url_decode(pair.substr(0, eq));
                    std::string value = utils::url_decode(pair.substr(eq + 1));
                    req.form_data[key] = value;
                } else {
                    std::string key = utils::url_decode(pair);
                    req.form_data[key] = "";
                }
            }
        }
    }

    return req;
}

// ========== Build HTTP Response ==========

std::string build_response(
    int status_code,
    const std::string& content_type,
    const std::string& body,
    const std::unordered_map<std::string, std::string>& extra_headers)
{
    std::ostringstream response;

    // Status line
    response << "HTTP/1.1 " << status_code << " " << status_text(status_code) << "\r\n";

    // Standard headers
    response << "Content-Type: " << content_type << "\r\n";
    response << "Content-Length: " << body.size() << "\r\n";
    response << "Connection: close\r\n";
    response << "Server: CppBlog/1.0\r\n";

    // Extra headers (Set-Cookie, Location, etc.)
    for (const auto& [key, value] : extra_headers) {
        response << key << ": " << value << "\r\n";
    }

    // Blank line separating headers from body
    response << "\r\n";

    // Body
    response << body;

    return response.str();
}

} // namespace http

#pragma once

#include <string>
#include <optional>
#include <cstdint>

namespace session {

struct Session {
    std::string token;      // 64-char random hex string
    std::string username;   // logged-in username
    int64_t created_at;     // Unix timestamp
};

// Create a new session for a username. Returns the session token.
std::string create_session(const std::string& username);

// Validate a session token. Returns Session if valid, nullopt if expired/not found.
std::optional<Session> validate_session(const std::string& token);

// Destroy a session (logout)
void destroy_session(const std::string& token);

// Check if request has a valid session and return username, or empty string
std::string get_username_from_cookie(const std::string& cookie_value);

} // namespace session

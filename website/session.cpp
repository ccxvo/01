#include "session.h"
#include "utils.h"

#include <unordered_map>
#include <mutex>

namespace session {

static std::unordered_map<std::string, Session> g_sessions;
static std::mutex g_session_mutex;

std::string create_session(const std::string& username) {
    std::lock_guard<std::mutex> lock(g_session_mutex);

    std::string token = utils::random_hex(64);
    Session sess;
    sess.token = token;
    sess.username = username;
    sess.created_at = utils::current_timestamp();

    g_sessions[token] = sess;
    return token;
}

std::optional<Session> validate_session(const std::string& token) {
    std::lock_guard<std::mutex> lock(g_session_mutex);

    auto it = g_sessions.find(token);
    if (it == g_sessions.end()) {
        return std::nullopt;
    }
    return it->second;
}

void destroy_session(const std::string& token) {
    std::lock_guard<std::mutex> lock(g_session_mutex);

    auto it = g_sessions.find(token);
    if (it != g_sessions.end()) {
        g_sessions.erase(it);
    }
}

std::string get_username_from_cookie(const std::string& cookie_value) {
    if (cookie_value.empty()) return "";
    auto sess = validate_session(cookie_value);
    if (sess.has_value()) {
        return sess->username;
    }
    return "";
}

} // namespace session

#include "handlers.h"
#include "db.h"
#include "session.h"
#include "templates.h"
#include "utils.h"

#include <string>
#include <unordered_map>
#include <cstdlib>

namespace handlers {

// ========== Helper: get logged-in username from request ==========

static std::string get_logged_in_user(const http::HttpRequest& req) {
    std::string token = req.get_cookie("session_id");
    return session::get_username_from_cookie(token);
}

static bool is_logged_in(const http::HttpRequest& req) {
    return !get_logged_in_user(req).empty();
}

// ========== Helper: extract ID from path (e.g. "/post/5" -> 5) ==========

static int extract_id(const std::string& path, const std::string& prefix) {
    if (path.size() <= prefix.size()) return -1;
    std::string id_str = path.substr(prefix.size());
    // Parse integer
    try {
        return std::stoi(id_str);
    } catch (...) {
        return -1;
    }
}

// ========== Helper: redirect response ==========

static std::string redirect(const std::string& location,
                             const std::unordered_map<std::string, std::string>& extra = {}) {
    auto headers = extra;
    headers["Location"] = location;
    std::string body = "<!DOCTYPE html><html><head><meta http-equiv=\"refresh\" content=\"0;url="
                     + location + "\"></head><body>正在跳转到 <a href=\"" + location
                     + "\">" + location + "</a></body></html>";
    return http::build_response(302, "text/html; charset=utf-8", body, headers);
}

// ========== GET / ==========

std::string handle_index(const http::HttpRequest& req) {
    bool logged_in = is_logged_in(req);
    auto posts = db::post_list_all();
    std::string html = templates::render_post_list(posts, logged_in);
    return http::build_response(200, "text/html; charset=utf-8", html);
}

// ========== GET /post/{id} ==========

std::string handle_view_post(const http::HttpRequest& req) {
    bool logged_in = is_logged_in(req);
    int post_id = extract_id(req.path, "/post/");

    if (post_id <= 0) {
        std::string html = templates::render_404(logged_in);
        return http::build_response(404, "text/html; charset=utf-8", html);
    }

    auto meta = db::post_find_by_id(post_id);
    if (!meta.has_value()) {
        std::string html = templates::render_404(logged_in);
        return http::build_response(404, "text/html; charset=utf-8", html);
    }

    std::string content = db::post_read_content(post_id);
    auto comments = db::comment_list_by_post(post_id);
    std::string html = templates::render_post_detail(*meta, content, comments, logged_in);
    return http::build_response(200, "text/html; charset=utf-8", html);
}

// ========== GET /register ==========

std::string handle_register_form(const http::HttpRequest& req) {
    bool logged_in = is_logged_in(req);
    if (logged_in) {
        return redirect("/");
    }
    std::string html = templates::render_register_form({}, false);
    return http::build_response(200, "text/html; charset=utf-8", html);
}

// ========== POST /register ==========

std::string handle_register_submit(const http::HttpRequest& req) {
    bool logged_in = is_logged_in(req);
    if (logged_in) {
        return redirect("/");
    }

    std::string username = req.get_form("username");
    std::string email    = req.get_form("email");
    std::string password = req.get_form("password");
    std::string confirm  = req.get_form("confirm");

    std::unordered_map<std::string, std::string> errors;

    // Validate passwords match
    if (password != confirm) {
        errors["confirm"] = "两次输入的密码不一致。";
    }

    // Validate password length
    if (password.size() < 6) {
        errors["password"] = "密码长度不能少于6位。";
    }

    // Validate username
    if (!utils::is_valid_username(username)) {
        errors["username"] = "用户名需为3-31位字符，仅支持字母、数字和下划线。";
    }

    // Validate email
    if (!utils::is_valid_email(email)) {
        errors["email"] = "邮箱格式不正确。";
    }

    // Check if username already exists
    if (errors.empty()) {
        auto existing = db::user_find_by_username(username);
        if (existing.has_value()) {
            errors["username"] = "该用户名已被注册。";
        }
    }

    // If any errors, re-render form with errors
    if (!errors.empty()) {
        std::string html = templates::render_register_form(errors, false);
        return http::build_response(200, "text/html; charset=utf-8", html);
    }

    // Create user
    db::User user = {};
    user.id = 0; // Will be assigned by db::user_create
    user.deleted = 0;
    db::str_to_buf(user.username, username);
    db::str_to_buf(user.email, email);

    // Hash password with salt
    std::string salt = utils::random_hex(16);
    std::string hash = utils::sha256(salt + password);
    std::string stored = salt + ":" + hash;
    db::str_to_buf(user.password_hash, stored);

    int32_t user_id = db::user_create(user);
    if (user_id < 0) {
        errors["general"] = "注册失败，用户名可能已被占用。";
        std::string html = templates::render_register_form(errors, false);
        return http::build_response(200, "text/html; charset=utf-8", html);
    }

    // Redirect to login page with success message (via query param or just redirect)
    return redirect("/login");
}

// ========== GET /login ==========

std::string handle_login_form(const http::HttpRequest& req) {
    bool logged_in = is_logged_in(req);
    if (logged_in) {
        return redirect("/");
    }
    std::string html = templates::render_login_form("", false);
    return http::build_response(200, "text/html; charset=utf-8", html);
}

// ========== POST /login ==========

std::string handle_login_submit(const http::HttpRequest& req) {
    bool logged_in = is_logged_in(req);
    if (logged_in) {
        return redirect("/");
    }

    std::string username = req.get_form("username");
    std::string password = req.get_form("password");

    // Validate inputs
    if (username.empty() || password.empty()) {
        std::string html = templates::render_login_form("请填写所有字段。", false);
        return http::build_response(200, "text/html; charset=utf-8", html);
    }

    // Find user
    auto user = db::user_find_by_username(username);
    if (!user.has_value()) {
        std::string html = templates::render_login_form("用户名或密码错误。", false);
        return http::build_response(200, "text/html; charset=utf-8", html);
    }

    // Verify password
    std::string stored = db::buf_to_str(user->password_hash);
    size_t colon = stored.find(':');
    if (colon == std::string::npos) {
        std::string html = templates::render_login_form("账号异常，请联系管理员。", false);
        return http::build_response(200, "text/html; charset=utf-8", html);
    }

    std::string salt = stored.substr(0, colon);
    std::string expected_hash = stored.substr(colon + 1);
    std::string actual_hash = utils::sha256(salt + password);

    if (actual_hash != expected_hash) {
        std::string html = templates::render_login_form("用户名或密码错误。", false);
        return http::build_response(200, "text/html; charset=utf-8", html);
    }

    // Create session and set cookie
    std::string token = session::create_session(username);
    std::string html = templates::render_page("登录成功",
        "<div class=\"card\" style=\"text-align:center;\">"
        "<h2 style=\"color:#27ae60;\">✅ 登录成功！</h2>"
        "<p>正在跳转到首页...</p>"
        "<a href=\"/\" class=\"btn\" style=\"margin-top:16px;display:inline-block;\">前往首页</a>"
        "</div>", true);
    return http::build_response(200, "text/html; charset=utf-8", html,
        {{"Set-Cookie", "session_id=" + token + "; Path=/; HttpOnly; Max-Age=86400"}});
}

// ========== GET /logout ==========

std::string handle_logout(const http::HttpRequest& req) {
    std::string token = req.get_cookie("session_id");
    if (!token.empty()) {
        session::destroy_session(token);
    }

    // Clear cookie by setting Max-Age=0
    return redirect("/", {{"Set-Cookie", "session_id=deleted; Path=/; HttpOnly; Max-Age=0"}});
}

// ========== GET /publish ==========

std::string handle_publish_form(const http::HttpRequest& req) {
    bool logged_in = is_logged_in(req);
    if (!logged_in) {
        return redirect("/login");
    }
    std::string html = templates::render_publish_form(true);
    return http::build_response(200, "text/html; charset=utf-8", html);
}

// ========== POST /publish ==========

std::string handle_publish_submit(const http::HttpRequest& req) {
    std::string username = get_logged_in_user(req);
    if (username.empty()) {
        return redirect("/login");
    }

    std::string title = req.get_form("title");
    std::string content = req.get_form("content");

    // Validate
    if (title.empty() || content.empty()) {
        std::string html = templates::render_page("错误",
            "<div class=\"card\"><div class=\"alert alert-error\">标题和内容不能为空。</div>"
            "<a href=\"/publish\" class=\"btn\">返回</a></div>", true);
        return http::build_response(200, "text/html; charset=utf-8", html);
    }

    if (title.size() > 127) {
        title = title.substr(0, 127);
    }

    // Create summary from first 250 chars of content
    std::string summary = content.substr(0, 250);
    // Remove newlines in summary for cleaner display
    for (char& c : summary) {
        if (c == '\n' || c == '\r') c = ' ';
    }

    // Build post meta
    db::PostMeta meta = {};
    meta.id = 0; // Will be assigned
    meta.deleted = 0;
    meta.timestamp = utils::current_timestamp();
    meta.comment_count = 0;
    db::str_to_buf(meta.author, username);
    db::str_to_buf(meta.title, title);
    db::str_to_buf(meta.summary, summary);

    int32_t post_id = db::post_create(meta, content);
    if (post_id < 0) {
        std::string html = templates::render_page("错误",
            "<div class=\"card\"><div class=\"alert alert-error\">文章发布失败，请重试。</div>"
            "<a href=\"/publish\" class=\"btn\">返回</a></div>", true);
        return http::build_response(200, "text/html; charset=utf-8", html);
    }

    // Redirect to the new post
    return redirect("/post/" + std::to_string(post_id));
}

// ========== POST /comment/{id} ==========

std::string handle_comment_submit(const http::HttpRequest& req) {
    int post_id = extract_id(req.path, "/comment/");
    if (post_id <= 0) {
        return redirect("/");
    }

    // Verify post exists
    auto meta = db::post_find_by_id(post_id);
    if (!meta.has_value()) {
        return redirect("/");
    }

    std::string nickname = req.get_form("nickname");
    std::string content = req.get_form("content");

    // Validate
    if (nickname.empty()) nickname = "匿名用户";
    if (content.empty()) {
        // Redirect back to post page (comment will just not appear)
        return redirect("/post/" + std::to_string(post_id));
    }

    // Truncate if needed
    if (nickname.size() > 31) nickname = nickname.substr(0, 31);
    if (content.size() > 1023) content = content.substr(0, 1023);

    // Create comment
    db::Comment comment = {};
    comment.id = 0;
    comment.post_id = post_id;
    comment.deleted = 0;
    comment.timestamp = utils::current_timestamp();
    db::str_to_buf(comment.nickname, nickname);
    db::str_to_buf(comment.content, content);

    int32_t comment_id = db::comment_create(comment);
    if (comment_id >= 0) {
        // Increment post's comment count
        db::post_increment_comment_count(post_id);
    }

    // Redirect back to the post
    return redirect("/post/" + std::to_string(post_id));
}

// ========== 404 ==========

std::string handle_404(const http::HttpRequest& req) {
    bool logged_in = is_logged_in(req);
    std::string html = templates::render_404(logged_in);
    return http::build_response(404, "text/html; charset=utf-8", html);
}

} // namespace handlers

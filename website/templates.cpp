#include "templates.h"
#include "utils.h"

#include <sstream>
#include <ctime>
#include <iomanip>

namespace templates {

// ========== Common CSS (embedded in every page) ==========

static const char* COMMON_CSS = R"(<style>
* { margin: 0; padding: 0; box-sizing: border-box; }
body {
    font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", "Microsoft YaHei", Roboto, "Helvetica Neue", Arial, sans-serif;
    background: #f0f2f5;
    color: #1a1a2e;
    line-height: 1.6;
    min-height: 100vh;
}
/* --- Nav Bar --- */
.navbar {
    background: #ffffff;
    border-bottom: 1px solid #e4e6eb;
    padding: 0 24px;
    height: 60px;
    display: flex;
    align-items: center;
    justify-content: space-between;
    position: sticky;
    top: 0;
    z-index: 100;
    box-shadow: 0 1px 3px rgba(0,0,0,0.06);
}
.navbar .logo {
    font-size: 22px;
    font-weight: 700;
    color: #4a6cf7;
    text-decoration: none;
    letter-spacing: -0.5px;
}
.navbar .logo:hover { color: #3651d5; }
.navbar .nav-links { display: flex; gap: 8px; align-items: center; }
.navbar .nav-links a {
    color: #555;
    text-decoration: none;
    padding: 8px 18px;
    border-radius: 8px;
    font-size: 14px;
    font-weight: 500;
    transition: all 0.2s;
}
.navbar .nav-links a:hover {
    background: #f0f2f5;
    color: #4a6cf7;
}
.navbar .nav-links a.btn-primary {
    background: #4a6cf7;
    color: #fff;
}
.navbar .nav-links a.btn-primary:hover {
    background: #3651d5;
}
/* --- Container --- */
.container { max-width: 800px; margin: 0 auto; padding: 32px 20px; }
/* --- Cards --- */
.card {
    background: #ffffff;
    border-radius: 12px;
    padding: 28px 32px;
    margin-bottom: 20px;
    box-shadow: 0 1px 4px rgba(0,0,0,0.06);
    border: 1px solid #e8eaed;
    transition: box-shadow 0.2s;
}
.card:hover { box-shadow: 0 4px 16px rgba(0,0,0,0.10); }
.card h2 { font-size: 20px; margin-bottom: 8px; color: #1a1a2e; }
.card h2 a { color: #1a1a2e; text-decoration: none; }
.card h2 a:hover { color: #4a6cf7; }
.card .meta {
    font-size: 13px;
    color: #888;
    margin-bottom: 12px;
    display: flex;
    gap: 16px;
    align-items: center;
}
.card .summary { font-size: 15px; color: #555; line-height: 1.7; }
.card .read-more {
    display: inline-block;
    margin-top: 12px;
    color: #4a6cf7;
    font-weight: 600;
    font-size: 14px;
    text-decoration: none;
}
.card .read-more:hover { text-decoration: underline; }
/* --- Forms --- */
.form-card {
    max-width: 440px;
    margin: 40px auto;
}
.form-card.wide { max-width: 700px; }
.form-group { margin-bottom: 20px; }
.form-group label {
    display: block;
    font-size: 14px;
    font-weight: 600;
    color: #333;
    margin-bottom: 6px;
}
.form-group input, .form-group textarea {
    width: 100%;
    padding: 11px 14px;
    border: 1.5px solid #dde;
    border-radius: 8px;
    font-size: 15px;
    font-family: inherit;
    transition: border-color 0.2s;
    background: #fafbfc;
}
.form-group input:focus, .form-group textarea:focus {
    outline: none;
    border-color: #4a6cf7;
    background: #fff;
    box-shadow: 0 0 0 3px rgba(74,108,247,0.12);
}
.form-group textarea { resize: vertical; min-height: 120px; }
.form-group .field-error {
    color: #e74c3c;
    font-size: 13px;
    margin-top: 4px;
}
.btn {
    display: inline-block;
    padding: 11px 28px;
    background: #4a6cf7;
    color: #fff;
    border: none;
    border-radius: 8px;
    font-size: 15px;
    font-weight: 600;
    cursor: pointer;
    text-decoration: none;
    transition: background 0.2s;
    font-family: inherit;
}
.btn:hover { background: #3651d5; }
.btn-block { display: block; width: 100%; }
/* --- Alerts --- */
.alert {
    padding: 12px 16px;
    border-radius: 8px;
    margin-bottom: 16px;
    font-size: 14px;
    font-weight: 500;
}
.alert-error { background: #fdecea; color: #c0392b; border: 1px solid #f5c6cb; }
.alert-success { background: #eafaf1; color: #27ae60; border: 1px solid #b7ebcc; }
/* --- Post Content --- */
.post-content {
    font-size: 16px;
    line-height: 1.9;
    color: #333;
    white-space: pre-wrap;
    word-break: break-word;
    margin: 20px 0;
    padding: 24px 0;
    border-top: 1px solid #eee;
    border-bottom: 1px solid #eee;
}
/* --- Comments --- */
.comments-section { margin-top: 32px; }
.comments-section h3 {
    font-size: 18px;
    margin-bottom: 20px;
    color: #333;
    padding-bottom: 10px;
    border-bottom: 2px solid #4a6cf7;
    display: inline-block;
}
.comment {
    background: #f8f9fb;
    border-radius: 10px;
    padding: 16px 20px;
    margin-bottom: 12px;
    border-left: 3px solid #4a6cf7;
}
.comment .comment-header {
    font-size: 13px;
    color: #888;
    margin-bottom: 6px;
    display: flex;
    justify-content: space-between;
}
.comment .comment-nickname { font-weight: 700; color: #4a6cf7; }
.comment .comment-body { font-size: 15px; color: #444; line-height: 1.6; }
/* --- Empty State --- */
.empty-state {
    text-align: center;
    padding: 60px 20px;
    color: #999;
}
.empty-state .empty-icon { font-size: 56px; margin-bottom: 16px; }
.empty-state p { font-size: 16px; }
/* --- Form Link --- */
.form-link {
    text-align: center;
    margin-top: 16px;
    font-size: 14px;
    color: #777;
}
.form-link a { color: #4a6cf7; text-decoration: none; font-weight: 600; }
.form-link a:hover { text-decoration: underline; }
/* --- Footer --- */
.footer {
    text-align: center;
    padding: 24px;
    color: #999;
    font-size: 13px;
}
/* --- Post Page Title --- */
.post-page-title { font-size: 28px; font-weight: 700; color: #1a1a2e; margin-bottom: 8px; }
</style>)";

// ========== Helper: format timestamp ==========

static std::string format_time(int64_t ts) {
    std::time_t t = static_cast<std::time_t>(ts);
    std::tm* local = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(local, "%Y-%m-%d %H:%M");
    return oss.str();
}

// ========== Render Nav ==========

std::string render_nav(bool is_logged_in) {
    std::ostringstream nav;
    nav << R"(<nav class="navbar">
    <a href="/" class="logo">📝 我的博客</a>
    <div class="nav-links">
        <a href="/">首页</a>)";
    if (is_logged_in) {
        nav << R"(<a href="/publish" class="btn-primary">✏️ 写文章</a>)";
        nav << R"(<a href="/logout">退出登录</a>)";
    } else {
        nav << R"(<a href="/login">登录</a>)";
        nav << R"(<a href="/register">注册</a>)";
    }
    nav << R"(</div></nav>)";
    return nav.str();
}

// ========== Render Page Wrapper ==========

std::string render_page(const std::string& title, const std::string& body_html, bool is_logged_in) {
    std::ostringstream page;
    page << "<!DOCTYPE html>\n<html lang=\"zh-CN\">\n<head>\n"
         << "<meta charset=\"UTF-8\">\n"
         << "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
         << "<title>" << utils::html_escape(title) << " — 我的博客</title>\n"
         << COMMON_CSS << "\n"
         << "</head>\n<body>\n";
    page << render_nav(is_logged_in);
    page << "\n<div class=\"container\">\n" << body_html << "\n</div>\n";
    page << R"(<footer class="footer"><p>&copy; 2026 我的博客 — 基于 C++ 构建 ❤️</p></footer>)";
    page << "\n</body>\n</html>";
    return page.str();
}

// ========== Post List (Homepage) ==========

std::string render_post_list(const std::vector<db::PostMeta>& posts, bool is_logged_in) {
    std::ostringstream body;
    body << R"(<h1 style="font-size:26px;margin-bottom:24px;color:#1a1a2e;">📰 最新文章</h1>)";

    if (posts.empty()) {
        body << R"(<div class="card empty-state">
            <div class="empty-icon">📭</div>
            <p>还没有博客文章。</p>
            <p style="margin-top:8px;">)";
        if (is_logged_in) {
            body << R"(<a href="/publish" class="btn" style="margin-top:12px;display:inline-block;">发表第一篇文章！✨</a>)";
        } else {
            body << R"(<a href="/login">登录</a>后发表第一篇文章！)";
        }
        body << "</p></div>";
    } else {
        for (const auto& post : posts) {
            std::string title = db::buf_to_str(post.title);
            std::string author = db::buf_to_str(post.author);
            std::string summary = db::buf_to_str(post.summary);

            body << "<div class=\"card\">\n"
                 << "<h2><a href=\"/post/" << post.id << "\">" << utils::html_escape(title) << "</a></h2>\n"
                 << "<div class=\"meta\">\n"
                 << "<span>👤 " << utils::html_escape(author) << "</span>\n"
                 << "<span>🕒 " << format_time(post.timestamp) << "</span>\n"
                 << "<span>💬 " << post.comment_count << " 条评论</span>\n"
                 << "</div>\n"
                 << "<div class=\"summary\">" << utils::html_escape(summary) << "</div>\n"
                 << "<a href=\"/post/" << post.id << "\" class=\"read-more\">阅读全文 →</a>\n"
                 << "</div>\n";
        }
    }
    return render_page("最新文章", body.str(), is_logged_in);
}

// ========== Post Detail ==========

std::string render_post_detail(const db::PostMeta& meta, const std::string& content,
                                const std::vector<db::Comment>& comments, bool is_logged_in) {
    std::ostringstream body;

    std::string title = db::buf_to_str(meta.title);
    std::string author = db::buf_to_str(meta.author);

    // Post content
    body << "<div class=\"card\">\n"
         << "<h1 class=\"post-page-title\">" << utils::html_escape(title) << "</h1>\n"
         << "<div class=\"meta\">\n"
         << "<span>👤 " << utils::html_escape(author) << "</span>\n"
         << "<span>🕒 " << format_time(meta.timestamp) << "</span>\n"
         << "<span>💬 " << meta.comment_count << " 条评论</span>\n"
         << "</div>\n"
         << "<div class=\"post-content\">" << utils::html_escape(content) << "</div>\n"
         << "<a href=\"/\" style=\"color:#4a6cf7;text-decoration:none;font-weight:600;\">← 返回首页</a>\n"
         << "</div>\n";

    // Comments section
    body << "<div class=\"card comments-section\">\n"
         << "<h3>💬 评论区</h3>\n";

    if (comments.empty()) {
        body << "<p style=\"color:#999;text-align:center;padding:20px;\">暂无评论，快来发表第一条评论吧！</p>\n";
    } else {
        for (const auto& comment : comments) {
            std::string nickname = db::buf_to_str(comment.nickname);
            std::string comment_content = db::buf_to_str(comment.content);

            body << "<div class=\"comment\">\n"
                 << "<div class=\"comment-header\">\n"
                 << "<span class=\"comment-nickname\">" << utils::html_escape(nickname) << "</span>\n"
                 << "<span>" << format_time(comment.timestamp) << "</span>\n"
                 << "</div>\n"
                 << "<div class=\"comment-body\">" << utils::html_escape(comment_content) << "</div>\n"
                 << "</div>\n";
        }
    }

    // Comment form
    body << "<div style=\"margin-top:24px;padding-top:20px;border-top:1px solid #eee;\">\n"
         << "<h4 style=\"margin-bottom:16px;color:#333;\">✍️ 发表评论</h4>\n"
         << "<form method=\"POST\" action=\"/comment/" << meta.id << "\">\n"
         << "<div class=\"form-group\">\n"
         << "<label>昵称</label>\n"
         << "<input type=\"text\" name=\"nickname\" required maxlength=\"31\" placeholder=\"请输入昵称...\">\n"
         << "</div>\n"
         << "<div class=\"form-group\">\n"
         << "<label>评论内容</label>\n"
         << "<textarea name=\"content\" required maxlength=\"1023\" placeholder=\"请输入评论内容...\" style=\"min-height:100px;\"></textarea>\n"
         << "</div>\n"
         << "<button type=\"submit\" class=\"btn\">提交评论</button>\n"
         << "</form>\n"
         << "</div>\n"
         << "</div>\n";

    return render_page(title, body.str(), is_logged_in);
}

// ========== Login Form ==========

std::string render_login_form(const std::string& error_msg, bool is_logged_in) {
    std::ostringstream body;

    body << "<div class=\"card form-card\">\n"
         << "<h2 style=\"text-align:center;margin-bottom:24px;\">🔐 用户登录</h2>\n";

    if (!error_msg.empty()) {
        body << "<div class=\"alert alert-error\">" << utils::html_escape(error_msg) << "</div>\n";
    }

    body << "<form method=\"POST\" action=\"/login\">\n"
         << "<div class=\"form-group\">\n"
         << "<label>用户名</label>\n"
         << "<input type=\"text\" name=\"username\" required maxlength=\"31\" placeholder=\"请输入用户名\" autocomplete=\"username\">\n"
         << "</div>\n"
         << "<div class=\"form-group\">\n"
         << "<label>密码</label>\n"
         << "<input type=\"password\" name=\"password\" required maxlength=\"64\" placeholder=\"请输入密码\" autocomplete=\"current-password\">\n"
         << "</div>\n"
         << "<button type=\"submit\" class=\"btn btn-block\">登录</button>\n"
         << "</form>\n"
         << "<p class=\"form-link\">还没有账号？<a href=\"/register\">立即注册</a></p>\n"
         << "</div>\n";

    return render_page("登录", body.str(), is_logged_in);
}

// ========== Register Form ==========

std::string render_register_form(const std::unordered_map<std::string, std::string>& field_errors,
                                  bool is_logged_in) {
    std::ostringstream body;

    auto get_error = [&](const std::string& field) -> std::string {
        auto it = field_errors.find(field);
        if (it != field_errors.end() && !it->second.empty()) {
            return "<div class=\"field-error\">" + utils::html_escape(it->second) + "</div>";
        }
        return "";
    };

    body << "<div class=\"card form-card\">\n"
         << "<h2 style=\"text-align:center;margin-bottom:24px;\">📝 创建账号</h2>\n";

    // General error
    if (!get_error("general").empty()) {
        body << "<div class=\"alert alert-error\">" << get_error("general") << "</div>\n";
    }

    body << "<form method=\"POST\" action=\"/register\">\n";

    // Username
    body << "<div class=\"form-group\">\n"
         << "<label>用户名</label>\n"
         << "<input type=\"text\" name=\"username\" required maxlength=\"31\" placeholder=\"3-31位字符，字母/数字/下划线\">\n"
         << get_error("username")
         << "</div>\n";

    // Email
    body << "<div class=\"form-group\">\n"
         << "<label>邮箱</label>\n"
         << "<input type=\"email\" name=\"email\" required maxlength=\"63\" placeholder=\"请输入邮箱地址\">\n"
         << get_error("email")
         << "</div>\n";

    // Password
    body << "<div class=\"form-group\">\n"
         << "<label>密码</label>\n"
         << "<input type=\"password\" name=\"password\" required maxlength=\"64\" placeholder=\"至少6位字符\" autocomplete=\"new-password\">\n"
         << get_error("password")
         << "</div>\n";

    // Confirm Password
    body << "<div class=\"form-group\">\n"
         << "<label>确认密码</label>\n"
         << "<input type=\"password\" name=\"confirm\" required maxlength=\"64\" placeholder=\"请再次输入密码\" autocomplete=\"new-password\">\n"
         << get_error("confirm")
         << "</div>\n";

    body << "<button type=\"submit\" class=\"btn btn-block\">注册</button>\n"
         << "</form>\n"
         << "<p class=\"form-link\">已有账号？<a href=\"/login\">立即登录</a></p>\n"
         << "</div>\n";

    return render_page("注册", body.str(), is_logged_in);
}

// ========== Publish Form ==========

std::string render_publish_form(bool is_logged_in) {
    std::ostringstream body;

    body << "<div class=\"card form-card wide\">\n"
         << "<h2 style=\"text-align:center;margin-bottom:24px;\">✍️ 撰写新文章</h2>\n"
         << "<form method=\"POST\" action=\"/publish\">\n"
         << "<div class=\"form-group\">\n"
         << "<label>文章标题</label>\n"
         << "<input type=\"text\" name=\"title\" required maxlength=\"127\" placeholder=\"请输入文章标题...\">\n"
         << "</div>\n"
         << "<div class=\"form-group\">\n"
         << "<label>文章内容</label>\n"
         << "<textarea name=\"content\" required placeholder=\"请输入文章内容...\" style=\"min-height:300px;\"></textarea>\n"
         << "</div>\n"
         << "<button type=\"submit\" class=\"btn btn-block\">📢 发表文章</button>\n"
         << "</form>\n"
         << "</div>\n";

    return render_page("撰写新文章", body.str(), is_logged_in);
}

// ========== 404 Page ==========

std::string render_404(bool is_logged_in) {
    std::ostringstream body;
    body << "<div class=\"card empty-state\">\n"
         << "<div class=\"empty-icon\">🔍</div>\n"
         << "<h2 style=\"font-size:48px;color:#4a6cf7;margin-bottom:8px;\">404</h2>\n"
         << "<p style=\"font-size:18px;color:#666;\">页面未找到</p>\n"
         << "<p style=\"margin-top:8px;color:#999;\">您访问的页面不存在。</p>\n"
         << "<a href=\"/\" class=\"btn\" style=\"margin-top:20px;display:inline-block;\">🏠 返回首页</a>\n"
         << "</div>\n";
    return render_page("404 页面未找到", body.str(), is_logged_in);
}

} // namespace templates

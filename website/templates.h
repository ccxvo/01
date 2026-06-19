#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include "db.h"

namespace templates {

// ========== Core Layout ==========

// Render the nav bar: "is_logged_in" determines which links are shown
std::string render_nav(bool is_logged_in);

// Wrap body content in a complete HTML page (with embedded CSS)
std::string render_page(const std::string& title, const std::string& body_html, bool is_logged_in);

// ========== Page Renderers ==========

// Homepage: list of blog posts
std::string render_post_list(const std::vector<db::PostMeta>& posts, bool is_logged_in);

// Single post detail page with comments and comment form
std::string render_post_detail(const db::PostMeta& meta, const std::string& content,
                                const std::vector<db::Comment>& comments, bool is_logged_in);

// Login form page (error_msg is empty string on first visit)
std::string render_login_form(const std::string& error_msg, bool is_logged_in);

// Registration form page
// field_errors keys: "username", "email", "password", "confirm", "general"
std::string render_register_form(const std::unordered_map<std::string, std::string>& field_errors,
                                  bool is_logged_in);

// New post (publish) form page
std::string render_publish_form(bool is_logged_in);

// 404 Not Found page
std::string render_404(bool is_logged_in);

} // namespace templates

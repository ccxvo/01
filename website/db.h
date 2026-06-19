#pragma once

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <optional>
#include <mutex>

namespace db {

// ========== Data Model Structs (fixed-size, binary-writable) ==========

struct User {
    int32_t  id;                     // 4 bytes, primary key
    int32_t  deleted;                // 4 bytes, soft-delete flag (0=active, 1=deleted)
    char     username[32];           // 32 bytes, null-terminated, unique
    char     password_hash[128];     // 128 bytes, format: "salt_hex:sha256_hex"
    char     email[64];              // 64 bytes, null-terminated
};

struct PostMeta {
    int32_t  id;                     // 4 bytes
    int32_t  deleted;                // 4 bytes, soft-delete flag
    char     author[32];             // 32 bytes, author username
    char     title[128];             // 128 bytes, post title
    char     summary[256];           // 256 bytes, first ~250 chars of content
    int64_t  timestamp;              // 8 bytes, Unix epoch seconds
    int32_t  comment_count;          // 4 bytes, cached count
};

struct Comment {
    int32_t  id;                     // 4 bytes
    int32_t  post_id;                // 4 bytes, foreign key to PostMeta.id
    int32_t  deleted;                // 4 bytes, soft-delete flag
    char     nickname[32];           // 32 bytes, commenter nickname
    char     content[1024];          // 1024 bytes, comment text
    int64_t  timestamp;              // 8 bytes, Unix epoch seconds
};

// ========== Initialization ==========

// Create data directories and files if they don't exist
void init();

// ========== User CRUD ==========

// Create a new user. Returns new user ID, or -1 on error.
int32_t user_create(const User& user);

// Find user by username. Returns std::nullopt if not found.
std::optional<User> user_find_by_username(const std::string& username);

// Find user by ID. Returns std::nullopt if not found.
std::optional<User> user_find_by_id(int32_t id);

// List all active users
std::vector<User> user_list_all();

// ========== Post CRUD ==========

// Create a new post (meta + content file). Returns new post ID, or -1 on error.
int32_t post_create(const PostMeta& meta, const std::string& content);

// Find post meta by ID
std::optional<PostMeta> post_find_by_id(int32_t id);

// Read post content from text file
std::string post_read_content(int32_t id);

// List all active posts, sorted by timestamp descending
std::vector<PostMeta> post_list_all();

// Increment comment count for a post
void post_increment_comment_count(int32_t id);

// ========== Comment CRUD ==========

// Create a new comment. Returns comment ID, or -1 on error.
int32_t comment_create(const Comment& comment);

// List all active comments for a post, sorted by timestamp ascending
std::vector<Comment> comment_list_by_post(int32_t post_id);

// ========== Helper Functions ==========

// Set a fixed-size char array from a std::string (safe truncation + null terminator)
template<size_t N>
void str_to_buf(char (&buf)[N], const std::string& s) {
    size_t len = s.size();
    if (len >= N) len = N - 1;
    std::memcpy(buf, s.c_str(), len);
    buf[len] = '\0';
}

// Get std::string from fixed-size char array
template<size_t N>
std::string buf_to_str(const char (&buf)[N]) {
    return std::string(buf, strnlen(buf, N));
}

} // namespace db

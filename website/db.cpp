#include "db.h"

#include <cstdio>
#include <cstring>
#include <algorithm>
#include <vector>

#ifdef _WIN32
#include <direct.h>
#endif

namespace db {

// ========== Global state ==========

static std::mutex g_db_mutex;
static const char* DATA_DIR    = "data";
static const char* CONTENT_DIR = "data/content";
static const char* USERS_FILE  = "data/users.bin";
static const char* POSTS_FILE  = "data/posts.bin";
static const char* COMMENTS_FILE = "data/comments.bin";

// ========== Internal helpers (C-style I/O to avoid std::streampos bugs) ==========

// Write raw bytes to a record file at a given ID position
static bool file_write_raw(const char* filename, int32_t id, const void* data, size_t record_size) {
    FILE* f = fopen(filename, "r+b");
    if (!f) {
        // Create file first
        f = fopen(filename, "wb");
        if (!f) return false;
        fclose(f);
        f = fopen(filename, "r+b");
    }
    if (!f) return false;

    long offset = static_cast<long>(id) * static_cast<long>(record_size);
    if (fseek(f, offset, SEEK_SET) != 0) {
        fclose(f);
        return false;
    }
    size_t written = fwrite(data, 1, record_size, f);
    fclose(f);
    return written == record_size;
}

// Read raw bytes from a record file at a given ID position
static bool file_read_raw(const char* filename, int32_t id, void* out_data, size_t record_size) {
    if (id <= 0) return false;

    FILE* f = fopen(filename, "rb");
    if (!f) return false;

    long offset = static_cast<long>(id) * static_cast<long>(record_size);

    // Check file size
    fseek(f, 0, SEEK_END);
    long file_size = ftell(f);
    if (offset >= file_size) {
        fclose(f);
        return false;
    }

    fseek(f, offset, SEEK_SET);
    size_t nread = fread(out_data, 1, record_size, f);
    fclose(f);
    return nread == record_size;
}

// Get the next available ID for a record file
static int32_t file_next_id(const char* filename, size_t record_size) {
    FILE* f = fopen(filename, "rb");
    if (!f) return 1;
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    if (size < static_cast<long>(record_size)) return 1;
    return static_cast<int32_t>(size / static_cast<long>(record_size));
}

// ========== Initialization ==========

void init() {
    std::lock_guard<std::mutex> lock(g_db_mutex);

    // Create directories
    _mkdir(DATA_DIR);
    _mkdir(CONTENT_DIR);

    // Create record files with sentinel record at ID 0 if they don't exist
    auto init_file = [](const char* path, size_t record_size) {
        FILE* f = fopen(path, "rb");
        if (!f) {
            f = fopen(path, "wb");
            if (f) {
                std::vector<char> zero_buf(record_size, 0);
                fwrite(zero_buf.data(), 1, record_size, f);
                fclose(f);
            }
        } else {
            fclose(f);
        }
    };

    init_file(USERS_FILE, sizeof(User));
    init_file(POSTS_FILE, sizeof(PostMeta));
    init_file(COMMENTS_FILE, sizeof(Comment));
}

// ========== User CRUD ==========

int32_t user_create(const User& user) {
    std::lock_guard<std::mutex> lock(g_db_mutex);

    // Check for duplicate username
    auto existing = user_find_by_username(buf_to_str(user.username));
    if (existing.has_value()) {
        return -1;
    }

    int32_t new_id = file_next_id(USERS_FILE, sizeof(User));
    User copy = user;
    copy.id = new_id;
    if (!file_write_raw(USERS_FILE, new_id, &copy, sizeof(User))) {
        return -1;
    }
    return new_id;
}

std::optional<User> user_find_by_username(const std::string& username) {
    // Caller should hold g_db_mutex if called from user_create
    FILE* f = fopen(USERS_FILE, "rb");
    if (!f) return std::nullopt;

    User record;
    while (fread(&record, sizeof(User), 1, f) == 1) {
        if (record.id <= 0 || record.deleted != 0) continue;
        if (buf_to_str(record.username) == username) {
            fclose(f);
            return record;
        }
    }
    fclose(f);
    return std::nullopt;
}

std::optional<User> user_find_by_id(int32_t id) {
    std::lock_guard<std::mutex> lock(g_db_mutex);
    User user;
    if (file_read_raw(USERS_FILE, id, &user, sizeof(User))) {
        if (user.deleted == 0 && user.id > 0) return user;
    }
    return std::nullopt;
}

std::vector<User> user_list_all() {
    std::lock_guard<std::mutex> lock(g_db_mutex);
    std::vector<User> result;
    FILE* f = fopen(USERS_FILE, "rb");
    if (!f) return result;

    User record;
    while (fread(&record, sizeof(User), 1, f) == 1) {
        if (record.id > 0 && record.deleted == 0) {
            result.push_back(record);
        }
    }
    fclose(f);
    return result;
}

// ========== Post CRUD ==========

int32_t post_create(const PostMeta& meta, const std::string& content) {
    std::lock_guard<std::mutex> lock(g_db_mutex);

    int32_t new_id = file_next_id(POSTS_FILE, sizeof(PostMeta));
    PostMeta copy = meta;
    copy.id = new_id;
    if (!file_write_raw(POSTS_FILE, new_id, &copy, sizeof(PostMeta))) {
        return -1;
    }

    // Write content to text file (use C++ fstream for text is fine here)
    std::string content_path = std::string(CONTENT_DIR) + "/post_" + std::to_string(new_id) + ".txt";
    FILE* cf = fopen(content_path.c_str(), "w");
    if (!cf) return -1;
    fwrite(content.c_str(), 1, content.size(), cf);
    fclose(cf);

    return new_id;
}

std::optional<PostMeta> post_find_by_id(int32_t id) {
    std::lock_guard<std::mutex> lock(g_db_mutex);
    PostMeta meta;
    if (file_read_raw(POSTS_FILE, id, &meta, sizeof(PostMeta))) {
        if (meta.deleted == 0 && meta.id > 0) return meta;
    }
    return std::nullopt;
}

std::string post_read_content(int32_t id) {
    std::lock_guard<std::mutex> lock(g_db_mutex);
    std::string content_path = std::string(CONTENT_DIR) + "/post_" + std::to_string(id) + ".txt";
    FILE* f = fopen(content_path.c_str(), "r");
    if (!f) return "";
    std::string content;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0) {
        content.append(buf, n);
    }
    fclose(f);
    return content;
}

std::vector<PostMeta> post_list_all() {
    std::lock_guard<std::mutex> lock(g_db_mutex);
    std::vector<PostMeta> result;
    FILE* f = fopen(POSTS_FILE, "rb");
    if (!f) return result;

    PostMeta record;
    while (fread(&record, sizeof(PostMeta), 1, f) == 1) {
        if (record.id > 0 && record.deleted == 0) {
            result.push_back(record);
        }
    }
    fclose(f);

    std::sort(result.begin(), result.end(), [](const PostMeta& a, const PostMeta& b) {
        return a.timestamp > b.timestamp;
    });

    return result;
}

void post_increment_comment_count(int32_t id) {
    std::lock_guard<std::mutex> lock(g_db_mutex);
    PostMeta meta;
    if (file_read_raw(POSTS_FILE, id, &meta, sizeof(PostMeta))) {
        meta.comment_count++;
        file_write_raw(POSTS_FILE, id, &meta, sizeof(PostMeta));
    }
}

// ========== Comment CRUD ==========

int32_t comment_create(const Comment& comment) {
    std::lock_guard<std::mutex> lock(g_db_mutex);

    int32_t new_id = file_next_id(COMMENTS_FILE, sizeof(Comment));
    Comment copy = comment;
    copy.id = new_id;
    if (!file_write_raw(COMMENTS_FILE, new_id, &copy, sizeof(Comment))) {
        return -1;
    }
    return new_id;
}

std::vector<Comment> comment_list_by_post(int32_t post_id) {
    std::lock_guard<std::mutex> lock(g_db_mutex);
    std::vector<Comment> result;
    FILE* f = fopen(COMMENTS_FILE, "rb");
    if (!f) return result;

    Comment record;
    while (fread(&record, sizeof(Comment), 1, f) == 1) {
        if (record.id > 0 && record.deleted == 0 && record.post_id == post_id) {
            result.push_back(record);
        }
    }
    fclose(f);

    std::sort(result.begin(), result.end(), [](const Comment& a, const Comment& b) {
        return a.timestamp < b.timestamp;
    });

    return result;
}

} // namespace db

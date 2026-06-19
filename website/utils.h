#pragma once

#include <string>
#include <cstdint>

namespace utils {

// SHA-256 hash: returns 64-character lowercase hex string
std::string sha256(const std::string& input);

// HTML entity escape: replaces < > & " ' with entities
std::string html_escape(const std::string& input);

// URL-decode: %20 -> space, %40 -> @, etc.
std::string url_decode(const std::string& input);

// Current Unix timestamp in seconds
int64_t current_timestamp();

// Generate a random hex string of given length (uses std::random_device)
std::string random_hex(int length);

// Simple email format validation
bool is_valid_email(const std::string& email);

// Simple username validation: 3-31 chars, alphanumeric + underscore
bool is_valid_username(const std::string& username);

} // namespace utils

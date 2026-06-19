#include "utils.h"

#include <cstring>
#include <ctime>
#include <random>
#include <sstream>
#include <iomanip>
#include <chrono>

namespace utils {

// ========== SHA-256 Implementation ==========
// Compact, self-contained SHA-256 (public-domain style)

namespace {

constexpr uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

inline uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

} // anonymous namespace

std::string sha256(const std::string& input) {
    // Initial hash values
    uint32_t h[8] = {
        0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
        0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
    };

    // Pre-processing: padding
    uint64_t bit_len = input.size() * 8;
    std::vector<uint8_t> msg(input.begin(), input.end());

    // Append '1' bit (0x80 byte)
    msg.push_back(0x80);

    // Pad with zeros to 56 mod 64 bytes
    while ((msg.size() % 64) != 56) {
        msg.push_back(0x00);
    }

    // Append original length in bits as big-endian 64-bit integer
    for (int i = 7; i >= 0; --i) {
        msg.push_back(static_cast<uint8_t>((bit_len >> (i * 8)) & 0xFF));
    }

    // Process each 512-bit (64-byte) block
    for (size_t offset = 0; offset < msg.size(); offset += 64) {
        uint32_t w[64];

        // Prepare message schedule
        for (int i = 0; i < 16; ++i) {
            w[i] = (static_cast<uint32_t>(msg[offset + i * 4]) << 24) |
                   (static_cast<uint32_t>(msg[offset + i * 4 + 1]) << 16) |
                   (static_cast<uint32_t>(msg[offset + i * 4 + 2]) << 8) |
                   (static_cast<uint32_t>(msg[offset + i * 4 + 3]));
        }
        for (int i = 16; i < 64; ++i) {
            uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        // Initialize working variables
        uint32_t a = h[0], b = h[1], c = h[2], d = h[3];
        uint32_t e = h[4], f = h[5], g = h[6], hh = h[7];

        // Compression function
        for (int i = 0; i < 64; ++i) {
            uint32_t S1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = hh + S1 + ch + K[i] + w[i];
            uint32_t S0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = S0 + maj;

            hh = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        // Update hash values
        h[0] += a; h[1] += b; h[2] += c; h[3] += d;
        h[4] += e; h[5] += f; h[6] += g; h[7] += hh;
    }

    // Produce final hex string
    std::ostringstream oss;
    for (int i = 0; i < 8; ++i) {
        oss << std::hex << std::setfill('0') << std::setw(8) << h[i];
    }
    return oss.str();
}

// ========== HTML Escape ==========

std::string html_escape(const std::string& input) {
    std::string result;
    result.reserve(input.size() * 1.2);
    for (char c : input) {
        switch (c) {
            case '<':  result += "&lt;";   break;
            case '>':  result += "&gt;";   break;
            case '&':  result += "&amp;";  break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&#39;";  break;
            default:   result += c;        break;
        }
    }
    return result;
}

// ========== URL Decode ==========

std::string url_decode(const std::string& input) {
    std::string result;
    result.reserve(input.size());
    for (size_t i = 0; i < input.size(); ++i) {
        if (input[i] == '%' && i + 2 < input.size()) {
            char hex[3] = {input[i + 1], input[i + 2], '\0'};
            char* end;
            long val = std::strtol(hex, &end, 16);
            if (end == hex + 2) {
                result += static_cast<char>(val);
                i += 2;
                continue;
            }
        } else if (input[i] == '+') {
            result += ' ';
            continue;
        }
        result += input[i];
    }
    return result;
}

// ========== Current Timestamp ==========

int64_t current_timestamp() {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(
        now.time_since_epoch()
    ).count();
}

// ========== Random Hex String ==========

std::string random_hex(int length) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dist(0, 15);

    const char hex_chars[] = "0123456789abcdef";
    std::string result;
    result.reserve(length);
    for (int i = 0; i < length; ++i) {
        result += hex_chars[dist(gen)];
    }
    return result;
}

// ========== Email Validation ==========

bool is_valid_email(const std::string& email) {
    // Must contain exactly one '@'
    size_t at_pos = email.find('@');
    if (at_pos == std::string::npos || at_pos == 0 || at_pos == email.size() - 1) {
        return false;
    }
    // Check only one '@'
    if (email.find('@', at_pos + 1) != std::string::npos) {
        return false;
    }
    // Must have at least one '.' after '@'
    std::string domain = email.substr(at_pos + 1);
    if (domain.find('.') == std::string::npos) {
        return false;
    }
    // Domain part before last '.' must not be empty
    size_t last_dot = domain.rfind('.');
    if (last_dot == 0 || last_dot == domain.size() - 1) {
        return false;
    }
    // Check for invalid characters
    for (char c : email) {
        if (c <= 32 || c >= 127) return false;
    }
    return email.size() <= 63;
}

// ========== Username Validation ==========

bool is_valid_username(const std::string& username) {
    if (username.size() < 3 || username.size() > 31) {
        return false;
    }
    for (char c : username) {
        if (!((c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') ||
              c == '_')) {
            return false;
        }
    }
    return true;
}

} // namespace utils

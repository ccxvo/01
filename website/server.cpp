#include "server.h"
#include "http.h"
#include "router.h"

#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <vector>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#endif
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#define SOCKET int
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define closesocket close
#endif

namespace server {

// ========== Helper: send all data ==========

static void send_all(SOCKET sock, const std::string& data) {
    int total_sent = 0;
    int remaining = static_cast<int>(data.size());
    const char* buf = data.c_str();

    while (remaining > 0) {
        int sent = send(sock, buf + total_sent, remaining, 0);
        if (sent <= 0) {
            break;
        }
        total_sent += sent;
        remaining -= sent;
    }
}

// ========== Helper: receive HTTP request ==========

static std::string recv_request(SOCKET sock) {
    char buffer[65536];
    std::string result;

    // First recv
    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) return "";

    buffer[bytes] = '\0';
    result.assign(buffer, bytes);

    // Check if we got the full request
    // Simple approach: check for \r\n\r\n
    size_t header_end = result.find("\r\n\r\n");
    if (header_end == std::string::npos) {
        // Headers not complete - unlikely for simple requests, but handle it
        return result;
    }

    // Try to parse Content-Length to see if we need more body data
    std::string headers = result.substr(0, header_end);
    size_t cl_pos = headers.find("Content-Length:");
    if (cl_pos == std::string::npos) {
        cl_pos = headers.find("content-length:");
    }

    if (cl_pos != std::string::npos) {
        size_t cl_end = headers.find("\r\n", cl_pos);
        std::string cl_str = headers.substr(cl_pos + 15, cl_end - (cl_pos + 15));
        // Trim
        while (!cl_str.empty() && cl_str[0] == ' ') cl_str.erase(0, 1);
        int content_length = 0;
        try {
            content_length = std::stoi(cl_str);
        } catch (...) {
            content_length = 0;
        }

        size_t body_start = header_end + 4; // \r\n\r\n
        int body_received = static_cast<int>(result.size() - body_start);
        int remaining = content_length - body_received;

        // Read remaining body bytes
        while (remaining > 0) {
            bytes = recv(sock, buffer, std::min((int)sizeof(buffer) - 1, remaining), 0);
            if (bytes <= 0) break;
            buffer[bytes] = '\0';
            result.append(buffer, bytes);
            remaining -= bytes;
        }
    }

    return result;
}

// ========== Handle a single client connection ==========

static void handle_client(SOCKET client_socket, const std::string& client_ip) {
    // Receive the HTTP request
    std::string raw_request = recv_request(client_socket);

    if (raw_request.empty()) {
        closesocket(client_socket);
        return;
    }

    // Parse request
    http::HttpRequest req;
    try {
        req = http::parse_http_request(raw_request);
    } catch (const std::exception& e) {
        std::string response = http::build_response(400, "text/html; charset=utf-8",
            "<!DOCTYPE html><html><head><title>400 请求错误</title></head>"
            "<body><h1>400 请求错误</h1><p>无法解析该请求。</p></body></html>");
        send_all(client_socket, response);
        closesocket(client_socket);
        return;
    }

    // Log the request
    std::cout << "[" << client_ip << "] " << req.method << " " << req.path << std::endl;

    // Dispatch to handler
    std::string response = router::dispatch(req);

    // Send response
    send_all(client_socket, response);

    // Close connection
    closesocket(client_socket);
}

// ========== Main server run ==========

int run(int port) {
#ifdef _WIN32
    // Initialize Winsock
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        std::cerr << "[错误] WSAStartup 初始化失败。" << std::endl;
        return 1;
    }
#endif

    // Create socket
    SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == INVALID_SOCKET) {
        std::cerr << "[错误] 创建 Socket 失败。" << std::endl;
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Set SO_REUSEADDR to avoid "address already in use" on restart
    int opt = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&opt), sizeof(opt));

    // Bind
    sockaddr_in server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(static_cast<uint16_t>(port));

    if (bind(listen_socket, reinterpret_cast<sockaddr*>(&server_addr),
             sizeof(server_addr)) == SOCKET_ERROR) {
        std::cerr << "[错误] 绑定端口 " << port << " 失败。" << std::endl;
        closesocket(listen_socket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    // Listen
    if (listen(listen_socket, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "[错误] 监听失败。" << std::endl;
        closesocket(listen_socket);
#ifdef _WIN32
        WSACleanup();
#endif
        return 1;
    }

    std::cout << "========================================" << std::endl;
    std::cout << "  📝 博客服务器" << std::endl;
    std::cout << "  正在监听 http://localhost:" << port << std::endl;
    std::cout << "  按 Ctrl+C 停止服务器" << std::endl;
    std::cout << "========================================" << std::endl;

    // Accept loop
    while (true) {
        sockaddr_in client_addr = {};
        socklen_t client_len = sizeof(client_addr);

        SOCKET client_socket = accept(listen_socket,
                                       reinterpret_cast<sockaddr*>(&client_addr),
                                       &client_len);

        if (client_socket == INVALID_SOCKET) {
            std::cerr << "[警告] accept() 失败，继续运行..." << std::endl;
            continue;
        }

        // Get client IP for logging
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_str, sizeof(ip_str));

        // Spawn thread to handle this client
        std::thread(handle_client, client_socket, std::string(ip_str)).detach();
    }

    // Cleanup (unreachable in normal operation)
    closesocket(listen_socket);
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}

} // namespace server

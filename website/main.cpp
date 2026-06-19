#include "server.h"
#include "db.h"
#include "router.h"

#include <iostream>

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "  C++ 博客网站" << std::endl;
    std::cout << "========================================" << std::endl;

    std::cout << "正在初始化数据库..." << std::endl;
    db::init();

    std::cout << "正在注册路由..." << std::endl;
    router::init_routes();

    std::cout << "正在启动服务器..." << std::endl;
    return server::run(8080);
}

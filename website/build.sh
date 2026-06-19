#!/bin/bash
# 构建 C++ 博客网站
# 用法: cd website && bash build.sh

echo "正在构建 C++ 博客网站..."
g++ -std=c++20 -Wall -O2 -o blog.exe *.cpp -lws2_32

if [ $? -eq 0 ]; then
    echo ""
    echo "=========================================="
    echo "  构建成功: blog.exe"
    echo ""
    echo "  运行: ./blog.exe"
    echo "  访问: http://localhost:8080"
    echo "=========================================="
else
    echo ""
    echo "=========================================="
    echo "  构建失败！"
    echo "=========================================="
    exit 1
fi

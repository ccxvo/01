#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <windows.h>
using namespace std;
struct Student {
    int id;
    string name;
    double scores[5];
    double average;
};

int main() {
        SetConsoleOutputCP(65001); // 设置输出编码为 UTF-8
    SetConsoleCP(65001); 
    Student students[4];
    cout << "请输入4名学生的信息：" << endl;
    for (int i = 0; i < 4; i++) {
        cout << "\n请输入第 " << i + 1 << " 个学生的信息：" << endl;
        cout << "学号: ";
        cin >> students[i].id;
        cout << "姓名: ";
        cin >> students[i].name;
        double sum = 0;
        cout << "请输入5门课成绩 (用空格分隔): ";
        for (int j = 0; j < 5; j++) {
            cin >> students[i].scores[j];
            sum += students[i].scores[j];
        }
        students[i].average = sum / 5.0;
    cout << "平均分为: " << students[i].average << endl;
    }
    ofstream outFile("StudentGrade.txt");
    if (!outFile.is_open()) {
        cerr << "错误：无法创建或打开文件 StudentGrade.txt" << endl;
        return 1;
    }
    for (int i = 0; i < 4; i++) {
        outFile << students[i].id << ";" << students[i].name << ";";
        for (int j = 0; j < 5; j++) {
            outFile << fixed << setprecision(1) << students[i].scores[j];
            if (j < 4) outFile << ";";
        }
        outFile << ";" << fixed << setprecision(2) << students[i].average << endl;
    }
    outFile.close();
    cout << "\n所有数据已成功保存到 StudentGrade.txt 中！" << endl;
    return 0;
}
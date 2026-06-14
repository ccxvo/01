#include <iostream>
#include <cstdio>
#include <cstring>
using namespace std;

struct StudentGrade {
    int id;
    char name[64];
    int scores[5];
    double average;
};

int main() {
    system("chcp 65001 > nul");

    int n;
    cout << "请输入学生数量：";
    cin >> n;
    if(n <= 0) {
        cerr << "学生数量必须大于0。" << endl;
        return 1;
    }
    if(n > 100) {
        cerr << "最大支持100名学生。" << endl;
        return 1;
    }

    StudentGrade *students = new StudentGrade[n];

    for(int i = 0; i < n; i++) {
        cout << "\n请输入第" << i + 1 << "名学生的学号：";
        cin >> students[i].id;
        cout << "请输入第" << i + 1 << "名学生的姓名：";
        cin >> students[i].name;
        int sum = 0;
        for(int j = 0; j < 5; j++) {
            cout << "请输入第" << i + 1 << "名学生第" << j + 1 << "门成绩：";
            cin >> students[i].scores[j];
            sum += students[i].scores[j];
        }
        students[i].average = static_cast<double>(sum) / 5.0;
    }

    FILE *fp = fopen("StudentGrade.dat", "wb");
    if(!fp) {
        cerr << "无法创建二进制文件 StudentGrade.dat" << endl;
        delete[] students;
        return 1;
    }

    for(int i = 0; i < n; i++) {
        fwrite(&students[i], sizeof(StudentGrade), 1, fp);
    }

    fclose(fp);
    delete[] students;
    cout << "已将 " << n << " 名学生的全部数据保存到 StudentGrade.dat" << endl;
    return 0;
}

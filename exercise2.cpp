#include<iostream>
#include<string>
#include<iomanip>
#include<cstdlib>
#include<fstream>
#include<vector>
#include<algorithm>

using namespace std;

static double extractAverage(const string &line) {
    const char delimiter = '；';
    int count = 0;
    size_t pos = string::npos;
    for(size_t i = 0; i < line.size(); i++) {
        if(line[i] == delimiter) {
            count++;
            if(count == 7) {
                pos = i;
                break;
            }
        }
    }
    if(pos == string::npos || pos + 1 >= line.size()) {
        return 0.0;
    }
    string avgStr = line.substr(pos + 1);
    try {
        return stod(avgStr);
    } catch(...) {
        return 0.0;
    }
}

int main() {
    system("chcp 65001 > nul");

    vector<string> records;
    ifstream ifs("StudentGrade.txt");
    if(!ifs) {
        cerr << "无法打开文件 StudentGrade.txt" << endl;
        return 1;
    }

    string line;
    while(getline(ifs, line)) {
        if(!line.empty()) {
            records.push_back(line);
        }
    }
    ifs.close();

    sort(records.begin(), records.end(), [](const string &a, const string &b) {
        return extractAverage(a) > extractAverage(b);
    });

    ofstream ofs("SortedGrade.txt", ios::out);
    if(!ofs) {
        cerr << "无法创建文件 SortedGrade.txt" << endl;
        return 1;
    }

    for(const auto &record : records) {
        ofs << record << endl;
    }
    ofs.close();

    cout << "排序完成，结果已保存到 SortedGrade.txt" << endl;
    return 0;
}

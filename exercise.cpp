#include<iostream>
using namespace std;
#include<string>
#include<iomanip>
#include<cstdlib>
#include<fstream>
#include<sstream>

class Student{
private:    
string m_name;
    int m_id;
    int grades[5];
    double m_average;

    public:
    
       void input(){
        cout<<"请输入学生姓名："<<endl;
        cin>>m_name;
        this->m_name;
        cout<<"请输入学生学号："<<endl;
        cin>>m_id;
        this->m_id;
      
        int sum=0;
        for(int i=0;i<5;i++){
            cout<<"请输入第"<<i+1<<"门课程成绩："<<endl;
            cin>>this->grades[i];
            sum+=this->grades[i];
        }
        this->m_average=(double)sum/5;}

      string toString() const {
        ostringstream oss;
        oss << m_name << "；" << m_id;
        for(int i = 0; i < 5; i++) {
            oss << "；" << grades[i];
        }
        oss << "；" << fixed << setprecision(2) << m_average;
        return oss.str();
      }

      void write(){
        ofstream ofs("StudentGrade.txt", ios::app);
        if(!ofs) {
            cerr << "无法打开文件 StudentGrade.txt" << endl;
            return;
        }
        ofs << toString() << endl;

        ofs.close();
      }

   
};
int main(){
    system("chcp 65001 > nul");

    for(int i = 0; i < 4; i++){
      cout << "输入第" << i+1 << "位学生信息" << endl;
      Student s;
      s.input();
      s.write();
    }

  return 0;
}

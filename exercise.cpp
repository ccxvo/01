#include<iostream>
using namespace std;
#include<fstream>
#include<string>


void test01(){

ifstream ifs;
ifs.open("text.txt",ios::in);
if(!ifs.is_open()){
    cout<<"文件打开失败"<<endl;
    return;
}

string buf;
while ((getline(ifs,buf)))
{
    cout<<buf<<endl;
}


ifs.close();
}


int main(){

test01();

    return 0;
}

#include<iostream>
using namespace std;
#include<fstream>

void test01(){

ofstream ofs;
ofs.open("text.txt",ios::out);
ofs<<"Hello World";
ofs.close();

}


int main(){

test01();

    return 0;
}

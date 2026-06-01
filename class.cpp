#include<iostream>
#include<string>
using namespace std;

class base{
    public:
    base(){
        m_a=100;
    }
    void func(){
        cout<<"base func"<<endl;
    }
    void func(int a){
cout<<"base func(int a)"<<endl;
    }
    int m_a;
};

class son:protected base{
    friend void test01();
public:
    son(){
    m_a=200;
}
void func(){
    cout<<"son func"<<endl;

}

int m_a;
};

void test01(){
  son s;
  cout<<s.m_a<<endl;
  cout<<s.base::m_a<<endl;

  s.func();
  s.base::func(100);
  s.base::func();
    
}

int main()
{
    test01();
return 0;
}
#include <iostream>
#include <string>
using namespace std;



class Teacher {
public:
    Teacher(int n, const string &na, char s, int a, const string &t, const string &ad)
        : num(n), name(na), sex(s), age(a), title(t), addr(ad) {}

    void display() const {
        cout << "Num: " << num << endl;
        cout << "Name: " << name << endl;
        cout << "Sex: " << sex << endl;
        cout << "Age: " << age << endl;
        cout << "Title: " << title << endl;
        cout << "Addr: " << addr << endl;
    }

protected:
    int num;
    string name;
    char sex;
    int age;
    string title;
    string addr;
};

class Cadre {
public:
    Cadre(int n, const string &na, char s, int a, const string &p, const string &ad)
        : num(n), name(na), sex(s), age(a), post(p), addr(ad) {}

protected:
    int num;
    string name;
    char sex;
    int age;
    string post;
    string addr;
};

class Teacher_Cadre : public Teacher, public Cadre {
public:
    Teacher_Cadre(int n, const string &na, char s, int a, const string &t, const string &p, const string &ad, float w)
        : Teacher(n, na, s, a, t, ad), Cadre(n, na, s, a, p, ad), wages(w) {}

  
    void set_wages(float w) {
        wages = w;
    }

    void show() const {
        Teacher::display(); 
        cout << "Post: " << Cadre::post << endl;
        cout << "Wages: " << wages << endl;
    }

private:
    float wages;
};



int main() {
    Teacher_Cadre prof1(101, "Zhang San", 'm', 40, "Associate Prof", "Director", "Beijing Road 101", 5000.0);

    cout << ">>> Original Data:" << endl;
    prof1.show();

    cout << "\n>>> Modifying wages..." << endl;
    prof1.set_wages(8800.0);

    cout << "\n>>> Updated Data:" << endl;
    prof1.show();

    return 0;
}
#include <iostream>
using namespace std;

int fn(int n,int x){
if(n==0) return 1;
else if (n==1) return 2*x;
else  return 2*x*fn(n-1,x)-2*(n-1)*fn(n-1,x);
}

int gcd(int a,int b){
	while(b!=0){
		int temp=b;
		b=a%b;
		a=temp;
	}
	

}

int main(){
	
cout<<fn(3,2)<<endl;
return 1;

}
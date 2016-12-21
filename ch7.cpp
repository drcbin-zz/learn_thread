#include<iostream>
#include<thread>
using namespace std;

int add(int a, int b){
    cout << "results is : " << a + a << endl;
    return a+a;
}

int main(){
    int a = 2;
    thread t1(add, 4, ref(a));    //这个4 是传进去作为add函数的参数,此处没有发生复制,因此可以执行.
    t1.join();
    return 0;
}

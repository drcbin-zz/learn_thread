#include<iostream>
#include<thread>
#include<future>
using namespace std;

int add(int a, shared_future<int> f){
    int b = f.get();
    cout << "results is : " << a + b << endl;
    return a+b;
}

int main(){
    int a;
    std::promise<int> p;
    future<int> f = p.get_future();
    shared_future<int> fs = f.share();
    future<int> fu = std::async(add, 4, fs);
    future<int> fu1 = std::async(add, 4,fs);
    future<int> fu2 = std::async(add, 4,fs);
    future<int> fu3 = std::async(add, 4,fs);
    a = 20;
    p.set_value(a);
    cout << fu.get() << endl;    //fu.get()函数只能被调用一次,如果调用多次,程序崩溃.
    return 0;
}

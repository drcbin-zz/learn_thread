#include<iostream>
#include<thread>
#include<future>
using namespace std;

int add(int a, int& b){
    cout << "results is : " << a + b << endl;
    this_thread::get_id();
    return a+b;
}

int main(){
    int a;
    std::promise<int> p;
    future<int> f = p.get_future();
    future<int> fu = std::async(add, 4, ref(f));
    p.set_value(20);
    cout << fu.get() << endl;    //fu.get()函数只能被调用一次,如果调用多次,程序崩溃.
    return 0;
}

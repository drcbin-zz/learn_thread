#include<thread>
#include<iostream>
using namespace std;
void fun(){
    cout << "Hello" << endl;
}
int main(){
    thread t1(&fun);
    t1.join();
    return 0;
}

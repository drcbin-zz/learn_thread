#include<iostream>
#include<thread>
#include<mutex>
#include <condition_variable>
#include<deque>
using namespace std;

int data = 0;
deque<int> q;
mutex mu;
condition_variable con;

void fun1(){
    for (int i = 0; i < 10; ++i) {
        unique_lock<mutex> locker(mu);
        cout << "data " << data << " is pushing" << endl;
        q.push_front(data);
        cout  << "data pushed: " << data << "    ---" << this_thread::get_id() <<  endl;
        data++;
        cout  << "data added: " << data << "    ---" << this_thread::get_id() <<  endl << endl << endl << endl;
        con.notify_one();
        locker.unlock();
        this_thread::sleep_for(chrono::milliseconds(2000));
    }
}

void fun2(){
    for(int i = 0; i < 10; i++){
        unique_lock<mutex> locker(mu);
        con.wait(locker, []{return !q.empty();});
        cout << "data " << q.back() << " is poping" << endl;
        int temp = q.back();
        q.pop_back();
        cout << "data poped: " << temp << "    -----" << this_thread::get_id() <<  endl << endl << endl << endl;
        locker.unlock();
        this_thread::sleep_for(chrono::milliseconds(200));
    }
}
int main()
{
    thread t1(fun1);
    thread t2(fun1);
    thread t3(fun1);
    thread t4(fun1);
    thread t5(fun1);
    thread t6(fun1);
    thread t7(fun1);
    thread t8(fun1);
    thread t9(fun1);
    thread t10(fun1);
    thread t11(fun2);
    thread t12(fun2);
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
    t9.join();
    t10.join();
    t11.join();
    t12.join();
    this_thread::sleep_for(chrono::seconds(2));
    cout << "main is running" << endl;
    return 0;
}

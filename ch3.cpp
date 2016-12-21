#include <fstream>
#include<mutex>
#include<iostream>
#include<thread>
using namespace std;
// mutex mu;
// void share_print(string msg, int id){
    // // mu.lock();
    // lock_guard<mutex> guard(mu);    //构造lock_guard 对象，并将锁工具 mu 作为参数传递进去，这样 mu这个锁工具就由 guard控制，当guard销毁的时候会自动解锁
    // cout << msg << id << endl;
    // mu.unlock();
// }

#ifndef CH3_H
#define CH3_H

class LockFile    //确保同时只有一个线程能操作这个文件
{
private:
    ofstream f;
    mutex m_mu, m_mu2;

public:
    LockFile(){
        f.open("log.txt");
    }
    void share_print(string id, int vaue){
        // this_thread::sleep_for(chrono::microseconds(10));
        lock(m_mu, m_mu2);    //std::lock()
        lock_guard<mutex> guard(m_mu, adopt_lock);
        lock_guard<mutex> guard2(m_mu2, adopt_lock);
        cout << "from" << id << ":" << vaue << endl; 
    }
    void share_print2(string id, int vaue){
        // this_thread::sleep_for(chrono::microseconds(10));
        lock(m_mu, m_mu2);    //std::lock()
        lock_guard<mutex> guard2(m_mu2, adopt_lock);
        lock_guard<mutex> guard(m_mu, adopt_lock);
        cout << "from" << id << ":" << vaue << endl; 
    }
};

#endif /* CH3_H */

void function_1(LockFile& lockfile){
    for (int i = 0; i < 1000; ++i) {
        lockfile.share_print("t1", i);
    }
}
int main(int argc, char *argv[])
{
    LockFile lockfile;
    thread t1(function_1, ref(lockfile));
    for(int i = 0; i > -1000; i--){
        lockfile.share_print2("main", i);
    }
    t1.join();
    return 0;
}




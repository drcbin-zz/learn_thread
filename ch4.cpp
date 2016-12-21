#include <fstream>
#include<mutex>
#include<iostream>
#include<thread>
using namespace std;
class LockFile    //确保同时只有一个线程能操作这个文件
{
private:
    ofstream f;
    mutex m_mu;
    once_flag m_flag;
public:
    LockFile(){
    }
    void operator()(){
        f.open("log.txt");
    }
    void share_print(string id, int vaue){
        call_once(m_flag, [&]{f.open("log.txt");});
        unique_lock<mutex> locker(m_mu);
        cout << "from " << id << ":" << vaue << endl; 
    }
};


void function_1(LockFile& lockfile){
    for (int i = 0; i < 1000; ++i) {
        lockfile.share_print("t1", i);
    }
}
int main()
{
    LockFile lockfile;
    thread t1(function_1, ref(lockfile));
    for(int i = 0; i > -1000; i--){
        lockfile.share_print("main", i);
    }
    t1.join();
    return 0;
}

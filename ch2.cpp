#include<iostream>
#include <string>
#include <thread>
void function_1(){
    std::cout << "Hello,Thread" << std::endl;
}

#ifndef CH2_H
#define CH2_H

class Fctor
{
public:
    void operator()(std::string args){
        for(int i = 0;i > -10; i--){
            std::cout << "ft1" << args << std::endl;
        }
        std::cout << "id of t2 is:" << std::this_thread::get_id() << std::endl;
        args = "i don't love you";
    }
};

#endif /* CH2_H */
int main()
{
    std::string s = "i love you";

    // Fctor ft1;
    // std::thread t1(ft1, s);
    
    std::thread t1((Fctor()), std::move(s)); //也可以通过这种类似于java中匿名对象的方法构造线程，两种等价

    std::thread t2 = std::move(t1);

    // std::thread t1(function_1);
    try{
        for(int i = 0; i < 10; i++){
            //如果在for循环内出现异常，那么主线程将被终止，所以这样的线程是不安全的
            std::cout << "for main" << i << std::endl;
        }
    }catch(...){
        std::cout << "exception catched" << std::endl;
        t2.join();
        throw;
    }
    t2.join();
    std::cout << "the id of main is:" << std::this_thread::get_id() << std::endl;
    std::cout << "max threads:" << std::thread::hardware_concurrency() << std::endl;
    return 0;
}

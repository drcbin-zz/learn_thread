#include<iostream>
#include <thread>
void function_1(){
    std::cout << "Hello,Thread" << std::endl;
}

int main(int argc, char *argv[])
{
    std::thread t1(function_1);
    t1.join();
    return 0;
}

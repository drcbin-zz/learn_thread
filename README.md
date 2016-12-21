## c++ 多线程学习笔记 ##
#### ch 1   课程简介 #####
> - 包含的头文件 'thread'
> > linux 下 g++ 编译的时候加上参数 -lpthear,否则会报 undefined reference to `pthread_create' 错误

> - 线程创建的前提:给线程对象传递一个[可调用对象](http://blog.csdn.net/lc_910927/article/details/21250111)作为参数
> - 创建线程的方法：thread t1(functionName);  //functionName为待执行的函数
> - 启动线程的方法:
	1. t1.join()    //主线程会等待t1线程执行完毕后才会退出。
	2. t1.detach();    //主线程执行完毕就会退出，可能导致t1线程没有执行完成就退出了
> - 任何线程在 detach() 之后就不能在 join()了。

 	void fun(){ do something;}
	int main(){
		thread t1(fun);
		t1.detach();
		t1.join();    //禁止会报错。
		return 0;
	} 


> - 线程在join() 之前可以判断是不是可以被 join(), 使用 joinable() 方法,来保证程序的正常.
	
	int main(){
		thread t 1(fun);
		t1.detach();
		if(t1.joinable()){
			t1.join();
		}
		return 0;
	}
	
****

====================
	
#### ch2 线程管理 ####
----
> - 主线程可以和子线程同时执行业务逻辑，eg:

	int main(){
		std::thread t1(fun);
		for(int i = 0; i < 10; i++){
			//如果发生异常，则主程序中断，子程序也会中断
			std::cout << "in main thread" << std::endl;
		}
		t1.join();    //不安全线程
	}

> 但就像上面的代码，如果主程序的for循环中出现异常的话，主线程就中断了，导致的结果是 子线程也会意外中断。为了防止这种不正常的结束，需要对可能出现异常的地方用try/catch 包围。

	int main(){
		std::thread t1(fun);
		try{
			for(int i = 0; i < 10; i++){
				//如果发生异常，则主程序中断，子程序也会中断
				std::cout << "in main thread" << std::endl;
			}
		}catch(...)    //... 代表任意类型的异常
		{
			t1.join();
			throw; //将错误往上抛
		}
		t1.join();    //这段代码如果不写这句就会报错，是为什么？
	}
	
> - 线程不止可以被函数构造，还可以通过任何<strong>可被调用</strong>的对象构造。创建这样一个类:

	class Fctor{
	public:
		void operator()(){ //这个名字是不是一定的？两个括号是什么意思？
			for(int i = 0; i > -10; i--){
				std::cout << "for ft1" << i << std::endl;
			}
		}
	};
	int main(){
		Fctor ft1;    //实例化对象
		std::thread t1(ft1);    //通过对象构造线程
		std::thread t1((Fctor()));    //和上述方法完全等效的另一种构造方式。这种方式叫什么呢？ //java中叫匿名函数
		try{
			for(int i = 0; i < 10; i++){
				//如果发生异常，则主程序中断，子程序也会中断
				std::cout << "in main thread" << std::endl;
			}
		}catch(...)    //... 代表任意类型的异常
		{
			t1.join();
			throw; //将错误往上抛
		}
		t1.join();
	}
	

> - 向线程传递参数
	修改class中的 operator()()的参数以及主函数。

	#include <string>
	class Fctor{
	public:
		void operator()(std::string args){
			for(int i = 0; i > -10; i--){
				std::cout << "for ft1" << args << std::endl;
			}
		}
	};
	int main(){
		string s = "i love you";
		Fctor ft1;    //实例化对象时不用传递参数
		std::thread t1(ft1, s);    //参数是在线程创建的时候向线程传递的
		
		std::thread t1((Fctor()), s);   
		try{
			for(int i = 0; i < 10; i++){
				std::cout << "in main thread" << std::endl;
			}
		}catch(...)    //... 代表任意类型的异常
		{
			t1.join();
			throw; //将错误往上抛
		}
		t1.join();
	}
	执行上述代码，可以清楚的得到结果，成功的往线程里传递了参数。
	
> - 线程的引用传参
	有时候我们为了节省空间，提高效率，会采用引用的方式传递参数
		
	这段代码在我的电脑上执行会报错，windous下可以执行	
	#include <string>
	class Fctor{
	public:
		void operator()(std::string& args){
			for(int i = 0; i > -10; i--){
				std::cout << "for ft1" << args << std::endl;
				args = "i don't love you";
			}
		}
	};
	int main(){
		string s = "i love you";
		Fctor ft1;    //实例化对象时不用传递参数
		std::thread t1(ft1, s);    //参数是在线程创建的时候向线程传递的
		
		std::thread t1((Fctor()), s);   
		try{
			for(int i = 0; i < 10; i++){
				std::cout << "in main thread" << std::endl;
			}
		}catch(...)    //... 代表任意类型的异常
		{
			t1.join();
			throw; //将错误往上抛
		}
		t1.join();
		
		std::cout << "in main,s is:" << s << std::endl;
	}
	执行上述程序，可以看到，s并没有在线程中被更改，此处还是复制传参。
	
	
	正确引用传递参数的方法是：  std::thread t1((Fctor()), std::ref(s));
	
	
> - 线程间数据的移动
	有时候我们不想在两个线程之间共享数据，因为则会导致数据的同步问题，例如上面，我只想将主线程中创建的 ```string s;``` 只让线程 t1 使用，此时就要用数据移动。并且在C++中有的对象只能被移动，不能被复值。如:线程对象(thread t1 = t1是不行的)。
	
	#include <string>
	class Fctor{
	public:
		void operator()(std::string& args){    //教程上给的是引用传参，但我自己的不能使用引用传参，必须使用复制传参，如果这里编译通不过的可以把引用去掉试试。
			for(int i = 0; i > -10; i--){
				std::cout << "for ft1" << args << std::endl;
				args = "i don't love you";
			}
		}
	};
	int main(){
		string s = "i love you";
		std::thread t1((Fctor()), std::move(s));       //通过move把 s 移动到线程 t1。
		try{
			for(int i = 0; i < 10; i++){
				std::cout << "in main thread" << std::endl;
			}
		}catch(...)    //... 代表任意类型的异常
		{
			t1.join();
			throw; //将错误往上抛
		}
		t1.join();
		
		std::cout << "in main,s is:" << s << std::endl;  //因为参数已经移动到线程 t1  所以此处的s输出便为空。
	}
	
	
> - 线程的移动
	线程是不能被复制的，只能移动。
	
	std::thread t2 = std::move(t1);  //移动后 t1 为空。
	
> - 线程的id
	每个线程都有一个唯一的id，可以获取：```this_thread::get_id();```获取当前线程的id。
> - 最大线程数
	在写并发编程的时候，要考虑cup的性能，频率等。不能一味的就创建尽量多的线程。```std::thread::hardware_concurrency()```可以获取cup可支持的最大线程数。
	
	
	
-----
#### ch3 数据竞争和互斥 #####
----

> - 线程间的竞争
	多个线程之间会争夺同一资源
	
	#include<iostream>
	#include<thread>
	using namespace std;
	void function_1(){
	    for (int i = 0; i < 100; ++i) {
		cout <<  "from t1:" << i << endl;  //线程 t1 使用cout
	    }
	}
	int main(int argc, char *argv[])
	{
	    thread t1(function_1);
	    for(int i = 0; i > -100; i--){
		cout << "for main:" << i << endl;  //线程 main 使用cout
	    }
	    t1.join();
	    return 0;
	}
	可以看到，输出的结果杂乱无章，很多输出还是被从中间截断了的。因为两个线程同时在抢夺 cout 资源。
	
> - 使用互斥锁保持数据完整
	可以使用mutex对象提供的锁方法来锁定在使用中的数据。
	
	#include<mutex>
	mutex mu;    //实例化锁对象
	void share_print(string msg, int id){    //共享函数
	mu.lock();    //加锁
	cout << msg << id << endl;
	mu.unlock();    //释放锁
	}
	void function_1(){
   	for (int i = 0; i < 100; ++i) {
        	share_print("from t1:", i);    //调用共享和函数
    		}
	}
	int main(int argc, char *argv[])
	{
	    thread t1(function_1);
	    for(int i = 0; i > -100; i--){
		cout << "for main:" << i << endl;  //线程 main 使用cout
	    }
	    t1.join();
	    return 0;
	}
	//执行完毕，可以看到 t1线程里面句子都完整的输出，不会被打乱，但主线程里面的还是会被打乱，因为主线程的没有加锁。将主线程中的cout换成调用函数就能完整了。

> - 使用互斥锁的缺陷
	== 如果在加锁的范围内出现了异常，那么锁将永远不会被解开。 ==
	为了解决上述问题，需要改进lock();
	
	#include<mutex>
	mutex mu;    //实例化锁对象
	void share_print(string msg, int id){    //共享函数
	//mu.lock();    不使用mu.lock() 加锁。
	lock_guard<mutex> guard(mu);    //把锁工具 mu 交给guard对象控制，当guard对象销毁的时候，会自动解说。这样就避免了在加锁期间出现异常，无法解锁而造成死锁的问题。
	cout << msg << id << endl;
	// mu.unlock();    锁会由guard对象自动释放
	}
	然而这样还是没能完整的保护好，因为cout是个全局对象，不是所有的cout都能不使用参与竞争。
	
> - 构造对象保护
	为了解决上述问题，我们可以构造出一个对象来对需要保护的对象进行全方位的保护。
	
	class LockFile    //确保同时只有一个线程能操作这个文件,该对象是f文件能访问的唯一通道。
	{
	private:
	    ofstream f;
	    mutex m_mu;

	public:
	    LockFile(){
		 f.open("log.txt");
	    }
	    void share_print(string id, int vaue){    //真正起作用的事这个方法,这个方法保护了文件
		  lock_guard<mutex> guard(m_mu);
		  f << "from" << id << ":" << vaue << endl; 
	    }
	}; 
	
    void function_1(LockFile& lockfile){    //访问文件必须通过锁对象
   	        for (int i = 0; i < 100; ++i) {
        	    lockfile.share_print("from t1:", i);
         	}
    }
    
    int main(int argc, char *argv[])
    {
	LockFile lockfile;
	thread t1(function_1,ref(lockfile))
        for(int i = 0; i > -100; i--){
        	lockfile.share_print("main", i);    //访问文件必须通过锁对象
	    }
	    t1.join();
	    return 0;
    }
    使用锁对象来保护文件是个很不错的方式,但需要注意的是,锁对象不能将受保护对象返回到外面,也不能将受保护对象作为函数参数;eg:
    	fstream& LockFile::getFile(){return f;}
    	void LockFile::doSomething(void fun(ofstream&)){
    		fun(f);
    	}
    	
 
   	
---- 
#### 死锁 ####
----

> - 死锁的产生:
		有时候我们需要同时使用两把或则以上的锁,如果锁的使用顺序不同,第一个线程获得了第一把锁,第二个线程获得了第二把锁,那么就会导致第一个线程等待第二个线程解开第二把锁,而第二个线程又在等待第一个线程解开第一把锁.这样就形成了死锁.
		
    class LockFile{
	  void share_print(string id, int vaue){
		// this_thread::sleep_for(chrono::microseconds(10));
		lock_guard<mutex> guard(m_mu);
		lock_guard<mutex> guard2(m_mu2);
		cout << "from" << id << ":" << vaue << endl; 
	     }
	     //上下两个方法同时使用了两把锁锁同一个文件,并且两把锁的获取顺序不同
	   void share_print2(string id, int vaue){
		// this_thread::sleep_for(chrono::microseconds(10));
		lock_guard<mutex> guard2(m_mu2);
		lock_guard<mutex> guard(m_mu);
		cout << "from" << id << ":" << vaue << endl; 
	    }
    };
    
    void function_1(LockFile &lockfile){
   		for(int i = 0; i < 100; i++){
   			lockfile.shart_print("t1", i);    //t1调用的事 share_print()
   		}
   	
    }
   
    int main(){
   	LockFile lockfile;
   	thread t1(function_1, ref(lockfile))
   	for(int i = 0; i < 100; i++){
   	    lockfile.share_print2("main", i);    //main函数调用的是share_print2()
        }
    }
    执行以上代码,可能会出现卡死
    
    
    

>  - 解决和避免死锁
       1. 当使用两把或者以上锁的时候,留意调用的顺序.
       2. 当在使用锁的期间,尽量不要调用其他不明函数,除非你能保证这个函数里面没有任何的锁
       3. 当必须要使用两把或者以上的锁的时候,可以调用C++函数库提供的 std::lock()来保证锁的顺序.
       
        lock(m_mu, m_mu2);    //std::lock() 保证锁的顺序
        lock_guard<mutex> guard(m_mu, adopt_lock);    //目前还不是太明白后面一个参数的意思,大体上是:如果不加第二个参数,则guard对象会进行加锁和解锁操作,如果加了第二个参数,表示但前线程已经获得锁,只需要执行解锁操作.
        lock_guard<mutex> guard2(m_mu2, adopt_lock);
        


----
#### ch5 uniqueLock 和 lazy Initialization ####
----

> - unique Lock
	除了使用guard_lock 来创建锁,我们还可以用 unique_lock来创建锁,并且 unique_lock 具有更高的灵活性.然而==牺牲的是性能.==
	
    void share_print(string id, int vaue){
     	unique_lock<mutex> locker(m_mu);
        cout << "from" << id << ":" << vaue << endl; 
        locker.unlock();    //unique_lock 可以提前解锁
        //...    其他逻辑代码
    }
    使用 unique_lock可以提前解锁.然后执行其他逻辑代码.
    
    
> 也可以在想加锁的地方再加锁,而不是创建对象就加锁

    void share_print(string id, int vaue){
        unique_lock<mutex> locker(m_mu, defer_lock);    //defer_lock表示不上锁
        cout << "你好,我";
        this_thread::sleep_for(chrono::milliseconds(1));
        cout << "知道逆会断" << endl;
        locker.lock();
        cout << "from " << id;
        this_thread::sleep_for(chrono::milliseconds(20));  //此处即使休眠,也不会断开,使得其他线程可以使用这部分
        cout << ":" << vaue << endl; 
        locker.unlock();    //unique_lock 可以提前解锁
        //...    其他逻辑代码
    }
    解锁之后还可以再次调用locker.lock();
    
> lock_guard 与 unique_lock的区别
> >   1. lock_guard 和unique_lock 都不可以被复制,但是 unique_lock 可以被转移,但是lock_gurad 不可以
> > 2. lock_guard 不可以重复的加解锁 
	
    unique_lock<mutex> locker2 = std::mive(locker);

    
> - lazy initlialization 
	在之前我们创建的LockFile 类中, 每实例化一个对象,就打开一次文件,显然,这是不必要,因此,我们因该在有需要的时候才去打开.所以我们改造一下我们的类,使得文件只被打开一次
	
    void share_print(string id, int vaue){
        if(!f.is_open()){
            unique_lock<mutex> locker2(m_mu2);    //为防止多个线程同时打开一个文件,得加锁.
            f.open("log.txt");
        }
        unique_lock<mutex> locker(m_mu);
        cout << "from " << id << ":" << vaue << endl; 
    }
   然而上述的加锁也不是安全的线程:如果一个线程刚进入if代码块,就被剥夺了cup权利,另一个线程进来,顺利的把文件打开了,此时第一个线程又获得了cup权利,那么就会导致这个文件打开两次.因此,正确的锁方法是锁住  ==f.is_open()==
   
> 然而这样还是会有一些新问题的.比如,每次程序调用share_print() 的时候都会去判断文件是不是被打开,还要创建个锁来保护这段判断代码的执行,这纯粹的是在消耗计算机的资源,并且做着无用.因此我们需要跟好的方法来解决这个问题.

	class LockFile 
	{
	private:
	    ofstream f;
	    mutex m_mu;
	    once_flag m_flag;
	public:
	    LockFile(){
	    }
	    void share_print(string id, int vaue){
		call_once(m_flag, [&]{f.open("log.txt");});    //文件只被打开一次,C++11一下不支持lambda
		unique_lock<mutex> locker(m_mu);
		cout << "from " << id << ":" << vaue << endl; 
	    }
	};


------
#### ch6 条件变量 ####
------
 假设有如下程序
	 
	#include<functional>
	#include <iostream>
	#include<mutex>
	#include<string>
	#include<thread>
	#include<fstream>
	#include<deque>
	using namespace std;

	std::deque<int> q;    //全局队列
	mutex mu;


	void fun1(){
	    int count = 10;
	    while(count > 0){
			std::unique_lock<mutex> locker1(mu);
			q.push_front(count);
			locker1.unlock();
			this_thread::sleep_for(chrono::milliseconds(20));
			count--;
	    }
	}

	void fun2(){
	    int data = 0;
	    while(data != 1){
			std::unique_lock<mutex> locker2(mu);
			cout << "正在尝试读取数据!" <<  count++ << endl;
			if(!q.empty()){
			    data = q.back();
			    q.pop_back();
			    locker2.unlock();
			    cout << "成功读取数据:" << data << endl;
			}else{
			    cout << "数据读取失败!" << endl;
			    locker2.unlock();
			}
		}
	}

	int main(int argc, char *argv[])
	{
	    thread t1(fun1);
	    thread t2(fun2);
	    t1.join();
	    t2.join();
	    return 0;
	}
	上述代码中,两个函数共同操作一个队列,一个函数负责入队,一个负责出队.两个线程同时操作一个变量,因此变量需要加锁.

但执行上述代码,我们可以看到,入队的函数只执行了10次,但是出队的函数却尝试了上万次的去读取数据,而真正度到数据的次数也就只有10次,可见做了多少无用工.究其原因,是出队函数不知道什么时候队列里面有东西,所以它只能不停的去尝试访问.从而造成了这种无用的消耗.那么有没有什么办法能让出队线程不这么一直不停的取访问,而是==入队线程==入队了之后==通知出队线程可以访问==呢?
> > **条件变量就是干这个的**

	#include<condition_variable>    //添加头文件
	#include<functional>
	#include <iostream>
	#include<mutex>
	#include<string>
	#include<thread>
	#include<fstream>
	#include<deque>
	using namespace std;
	
	condition_variable cond;    //实例化条件变量对象
	std::deque<int> q;    //全局队列
	mutex mu;


	void fun1(){
	    int count = 10;
	    while(count > 0){
			std::unique_lock<mutex> locker1(mu);
			q.push_front(count);
			cond.notify_one();    //唤醒一个线程
			locker1.unlock();
			this_thread::sleep_for(chrono::milliseconds(20));
			count--;
	    }
	}

	void fun2(){
	    int data = 0;
	    while(data != 1){
			std::unique_lock<mutex> locker2(mu);
			cout << "正在尝试读取数据!" <<  count++ << endl;
			cond.wait(locker2, []{return !q.empty();});	//进入睡眠状态,直到有cond.notify_one(); 唤醒. 加参数是为了防止自启动
			data = q.back();
			q.pop_back();
			locker2.unlock();
			cout << "成功读取数据:" << data << endl;
		}
	}

	int main(int argc, char *argv[])
	{
	    thread t1(fun1);
	    thread t2(fun2);
	    t1.join();
	    t2.join();
	    return 0;
	}
	
如上述这样,cond便成了生产者和消费者之间的条件.


----
#### ch7 future,promise 和 async() ####
----
有时候,我们是需要两个线程之间交换数据的.比如下面程序:

    #include<iostream>
    #include<thread>
    using namespace std;
    int add(int a){
    cout << "results is : " << a + a << endl;
        return a+a;
    }
    int main(){
        int a = 2;
        thread t1(add,std::ref(a));
        t1.join();
        return 0;
    }
    如果我们需要在主函数里面读取add函数的返回值,那么怎么办呢?
    
> 这就用到async了,修改我们的代码如下

    #include<future>
    int main(){
	    int a = 2;
	    future<int> fu = std::async(add, 4, a);
	    cout << fu.get() << endl;    //fu.get()函数稚嫩被调用一次,调用多次程序崩溃. 
	    return 0;
    }
    上述代码中,使用了async,他会异步执行一个可调用对象,并返回一个future对象.
    -------- future可以简单的理解成:现在未知,但以后一定会有并且确定的东西
    
> > async不一定会创建线程,这取决于其第一个参数.
> > 1. std::launch::referred:不创建线程,只是延期启动可调用对象.当fu.get()方法被调用的时候,才启动可调用对象
> > 2. std::launch::async: 启动一个线程来调用这个可可调用对像
> > 3. std::launch::referred | std::launch::async: 默认是这个,不任何参数

> - 从父线程中获取变量
     上面的例子是父线程获取子线程的变量,如果子线程相获取父线程的变量,就得用另一种方法.修改代码如下:
     
    int add(int a, future<int>& f){
	    int b = f.get();    // 1号注释
	    cout << "results is : " << a + b << endl;
	    return a+b;
    }
     
    int main(){
	    int a;
	    std::promise<int> p;    // 2号注释
	    future<int> f = p.get_future();    // 3号注释
	    future<int> fu = std::async(add, 4, ref(f));
	    a = 20;
	    p.set_value(a);    // 4 号注释
	    cout << fu.get() << endl; 
	    return 0;
    }
    上述代码中,我们是先创建的线程,在往线程里面传递参数(注释4的地方传递的参数),实现了子线程读取父线程中变量的目的.有一下地方需要注意.
    1. 注释2的地方表示给个承诺.由p产生的一个future对象在将来一定会给其设置值.所以就一定要有注释4这样设置值的地方存在.从而实现承诺p.
    2.如果注释4处没有给值实现承诺,那么当在其他线程中访问p产生的future对象的时候就会抛出 std::future_error::broken_promise 异常
    3. promise和future只能被移动,不能被复制
    
> 上面我们说了, future是不能复制的,只能移动.也就是说,当main线程的第一个子线程取走future之后,main里面就不存在这个future了.当main有多个子线程的时候并且同时需要这个future的时候,就 只有一个线程能得到.解决方法有:
> > 1. 创建多个promise 和多个 future, 分别传给不同的线程.(重复)
> > 2. 使用c++提供的future.share();方法,返回一个shared_future 对象.shared_future可以被复制.

	int add(int a, shared_future<int> f){    //由于shared_futurek可以被复制,可以通过值传递
	    int b = f.get();
	    cout << "results is : " << a + b << endl;
	    return a+b;
	}

	int main(){
	    int a;
	    std::promise<int> p;
	    future<int> f = p.get_future();    //实例化一个future
	    shared_future<int> fs = f.share();    //由f实例化出shared_future
	    future<int> fu = std::async(add, 4, fs);    //4个线程同时使用一个shared_future
	    future<int> fu1 = std::async(add, 4,fs);    //可以只用值传递
	    future<int> fu2 = std::async(add, 4,fs);
	    future<int> fu3 = std::async(add, 4,fs);
	    a = 20;
	    p.set_value(a);
	    cout << fu.get() << endl;   
	    return 0;
	}
	
	
	
----
#### ch8 可调用对象 ####
----

> - 可调用对象有5种:
> > 1. 函数.
> > 2. 函数指针.
> > 3. lambda表达式
> > 4. bind函数
> > 5. 重载了函数调用运算符的对象.
		函数调用运算符:()
		
> - 创建线程的9中方法:假设存在 class A a; 和 function f(); A 重载了'()'运算符.
> > - thread t1(a);    //用a的拷贝
> > - thread t2(std::ref(a));   //用a的引用
> > - thread t3(std::move(a));    //用a.
> > - thread t4(A());    //A的构造函数,一个临时的A()对象
> > - thread t5(f);    //用函数f
> > - thread t6(&f)    //用函数的引用    //函数名本身就是函数的指针变量,入同数组名就是该数组的指针变量一样
> > - thread t7([](){});    //使用lambda函数
> > - thread t8(&A::f, a);    //使用对象a.f()函数拷贝
> > - thread t9(&A::f, &a);    //使用a.f() 函数的引用

## 暂时更新到此 ##
    
     

    
 

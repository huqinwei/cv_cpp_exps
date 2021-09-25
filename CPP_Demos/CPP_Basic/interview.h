
//面试题
//派生访问：在公有派生类的成员函数不能直接访问基类中继承来的某个成员，则该成员一定是基类中（私有）
//
class A {
private:
	int a;
protected:
	int b;

};
class B :public A
{
	int func() { return b; };
};

//在C++中,已知在32位系统中,一个结构体struct stu {short i;struct {char j; int k;};int m;char n;};,那么在考虑字节对齐下,编译后sizeof(stu)的值是多少（20）
struct stu {
	short i;
	struct {
		char j; int k;
	};
	int m;
	char n;
};

//嵌套类：类中类 的权限
class c1
{
public:
	int a;
	void foo();
	class c2//外部可以用c1::c2直接使用嵌套类
	{
	public:
		int a;
		void foo();
		void foooo();
	} b;
private://除了用a::b，我还需要给他用public,private外部不能直接访问
	class c3
	{
	public:
		int a;
		void foo() { a = 9l; }
		void foooo() { a = 9l; }
	} c;
};
void c1::foo()
{
	a = 1;
}
void c1::c2::foo()
{
	a = 2;
}

void c1::c2::foooo()
{
	a = 2;
}


//哪怕只有一个union，确实也不能不加分号
union var {
	long int l;
	int i;
};



//B的func2是不是虚函数
//关于怎么编译通过，虚函数机制：\
1.func1纯虚，B类对象不可以转换到A指针\
2.不纯虚，你必须实现两个func1
using namespace std;
class A_210909 {
public:
	virtual void func1() = 0;// { cout << "func1 in class A " << endl; }
	void func2();
};
class B_210909 :A_210909 {
public:
	void func1() { cout << "func1 in class B " << endl; }
	virtual void func2() { cout << "func2 in class B " << endl; }
};
class C_210909 {
public:
	virtual void func1() = 0;// { cout << "func1 in class A " << endl; }
	virtual void func2() = 0;
};





///////////////////////////////
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <mutex>
struct Base
{
	Base() { std::cout << "  Base::Base()\n"; }
	// Note: non-virtual destructor is OK here
	~Base() { std::cout << "  Base::~Base()\n"; }
};

struct Derived : public Base
{
	Derived() { std::cout << "  Derived::Derived()\n"; }
	~Derived() { std::cout << "  Derived::~Derived()\n"; }
};

void thr(std::shared_ptr<Base> p)
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::shared_ptr<Base> lp = p; // thread-safe, even though the
								  // shared use_count is incremented
	{
		static std::mutex io_mutex;
		std::lock_guard<std::mutex> lk(io_mutex);
		std::cout << "local pointer in a thread:\n"
			<< "  lp.get() = " << lp.get()
			<< ", lp.use_count() = " << lp.use_count() << '\n';
	}
}
/*
int main()
{
	std::shared_ptr<Base> p = std::make_shared<Derived>();

	std::cout << "Created a shared Derived (as a pointer to Base)\n"
		<< "  p.get() = " << p.get()
		<< ", p.use_count() = " << p.use_count() << '\n';
	std::thread t1(thr, p), t2(thr, p), t3(thr, p);
	p.reset(); // release ownership from main
	std::cout << "Shared ownership between 3 threads and released\n"
		<< "ownership from main:\n"
		<< "  p.get() = " << p.get()
		<< ", p.use_count() = " << p.use_count() << '\n';
	t1.join(); t2.join(); t3.join();
	std::cout << "All threads completed, the last one deleted Derived\n";
}
*/



//////////////////////////////////////////////


struct s_A
{
	static int g_constructCount;
	static int g_copyConstructCount;
	static int g_destructCount;

	s_A() {
		cout << "construct: " << ++g_constructCount << endl;
	}

	s_A(const s_A& a)
	{
		cout << "copy construct: " << ++g_copyConstructCount << endl;
	}
	~s_A()
	{
		cout << "destruct: " << ++g_destructCount << endl;
	}
};
int s_A::g_constructCount = 0;
int s_A::g_copyConstructCount = 0;
int s_A::g_destructCount = 0;

s_A Get_s_A()
{
	return s_A();
}
//VS 不能添加-fno-elide-constructors，呗强行优化
/*
int main() {
	s_A a = Get_s_A();
	return 0;
}
*/
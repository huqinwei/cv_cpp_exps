
//������
//�������ʣ��ڹ���������ĳ�Ա��������ֱ�ӷ��ʻ����м̳�����ĳ����Ա����ó�Աһ���ǻ����У�˽�У�
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

//��C++��,��֪��32λϵͳ��,һ���ṹ��struct stu {short i;struct {char j; int k;};int m;char n;};,��ô�ڿ����ֽڶ�����,�����sizeof(stu)��ֵ�Ƕ��٣�20��
struct stu {
	short i;
	struct {
		char j; int k;
	};
	int m;
	char n;
};

//Ƕ���ࣺ������ ��Ȩ��
class c1
{
public:
	int a;
	void foo();
	class c2//�ⲿ������c1::c2ֱ��ʹ��Ƕ����
	{
	public:
		int a;
		void foo();
		void foooo();
	} b;
private://������a::b���һ���Ҫ������public,private�ⲿ����ֱ�ӷ���
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


//����ֻ��һ��union��ȷʵҲ���ܲ��ӷֺ�
union var {
	long int l;
	int i;
};



//B��func2�ǲ����麯��
//������ô����ͨ�����麯�����ƣ�\
1.func1���飬B����󲻿���ת����Aָ��\
2.�����飬�����ʵ������func1
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
//VS �������-fno-elide-constructors����ǿ���Ż�
/*
int main() {
	s_A a = Get_s_A();
	return 0;
}
*/
#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>

#include "AccountManager.h"
#include "UserManager.h"

void Func1()
{
	for (int32 i = 0; i < 100; i++)
	{
		UserManager::Instance()->ProcessSave();
	}
}

void Func2()
{
	for (int32 i = 0; i < 100; i++)
	{
		AccountManager::Instance()->ProcessLogin();
	}
}

int main()
{
	std::thread t1(Func1);
	std::thread t2(Func2);

	t1.join();
	t2.join();

	cout << "Jobs Done" << endl;

	////참고
	//mutex m1;
	//mutex m2;
	//std::lock(m1, m2); 내부적으로 mutex의 일련의 번호를 발급하여 deadlock 관리

	//// adopt_lock: 이미 lock된 상태니까, 나중에 소멸될 때 lock을 풀도록 함
	//lock_guard<mutex> g1(m1, std::adopt_lock);
	//lock_guard<mutex> g1(m2, std::adopt_lock);
}

vector<int32> v; //atomic 사용불가

// Mutual Exclusive (상호배타적)
mutex m;

// RAII (Resource Acquisition is Initialization)
template<typename T>
class LockGuard
{
public:
	LockGuard(T& m)
	{
		_mutex = &m;
		_mutex->lock();
	}

	~LockGuard() //지역변수로 활용된 클래스가 더이상 활용되지 않을 때 자동으로 폐기, 언락
	{
		_mutex->unlock();
	}

private:
	T* _mutex;

};

void Push()
{
	for (int32 i = 0; i < 10000; i++)
	{
		//자물쇠 잠그기
		//m.lock();
		//LockGuard<std::mutex> lockGuard(m);
		std::lock_guard<std::mutex> lockGuard(m);

		//std::unique_lock<std::mutex> uniqueLock(m, std::defer_lock); //바로 잠그지 않고, 잠그는 시점을 따로 설정이 가능
		//uniqueLock.lock();
		

		v.push_back(i);

		if (i == 5000)
			break;

		//m.unlock();
	}
}

//int main()
//{
//	//v.reserve(20000);
//
//	std::thread t1(Push);
//	std::thread t2(Push);
//
//	t1.join();
//	t2.join();
//
//	cout << v.size() << endl;
//}

// atomic : All-Or-Nothing

// DB
atomic<int32> sum = 0;

void Add()
{
	for (int32 i = 0; i < 100'0000; i++)
	{
		sum.fetch_add(1);
		//sum++;
		//int32 eax = sum;
		//eax = eax + 1;
		//sum = eax;
	}
}

void Sub()
{
	for (int32 i = 0; i < 100'0000; i++)
	{
		sum.fetch_add(-1);
		//sum--;
		//int32 eax = sum;
		//eax = eax - 1;
		//sum = eax;
	}
}

//int main()
//{
//	Add();
//	Sub();
//
//	cout << sum << endl;
//
//	std::thread t1(Add);
//	std::thread t2(Sub);
//	t1.join();
//	t2.join();
//	cout << sum << endl;
//}

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

void HelloThread_2(int32 num)
{
	cout << num << endl;
}

//int main()
//{
//	vector<std::thread> v;
//
//	for (int32 i = 0; i < 10; i++)
//	{
//		v.push_back(std::thread(HelloThread_2, i));
//	}
//
//	//std::thread t(HelloThread_2, 10);
//	//
//	//if(t.joinable())
//	//	t.join();
//
//	for (int32 i = 0; i < 10; i++)
//	{
//		if (v[i].joinable())
//			v[i].join();
//	}
//
//	cout << "Hello Main" << endl;
//}
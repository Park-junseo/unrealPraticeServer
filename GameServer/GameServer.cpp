#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>

#include "AccountManager.h"
#include "UserManager.h"

#include <Windows.h>

mutex m;
queue<int32> q;
HANDLE handle;

void Producer()
{
	while (true)
	{
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		::SetEvent(handle); //커널 오브젝트 handle을 singal 상태로 변환
		this_thread::sleep_for(10000ms);
	}
}

void Consumer()
{
	int32 i = 0;
	while (true)
	{
		::WaitForSingleObject(handle, INFINITE); //논 시그널 상태일 시 수면상태 //bManualRset이 자동리셋이므로 Signal일 시 다시 Non-Signal로
		//::ResetEvent(handle); //자동리셋 아닐 시 수동으로 Non-Signal로 리셋하여야 함

		unique_lock<mutex> lock(m);
		if (q.empty() == false)
		{
			int32 data = q.front();
			q.pop();
			cout << data << " " << i++ << endl;
		}
	}
}

int main()
{
	// 커널 오브젝트
	// Usage Count
	// Signal (파란불) / Non-Signal (빨간불) << bool
	// Auto / Manual << bool
	//window api, bManualReset: 수동 리셋/자동 리셋(false), bInitialState: signal/non signal
	handle = ::CreateEvent(NULL/*보안속성*/, FALSE/*bManualReset*/,FALSE/*bInitialState*/,NULL); //handel 인덱스를 넘김

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}

/*
class SpinLock
{
public:
	void lock()
	{
		//CAS (Compare-And-Swap)
		
		bool expected = false;
		bool desired = true;

		//// CAS 의사 코드
		//if (_locked == expected)
		//{
		//	expected = _locked;
		//	_locked = desired;
		//	return true;
		//}
		//else
		//{
		//	expected = _locked;
		//	return false;
		//}
		
		//epxrected: locked 값을 예상해주고, desired: locked값이 바뀌길 원하는 값
		while (_locked.compare_exchange_strong(expected, desired) == false) //expected에 _locked 값이 들어가고 expected가 desired 값이 되면 성공 true 반환
		{
			expected = false;

			this_thread::sleep_for(0ms);
			//this_thread::yield(); <--this_thread::sleep_for(0ms);
			//this_thread::sleep_for(std::chrono::milliseconds(100));
		}

		//while (_locked)
		//{

		//}
		//_locked = true;
	}

	void unlock()
	{
		//_locked = false;
		_locked.store(false); //_locked 변수가 atomic임을 알림
	}

private:
	//volatile: 컴파일러에게 최적화 X
	//volatile bool _locked = false;

	atomic<bool> _locked = false;
};

int32 sum = 0;
mutex m;
SpinLock spinLock;

void Add()
{
	for (int32 i = 0; i < 10'0000; i++)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum++;
	}
}

void Sub()
{
	for (int32 i = 0; i < 10'0000; i++)
	{
		lock_guard<SpinLock> guard(spinLock);
		sum--;
	}
}

int main()
{
	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;
}
*/

/*

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

*/
/*

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

int main()
{
	//v.reserve(20000);

	std::thread t1(Push);
	std::thread t2(Push);

	t1.join();
	t2.join();

	cout << v.size() << endl;
}
*/

/*

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

int main()
{
	Add();
	Sub();

	cout << sum << endl;

	std::thread t1(Add);
	std::thread t2(Sub);
	t1.join();
	t2.join();
	cout << sum << endl;
}

*/

/*

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

void HelloThread_2(int32 num)
{
	cout << num << endl;
}

int main()
{
	vector<std::thread> v;

	for (int32 i = 0; i < 10; i++)
	{
		v.push_back(std::thread(HelloThread_2, i));
	}

	//std::thread t(HelloThread_2, 10);
	//
	//if(t.joinable())
	//	t.join();

	for (int32 i = 0; i < 10; i++)
	{
		if (v[i].joinable())
			v[i].join();
	}

	cout << "Hello Main" << endl;
}

*/
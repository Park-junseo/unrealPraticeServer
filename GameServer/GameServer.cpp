#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>
#include <mutex>

#include <Windows.h>

#include <future>

#include "ThreadManager.h"

#include "RefCounting.h"
#include "Memory.h"

class Player
{
public:
	Player() {}
	virtual ~Player() {}
};

class Knight : public Player
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	Knight(int32 hp) : _hp(hp)
	{
		cout << "Knight()" << endl;
	}

	~Knight() 
	{
		cout << "~Knight()" << endl;
	}

	//static void* operator new(size_t size)
	//{
	//	cout << "new! " << size << endl;
	//	void* ptr = ::malloc(size);
	//	return ptr;
	//}

	//static void operator delete(void* ptr)
	//{
	//	cout << "delete! " << endl;
	//	::free(ptr);
	//}

	//static void* operator new[](size_t size)
	//{
	//	cout << "new[]! " << size << endl;
	//	void* ptr = ::malloc(size);
	//	return ptr;
	//}

	//static void operator delete[](void* ptr)
	//{
	//	cout << "delete[]! " << endl;
	//	::free(ptr);
	//}

	int32 _hp = 100;
	int32 _mp = 10;
};

// new operator overloading (Global)
//void* operator new(size_t size)
//{
//	cout << "new! " << size << endl;
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//
//void operator delete(void* ptr)
//{
//	cout << "delete! " << endl;
//	::free(ptr);
//}
//
//void* operator new[](size_t size)
//{
//	cout << "new[]! " << size << endl;
//	void* ptr = ::malloc(size);
//	return ptr;
//}
//
//void operator delete[](void* ptr)
//{
//	cout << "delete[]! " << endl;
//	::free(ptr);
//}

int main()
{
	//Knight* k1 = new Knight();
	//k1->_hp = 200;
	//k1->_mp = 50;
	//delete k1;
	//k1->_hp = 100;	//잘못된 메모리 접근

	//Player* p = new Player();
	//Knight* k = static_cast<Knight*>(p);
	//k->_hp = 200;		//잘못된 멤버변수에 접근

	//// 가상 메모리 기본
	//int* num = new int;
	//*num = 100;
	//int64 address = reinterpret_cast<int64>(&num);
	//cout << address << endl;

	//int* num2 = reinterpret_cast<int*>(525471840008); //이전 address의 값
	//*num2 = 200; //접근 거부
	//delete num;

	//SYSTEM_INFO info;
	//::GetSystemInfo(&info);

	//info.dwPageSize; // 4KB (0x1000)
	//info.dwAllocationGranularity; // 64KB (0x1000)

	//Knight* k1 = new Knight();
	//int* test = (int*)::VirtualAlloc(NULL, 4, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	//*test = 100;
	//::VirtualFree(test, 0, MEM_RELEASE);
	//*test = 200;

	Knight* knight = xnew<Knight>(100);

	xdelete(knight);

	knight->_hp = 100;
}

// 스마트 포인터
/*
using KnightRef = TSharedPtr<class Knight>;
using InventoryRef = TSharedPtr<class Inventory>;

class Knight : public RefCountable
{
public:
	Knight()
	{
		cout << "Knight()" << endl;
	}

	~Knight()
	{
		cout << "~Knight()" << endl;
	}

	void SetTarget(KnightRef target)
	{
		_target = target;
	}

	KnightRef _target = nullptr;
	InventoryRef _inventory = nullptr;
};

class Inventory : public RefCountable
{
public:
	Inventory(KnightRef knight) : _knight(**knight) //레퍼런스 카운터를 증가시키지 않고 참조
	{

	}

	Knight& _knight;
};

int main()
{
	// 1) 이미 만들어진 클래스 대상으로 사용 불가
	// 2) 순환 (Cycle) 문제

	KnightRef k1(new Knight());
	k1->ReleaseRef();
	// 거의 일어나지 않지만, 순환 문제 발생
	//KnightRef k2(new Knight());
	//k2->ReleaseRef();

	//k1->SetTarget(k2);
	//k2->SetTarget(k1);

	//k1 = nullptr;
	//k2 = nullptr;

	k1->_inventory = new Inventory(k1);
	//결과: 레퍼런스 카운트: k1 - 1 / inventory - 2

	//shared_ptr<Knight> spr(new Knight());
	shared_ptr<Knight> spr = make_shared<Knight>();
	weak_ptr<Knight> wpr = spr;

	bool expired = wpr.expired();
	shared_ptr<Knight> spr2 = wpr.lock();
	if (spr2 != nullptr)
	{

	}

}
*/

//Reference Counting
/*
class Wraight;
class Missile;

using WraightRef = TSharedPtr<Wraight>;
using MissileRef = TSharedPtr<Missile>;

class Wraight : public RefCountable
{
public:
	int _hp = 150;
	int _poxX = 0;
	int _poxY = 0;
};

class Missile : public RefCountable
{
public:
	void SetTarget(WraightRef target)
	{
		_target = target;
		// 중간에 개입 가능
		//target->AddRef();
		Test(target);
	}

	//레퍼런스를 넘길 때 비용이 들 수 있으므로 스마트포인터 레퍼런스로 받아옴
	void Test(WraightRef& target)
	{

	}

	bool Update()
	{
		if (_target == nullptr)
			return true;

		int posX = _target->_poxX; 
		//_target이 delete 된 후에도, 그 공간을 계속 참조하여 연산함
		int posY = _target->_poxY;

		// TODO : 쫓아간다

		if (_target->_hp == 0)
		{
			//_target->ReleaseRef();
			_target = nullptr;
			return true;
		}

		return false;
	}

	WraightRef _target = nullptr;
};

int main()
{
	//Wraight* wraight = new Wraight();
	//Missile* missile = new Missile();
	WraightRef wraight(new Wraight());
	wraight->ReleaseRef(); // 레퍼런스 카운트가 처음 생성될 때 2가 되므로 1로 만듦
	MissileRef missile(new Missile());
	missile->ReleaseRef();

	missile->SetTarget(wraight);
	

	//레이스가 피격 당함
	wraight->_hp = 0;
	// delete wraight;
	// wraight->ReleaseRef();
	wraight = nullptr; // 복사 연산자 실행


	while (true)
	{
		if (missile)
		{
			missile->Update();
		}
	}

	//delete missile;
	// missile->ReleaseRef();
	missile = nullptr;
}
*/

// 소수 구하기
/*
#include <vector>

bool IsPrime(int number)
{
	if (number <= 1)
		return false;
	if (number == 2 || number == 3)
		return true;

	for (int i = 2; i < number; i++)
	{
		if ((number % i) == 0)
			return false;
	}

	return true;
}

// [start - end]
int CountPrime(int start, int end)
{
	int count = 0;
	
	for (int number = start; number <= end; number++)
	{
		if (IsPrime(number))
			count++;
	}

	return count;
}

// 1과 자기 자신으로만 나뉘면 그것을 소수라고 함

int main()
{
	const int MAX_NUMBER = 100'0000;
	// 1-MAX_NUMBER까지의 소수 개수

	vector<thread> threads;

	int coreCount = thread::hardware_concurrency();

	cout << "CoreCount: " << coreCount << endl;

	int jobCount = (MAX_NUMBER / coreCount) + 1;

	atomic<int> primeCount = 0;

	for (int i = 0; i < coreCount; i++)
	{
		int start = (i * jobCount) + 1;
		int end = min(MAX_NUMBER, ((i + 1) * jobCount));

		threads.push_back(thread([start, end, &primeCount]()
			{
				primeCount += CountPrime(start, end);
			}));
	}

	for (thread& t : threads)
		t.join();

	cout << primeCount << endl;
}
*/

// DeadLock Profiler
/*
#include "PlayerManager.h"
#include "LAccountManager.h"

int main()
{

	GThreadManager->Launch([=]
		{
			while (true)
			{
				cout << "PlayerThenAccount" << endl;
				GPlayerManager.PlayerThenAccount();
				this_thread::sleep_for(100ms);
			}
		});

	GThreadManager->Launch([=]
		{
			while (true)
			{
				cout << "AccountThenPlayer" << endl;
				GAccountManager.AccountThenPlayer();
				this_thread::sleep_for(100ms);
			}
		});

	GThreadManager->Join();
}
*/
//RW Lock
/*
class TestLock
{
	USE_LOCK;

public:
	int32 TestRead()
	{
		READ_LOCK;

		if (_queue.empty())
			return -1;

		return _queue.front();
	}

	void TestPush()
	{
		WRITE_LOCK;

		_queue.push(rand() % 100);
	}

	void TestPop()
	{
		WRITE_LOCK;

		if (_queue.empty() == false)
			_queue.pop();
	}

private:
	queue<int32> _queue;
};

TestLock testLock;

void ThreadWrite()
{
	while (true)
	{
		testLock.TestPush();
		this_thread::sleep_for(1ms);
		testLock.TestPop();
	}
}

void ThreadRead()
{
	while (true)
	{
		int32 value = testLock.TestRead();
		cout << value << endl;
		this_thread::sleep_for(1ms);
	}
}

int main()
{
	for (int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch(ThreadWrite);
	}
	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch(ThreadRead);
	}

	GThreadManager->Join();
}

*/

//Lock-Based Stack/Queue
/*
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"

LockQueue<int32> q;
LockStack<int32> s;
LockFreeStack<int32> fs;
LockFreeSmartStack<int32> ss;
LockFreeQueue<int32> fq;

void Push()
{
	while (true)
	{
		int32 value = rand() % 100;
		//q.Push(value);
		//fs.Push(value);
		//ss.Push(value);
		fq.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void Pop()
{
	while (true)
	{
		int32 data = 0;
		
		//if(q.TryPop(OUT data))
		//	cout << data << endl;

		//if (fs.TryPop(OUT data))
		//	cout << data << endl;
		//auto smartData = ss.TryPop();
		//if (smartData != nullptr)
		//	cout << (*smartData) << endl;
		auto smartData = fq.TryPop();
		if (smartData != nullptr)
			cout << (*smartData) << endl;
	}
}

int main()
{
	thread t1(Push);
	thread t2(Pop);

	t1.join();
	t2.join();
}
*/

//Thread Local Storage
/*
//__declspec(thread) int32 value;
thread_local int32 LThreadId = 0;
//thread_local queue<int32> q;

void ThreadMain(int32 threadId)
{
	LThreadId = threadId;

	while (true)
	{
		cout << "Hi! I am Thread " << &LThreadId << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	vector<thread> threads;

	for (int32 i = 0; i < 10; i++)
	{
		int32 threadId = i + 1;
		threads.push_back(thread(ThreadMain, threadId));
	}

	for (thread& t : threads)
		t.join();
}
*/
/*
int main()
{
	atomic<bool> flag = false;

	bool lockFree = flag.is_lock_free();
	//0: lock 필요함
	//1: 이미 원자적으로 처리하므로 lock 필요없음


}
*/
/*

int32 x = 0;
int32 y = 0;
int32 r1 = 0;
int32 r2 = 0;
volatile bool ready;

void Thread_1()
{
	while (!ready)
		;
	y = 1; //Store y
	r1 = x; //Load x
}

void Thread_2()
{
	while (!ready)
		;
	x = 1; //Store x
	r2 = y; //Load y
}

int main()
{
	int32 count = 0;
	
	while (true)
	{
		ready = false;
		count++;

		x = y = r1 = r2 = 0;

		thread t1(Thread_1);
		thread t2(Thread_2);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)
			break;
	}

	cout << count << " 번만에 빠져나옴ㅎㅎ" << endl;
}
*/
/*
int32 buffer[10000][10000];

int main()
{
	memset(buffer, 0, sizeof(buffer));

	// 빠름
	{
		uint64 start = GetTickCount64();

		int64 sum = 0;
		for (int32 i = 0; i < 10000; i++)
			for (int32 j = 0; j < 10000; j++)
				sum += buffer[i][j]; //일련의 데이터에서 한칸씩 이동

		uint64 end = GetTickCount64();

		cout << "Elapsed Tick" << (end - start) << endl;

	}

	// 더 느림
	{
		uint64 start = GetTickCount64();

		int64 sum = 0;
		for (int32 i = 0; i < 10000; i++)
			for (int32 j = 0; j < 10000; j++)
				sum += buffer[j][i];

		uint64 end = GetTickCount64();

		cout << "Elapsed Tick" << (end - start) << endl;

	}

	// 데이터 하나를 불러올 때, 인접데이터도 함께 캐시에 등록 (special locality)
	// 캐시에 있는 데이터를 불러올 때, 캐시 히트가 되며, 캐시 히트가 되면 메모리에서까지 데이터를 불러올 필요 없음
}

*/


/*

int64 result;

int64 Calculate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100'000; i++)
		sum += 1;
	
	return sum;
}

void PromiseWorker(std::promise<string>&& promise)
{
	promise.set_value("Secret Messege");
}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}
int main()
{
	//동기(synchronous) 실행
	int64 sum = Calculate();
	cout << sum << endl;

	//비동기 != 멀티 쓰레드

	// std::future
	// Cacluate 라는 일감 하나를 실행하기 위한 전용 쓰레드를 만들어 떠넘긴 상태
	{
		// 인수 속성
		// 1) defferred -> lazy evaluation 지연 실행
		// 2) async -> 별도의 쓰레드를 만들어서 실행
		// 3) deffered | async -> 둘 중 알아서 선택
		// 비동기 방식으로 Calculate 함수 호출, 언젠가 미래에 결과물을 반환 
		std::future<int64> future = std::async(std::launch::async, Calculate);

		// TODO
		//std::future_status  status = future.wait_for(1ms);
		//if (status == future_status::ready)
		//{

		//}

		//future.wait(); //wait 후에 get 호출하나 wait도 결과물이 나올때까지 기다린다.

		int64 sum = future.get(); //결과물이 이제서야 필요하다

		//class Knight
		//{
		//public:
		//	int64 GetHp() { return 100; }
		//};
		//Knight knight;
		//std::future<int64> future2 = std::async(std::launch::async, &Knight::GetHp, knight);//클래스 메소드 호출 예제
	}

	// std::promise

	{
		// 미래(std::future)에 결과물을 반환해줄 것이라고 약속함 (std::promise) 
		std::promise<string> promise; //promise는 메인 쓰레드가 가짐
		std::future<string> future = promise.get_future();

		thread t(PromiseWorker, std::move(promise)); //PromiseWorker 쓰레드에 promise를 넘김

		string message = future.get();
		cout << message << endl;

		t.join();
	}

	// std::packaged_task
	// 쓰레드를 만들어 일감을 넘김
	// 한 쓰레드가 여러 테스크를 처리할 수 있음
	{
		std::packaged_task<int64(void)> task(Calculate); //Calcuate 결과물 자체가 future를 통해 받아올 수 있음
		std::future<int64> future = task.get_future();

		std::thread t(TaskWorker, std::move(task)); //thread가 호출될 때 task가 호출이 되고 future 객체를 통해 추출해줄 수 있음

		int64 sum = future.get();
		cout << sum << endl;

		t.join();
	}

	// 결론)
	// mutex, condition_variable까지 가지 않고 단순한 애들을 처리할 수 있는
	// 특히나, 한 번 발생하는 이벤트에 유용하다!
	// 닭 잡는데 소 잡는 칼 쓸 필요 없다!
	// 1) async
	// 원하는 함수를 비동기적으로 실행
	// 2) promise
	// 결과물을 promise를 통해 future로 받아줌
	// 3) packaged_task
	// 원하는 함수의 실행 결과를 packaged_task를 통해 future로 받아줌
}
*/

/*
mutex m;
queue<int32> q;
HANDLE handle;

// 참고) CV 는 User-Level Object (커널 오브젝트X)
condition_variable cv;

void Producer()
{
	while (true)
	{
		// 1) Lock을 잡고
		// 2) 공유 변수 값을 수정
		// 3) Lock을 풀고
		// 4) 조건변수 통해 다른 쓰레드에게 통지
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		cv.notify_one(); // wait 중인 쓰레드가 있으면 딱 1개를 깨운다

		//Event Lock
		//::SetEvent(handle); //커널 오브젝트 handle을 singal 상태로 변환
		//this_thread::sleep_for(10000ms);
	}
}

void Consumer()
{
	int32 i = 0;
	while (true)
	{
		unique_lock<mutex> lock(m);
		cv.wait(lock, []() { return q.empty() == false; }); //조건
		// 1) Lock 잡고
		// 2) 조건 확인
		// - 만족0 => 빠져 나와서 이어서 코드 진행
		// - 만족X => Lock 풀어주고 대기 상태

		// 그런데 notify_one을 했으면 항상 조건식을 만족하는가?
		// Suprious Wakeup (가짜 기상?)
		// notify_one할 때 lock을 잡고 있는 것이 아니기 때문

		// Event Lock
		//::WaitForSingleObject(handle, INFINITE); //논 시그널 상태일 시 수면상태 //bManualRset이 자동리셋이므로 Signal일 시 다시 Non-Signal로
		////::ResetEvent(handle); //자동리셋 아닐 시 수동으로 Non-Signal로 리셋하여야 함

		//unique_lock<mutex> lock(m);
		while (q.empty() == false)
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
	handle = ::CreateEvent(
		NULL, //보안속성
		FALSE,//bManualReset
		FALSE,//bInitialState
		NULL
	); //handel 인덱스를 넘김

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);
}
*/

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
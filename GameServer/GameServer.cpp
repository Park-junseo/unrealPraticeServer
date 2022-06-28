#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>
#include <atomic>

void HelloThread()
{
	cout << "Hello Thread" << endl;
}

void HelloThread_2(int32 num)
{
	cout << num << endl;
}

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

	//vector<std::thread> v;

	//for (int32 i = 0; i < 10; i++)
	//{
	//	v.push_back(std::thread(HelloThread_2, i));
	//}

	////std::thread t(HelloThread_2, 10);
	////
	////if(t.joinable())
	////	t.join();

	//for (int32 i = 0; i < 10; i++)
	//{
	//	if (v[i].joinable())
	//		v[i].join();
	//}

	//cout << "Hello Main" << endl;

}
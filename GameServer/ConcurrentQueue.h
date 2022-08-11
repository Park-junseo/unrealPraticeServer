#pragma once
#include <mutex>

template<typename T>
class LockQueue_
{
public:
	LockQueue_() {}

	LockQueue_(const LockQueue_&) = delete;
	LockQueue_& operator=(const LockQueue_&) = delete;

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_queue.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty())
			return false;

		value = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _queue.empty() == false; });
		value = std::move(_queue.front());
		_queue.pop();
	}

private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};

//template<typename T>
//class LockFreeQueue
//{
//	struct Node
//	{
//		shared_ptr<T> data;
//		Node* next = nullptr;
//	};
//
//public:
//	LockFreeQueue() : _head(new Node), _tail(_head)
//	{
//
//	}
//
//	LockFreeQueue(const LockFreeQueue&) = delete;
//	LockFreeQueue& operator=(const LockFreeQueue&) = delete;
//
//	void Push(const T& value)
//	{
//		shared_ptr<T> newData = make_shared<T>(value);
//
//		Node* dummy = new Node();
//		Node* oldTail = _tail;
//		oldTail->data.swap(newData);
//		oldTail->next = dummy;
//
//		_tail = dummy;
//	}
//
//	shared_ptr<T> TryPop()
//	{
//		Node* oldHead = PopHead();
//		if (oldHead == nullptr)
//			return shared_ptr<T>();
//
//		shared_ptr<T> res(oldHead->data);
//		delete oldHead;
//		return res;
//	}
//
//private:
//	Node* PopHead()
//	{
//		Node* oldHead = _head;
//		if (oldHead == _tail)
//			return nullptr;
//
//		_head = oldHead->next;
//		return oldHead;
//	}
//
//private:
//	// [data][data][data][data]
//	// [head][tail]
//	Node* _head = nullptr;
//	Node* _tail = nullptr;
//
//};

template<typename T>
class LockFreeQueue
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount; //참조권
		Node* ptr = nullptr;
	};

	struct NodeCounter
	{
		uint32 internalCount : 30;
		uint32 externalCountRemaining : 2;
	};

	struct Node
	{
		Node()
		{
			NodeCounter newCount;
			newCount.internalCount = 0;
			newCount.externalCountRemaining = 2;
			count.store(newCount);

			next.ptr = nullptr;
			next.externalCount = 0;
		}

		void ReleaseRef()
		{
			NodeCounter oldCounter = count.load();

			while (true)
			{
				NodeCounter newCounter = oldCounter;
				newCounter.internalCount--;

				if (count.compare_exchange_strong(oldCounter, newCounter))
				{
					if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0)
						delete this;
					break;
				}
			}
		}

		atomic<T*> data;
		atomic<NodeCounter> count;
		CountedNodePtr next;
	};

public:
	LockFreeQueue()
	{
		CountedNodePtr node;
		node.ptr = new Node;
		node.externalCount = 1;

		_head.store(node);
		_tail.store(node);
	}

	LockFreeQueue(const LockFreeQueue&) = delete;
	LockFreeQueue& operator=(const LockFreeQueue&) = delete;

	void Push(const T& value)
	{
		unique_ptr<T> newData = make_unique<T>(value);

		CountedNodePtr dummy;
		dummy.ptr = new Node;
		dummy.externalCount = 1;

		CountedNodePtr oldTail = _tail.load();

		while (true)
		{
			// 참조권 획득 (externalCount를 현시점 기준 +1 한 애가 이김)
			IncreaseExternalCount(_tail, oldTail);

			// 소유권 획득 (data를 먼저 교환한 애가 이김)
			T* oldData = nullptr;
			//Node의 data 값이 포인터인 이유, 노드의 데이터에 접근할 때 값이 있는지 확인하기 위해
			if (oldTail.ptr->data.compare_exchange_strong(oldData, newData.get()))
			{
				oldTail.ptr->next = dummy;
				oldTail = _tail.exchange(dummy);

				newData.release(); // 데이터에 대한 unique_ptr의 소유권 포기,
				break;
			}

			//소유권 경쟁 패배
			oldTail.ptr->ReleaseRef();
		}

		/*shared_ptr<T> newData = make_shared<T>(value);

		Node* dummy = new Node();
		Node* oldTail = _tail;
		oldTail->data.swap(newData);
		oldTail->next = dummy;

		_tail = dummy;*/
	}

	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head.load();

		while (true)
		{
			//참조권 획득 (externalCount를 현시점 기준 +1 한 애가 이김)
			IncreaseExternalCount(_head, oldHead);

			Node* ptr = oldHead.ptr;
			if (ptr == _tail.load().ptr)
			{
				ptr->ReleaseRef();
				return shared_ptr<T>();
			}

			//소유권 획득 (head = ptr->next), 
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				T* res = ptr->data.exchange(nullptr);
				FreeExternalCount(oldHead);
				return shared_ptr<T>(res);
			}

			ptr->ReleaseRef();
		}

		/*Node* oldHead = PopHead();
		if (oldHead == nullptr)
			return shared_ptr<T>();

		shared_ptr<T> res(oldHead->data);
		delete oldHead;
		return res;*/
	}

private:
	//Node* PopHead()
	//{
	//	Node* oldHead = _head;
	//	if (oldHead == _tail)
	//		return nullptr;

	//	_head = oldHead->next;
	//	return oldHead;
	//}

	static void IncreaseExternalCount(atomic<CountedNodePtr>& counter, CountedNodePtr& oldCounter) // 첫번째 인자 counter엔 head를 담거나 tail를 담거나
	{
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;

			if (counter.compare_exchange_strong(oldCounter, newCounter))
			{
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

	static void FreeExternalCount(CountedNodePtr& oldNodePtr)
	{
		Node* ptr = oldNodePtr.ptr;
		const int32 countIncrease = oldNodePtr.externalCount - 2;

		NodeCounter oldCounter = ptr->count.load();

		while (true)
		{
			NodeCounter newCounter = oldCounter;
			newCounter.externalCountRemaining--; // TODO
			newCounter.internalCount += countIncrease;

			if (ptr->count.compare_exchange_strong(oldCounter, newCounter))
			{
				if (newCounter.internalCount == 0 && newCounter.externalCountRemaining == 0) //건드리는 쓰레드가 없을 때
					delete ptr;

				break;
			}
		}
	}

private:
	// [data][data][data][data]
	// [head][tail]
	atomic<CountedNodePtr> _head;
	atomic<CountedNodePtr> _tail;

};
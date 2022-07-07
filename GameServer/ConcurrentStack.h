#pragma once

#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack() {}

	LockStack(const LockStack&) = delete; //복사하려 하는 경우 삭제
	LockStack& operator=(const LockStack&) = delete; //연산자를 활용해 복사하려는 경우 또한 삭제

	void Push(T value)
	{
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value)
	{
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;

		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	void WaitPop(T& value)
	{
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return _stack.empty() == false; });
		value = std::move(_stack.top());
		_stack.pop();
	}

	//bool Empty()
	//{
	//	lock_guard<mutex> lock(_mutex);
	//	return _stack.empty();
	//}
private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};

template<typename T>
class LockFreeStack
{
	struct Node
	{
		Node(const T& value) : data(value), next(nullptr)
		{

		}

		T data;
		Node* next;
	};

public:

	// 1) 새 노드를 만들고
	// 2) 새노드의 next = head
	// 3) head = 새 노드

	// [ ][ ][ ][ ][ ][ ]
	// [head]
	void Push(const T& value)
	{
		Node* node = new Node(value);
		node->next = _head;

		//if (_head == node->next)
		//{
		//	_head = node;
		//	return true;
		//}
		//else
		//{
		//	node->next = _head;
		//	return false;
		//}
		while (_head.compare_exchange_weak(node->next, node) == false)
		{
			//node->next = _head;
		}
	}

	// 1) head 읽기
	// 2) head->next 읽기
	// 3) head = head->next
	// 4) data 추출해서 반환
	// 5) 추출한 노드 삭제

	// [ ][ ][ ][ ][ ][ ]
	// [head]
	bool TryPop(T& value)
	{
		++_popCount;

		Node* oldHead = _head;

		//if (_head == oldHead)
		//{
		//	_head = oldHead->next;
		//	return true;
		//}
		//else
		//{
		//	oldHead = _head;
		//	return false;
		//}
		while (oldHead && _head.compare_exchange_weak(oldHead, oldHead->next) == false)
		{
			//oldHead = _head;
		}

		if (oldHead == nullptr)
		{
			--_popCount;
			return false;
		}

		//Exeption x
		value = oldHead->data;

		//잠시 삭제 보류
		//delete oldHead;
		TryDelete(oldHead);

		return true;
	}

	// 1) 데이터 분리
	// 2) Count 체크
	// 3) 나 혼자면 삭제
	void TryDelete(Node* oldHead)
	{
		// 자신 외에 누가 있는가
		if (_popCount == 1)
		{
			//나 혼자네?

			// 이왕 혼자인거, 삭제 예약된 다른 데이터들도 삭제해보자
			Node* node = _pendingList.exchange(nullptr);

			if (--_popCount == 0)
			{
				// 끼어든 애가 없음 -> 삭제 진행
				// 이제와서 끼어들어도, 어차피 데이터는 분리해둔 상태~!
				DeleteNodes(node);
			}
			else if (node)
			{
				//누가 끼어들었으니 다시 갖다 놓자
				ChainPendingNodeList(node);
			}
			
			//내 데이터는 삭제
			delete oldHead;

		}
		else
		{
			// 누가 있네? 그럼 지금 삭제 않고, 삭제 예약만
			ChainPendingNode(oldHead);
			--_popCount;
		}

	}

	// [ ][ ][ ][ ][ ]

	void ChainPendingNodeList(Node* first, Node* last)
	{
		last->next = _pendingList;

		while (_pendingList.compare_exchange_weak(last->next, first) == false)
		{

		}
	}
	// [ ][ ][ ][ ][ ][ ]
	void ChainPendingNodeList(Node* node)
	{
		Node* last = node;
		while (last->next)
			last = last->next;

		ChainPendingNodeList(node, last);
	}

	void ChainPendingNode(Node* node)
	{
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node)
	{
		while (node)
		{
			Node* next = node->next;
			delete node;
			node = next;
		}
	}

private:
	// [ ][ ][ ][ ][ ][ ]
	// [head]
	atomic<Node*> _head;

	atomic<uint32> _popCount = 0; //Pop을 실행중인 쓰레드 개수
	atomic<Node*> _pendingList; // 삭제되어야 할 노드들 (첫번째 노드)
};

//shared_ptr 이 애초에 lock free가 되지 못함
//template<typename T>
//class LockFreeSmartStack
//{
//	struct Node
//	{
//		Node(const T& value) : data(make_shared<T>(value)), next(nullptr)
//		{
//
//		}
//
//		shared_ptr<T> data;
//		shared_ptr<Node> next;
//	};
//public:
//	void Push(const T& value)
//	{
//		shared_ptr<Node> node = make_shared<Node>(value);
//		node->next = std::atomic_load(&_head); //꺼내올 때 레퍼런스 카운터를 1 증가시킨 상태에서 해당 포인터를 건내줘야 함
//		while (std::atomic_compare_exchange_weak(&_head, &node->next, node) == false)
//		{
//
//		}
//	}
//
//	shared_ptr<T> TryPop()
//	{
//		shared_ptr<Node> oldHead = std::atomic_load(&_head); //레퍼런스증감과 적재를 한번에 해야함
//
//		while (oldHead && std::atomic_compare_exchange_weak(&_head, &oldHead, oldHead->next) == false)
//		{
//
//		}
//
//		if (oldHead == nullptr) 
//			return shared_ptr<T>();
//
//		return oldHead->data;
//	}
//
//private:
//	shared_ptr<Node> _head;
//};

template<typename T>
class LockFreeSmartStack
{
	struct Node;

	struct CountedNodePtr
	{
		int32 externalCount = 0;
		Node* ptr = nullptr;
	};

	struct Node
	{
		Node(const T& value) : data(make_shared<T>(value))
		{

		}

		shared_ptr<T> data;
		atomic<int32> internalCount = 0;
		CountedNodePtr next;
	};
public:
	void Push(const T& value)
	{
		CountedNodePtr node;
		node.ptr = new Node(value);
		node.externalCount = 1;
		//[1]
		node.ptr->next = _head;
		while (_head.compare_exchange_weak(node.ptr->next, node) == false)
		{

		}
	}

	shared_ptr<T> TryPop()
	{
		CountedNodePtr oldHead = _head;
		while (true)
		{
			//참조권 획득
			IncreaseHeadCount(oldHead);
		}
	}

private:

	void IncreaseHeadCount(CountedNodePtr& oldCounter)
	{
		//멀티 쓰레드 환경에서 경합이 일어나는데, while 문 아래의 코드를 모두 실행하여 참조권을 획득할 때까지 반복함
		while (true)
		{
			CountedNodePtr newCounter = oldCounter;
			newCounter.externalCount++;

			if (_head.compare_exchange_strong(oldCounter, newCounter))
			{
				oldCounter.externalCount = newCounter.externalCount;
				break;
			}
		}
	}

private:
	atomic<CountedNodePtr> _head;
};
#pragma once

#include <mutex>

template<typename T>
class LockStack
{
public:
	LockStack() {}

	LockStack(const LockStack&) = delete; //�����Ϸ� �ϴ� ��� ����
	LockStack& operator=(const LockStack&) = delete; //�����ڸ� Ȱ���� �����Ϸ��� ��� ���� ����

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

	// 1) �� ��带 �����
	// 2) ������� next = head
	// 3) head = �� ���

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

	// 1) head �б�
	// 2) head->next �б�
	// 3) head = head->next
	// 4) data �����ؼ� ��ȯ
	// 5) ������ ��� ����

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

		//��� ���� ����
		//delete oldHead;
		TryDelete(oldHead);

		return true;
	}

	// 1) ������ �и�
	// 2) Count üũ
	// 3) �� ȥ�ڸ� ����
	void TryDelete(Node* oldHead)
	{
		// �ڽ� �ܿ� ���� �ִ°�
		if (_popCount == 1)
		{
			//�� ȥ�ڳ�?

			// �̿� ȥ���ΰ�, ���� ����� �ٸ� �����͵鵵 �����غ���
			Node* node = _pendingList.exchange(nullptr);

			if (--_popCount == 0)
			{
				// ����� �ְ� ���� -> ���� ����
				// �����ͼ� �����, ������ �����ʹ� �и��ص� ����~!
				DeleteNodes(node);
			}
			else if (node)
			{
				//���� ���������� �ٽ� ���� ����
				ChainPendingNodeList(node);
			}
			
			//�� �����ʹ� ����
			delete oldHead;

		}
		else
		{
			// ���� �ֳ�? �׷� ���� ���� �ʰ�, ���� ���ุ
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

	atomic<uint32> _popCount = 0; //Pop�� �������� ������ ����
	atomic<Node*> _pendingList; // �����Ǿ�� �� ���� (ù��° ���)
};

//shared_ptr �� ���ʿ� lock free�� ���� ����
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
//		node->next = std::atomic_load(&_head); //������ �� ���۷��� ī���͸� 1 ������Ų ���¿��� �ش� �����͸� �ǳ���� ��
//		while (std::atomic_compare_exchange_weak(&_head, &node->next, node) == false)
//		{
//
//		}
//	}
//
//	shared_ptr<T> TryPop()
//	{
//		shared_ptr<Node> oldHead = std::atomic_load(&_head); //���۷��������� ���縦 �ѹ��� �ؾ���
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
			//������ ȹ��
			IncreaseHeadCount(oldHead);
			//�ּ��� externalCount >= 2 ���״� ����X (�����ϰ� ������ �� �ִ� ����)
			Node* ptr = oldHead.ptr;

			//������ ����
			if (ptr == nullptr)
				return shared_ptr<T>();

			//������ ȹ�� (ptr->next�� head�� �ٲ�ġ�� �� �ְ� �̱�)
			//���⿡�� �ٸ� ������� ���� count ���� �ٲ�� ����
			if (_head.compare_exchange_strong(oldHead, ptr->next))
			{
				shared_ptr<T> res;
				res.swap(ptr->data);

				//external : 1 -> 2(��+1) -> 4(��+1 ��+2)
				//internal : 1 -> 0
				const int32 countIncrease = oldHead.externalCount - 2;

				if (ptr->internalCount.fetch_add(countIncrease) == -countIncrease)
					delete ptr;

				return res;
			}
			else if (ptr->internalCount.fetch_sub(1) == 1)
			{
				//�������� �������, �������� ���� -> �޼����� ���� �Ѵ�
				delete ptr;
			}

		}
	}

private:

	void IncreaseHeadCount(CountedNodePtr& oldCounter)
	{
		//��Ƽ ������ ȯ�濡�� ������ �Ͼ�µ�, while �� �Ʒ��� �ڵ带 ��� �����Ͽ� �������� ȹ���� ������ �ݺ���
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

// Memory Pool #2
// ----------------
// 1�� �õ�
// ----------------

/*
struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* header);
void PushEntrySList(SListHeader* header, SListEntry* entry);
SListEntry* PopEntrySList(SListHeader* header);
*/

// ----------------
// 2�� �õ�
// ----------------

/*
struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* header);

void PushEntrySList(SListHeader* header, SListEntry* entry);

SListEntry* PopEntrySList(SListHeader* header);
*/
// ----------------
// 3�� �õ�
// ----------------

struct SListEntry
{
	SListEntry* next;
};

struct SListHeader
{
	SListEntry* next = nullptr;
};

void InitializeHead(SListHeader* header);

void PushEntrySList(SListHeader* header, SListEntry* entry);

SListEntry* PopEntrySList(SListHeader* header);
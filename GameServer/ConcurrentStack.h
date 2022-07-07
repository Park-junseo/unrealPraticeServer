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
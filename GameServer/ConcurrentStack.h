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
		Node(const T& value) : data(value)
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
			return false; 

		//Exeption x
		value = oldHead->data;

		//��� ���� ����
		//delete oldHead;

		return true;
	}

private:
	// [ ][ ][ ][ ][ ][ ]
	// [head]
	atomic<Node*> _head;
};
#pragma once


/*******************
	MemoryHeader
*******************/

struct MemoryHeader
{
	// [MemoryHeader][Data]

	MemoryHeader(int32 size) : allocSize(size) { }

	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size); // placement new
		return reinterpret_cast<void*>(++header);
	}

	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}

	int32 allocSize;
	// TODO : 필요한 추가 정보
};

/*******************
	MemoryPool
*******************/

class MemoryPool      
{
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void			Push(MemoryHeader* ptr); //메모리를 해제하지는 않고 데이터를 삭제할 때
	MemoryHeader*	Pop(); //메모리를 다시 사용할 때

private:
	int32 _allocSize = 0;
	atomic<int32> _allocCount = 0; //메모리 풀에서 뱉어 준 메모리 개수

	USE_LOCK;
	queue<MemoryHeader*> _queue;
};


#pragma once

class SendBufferChunk;

/*----------------
	Sendbuffer
----------------*/

class SendBuffer// : public enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(SendBufferChunkRef owner, BYTE* buffer, int32 allocSize);
	~SendBuffer();

	BYTE*		Buffer() { return _buffer; }
	// 사용할 버퍼 사이즈
	int32		WriteSize() { return _writeSize; }
	//// 버퍼 총용량
	//int32 Capacity() { return static_cast<int32>(_buffer.size()); }
	void		Close(uint32 writeSize);

	//데이터를 복사
	//void CopyData(void* data, int32 len);

private:
	//xVector<BYTE>	_buffer;
	BYTE*				_buffer;
	uint32				_allocSize = 0;
	int32				_writeSize = 0;
	SendBufferChunkRef	_owner;
};

/*----------------------
	SendBufferChunk
----------------------*/

class SendBufferChunk : public enable_shared_from_this<SendBufferChunk>
{
	enum
	{
		SEND_BUFFER_CHUNK_SIZE = 6000//0x1000
	};

public:
	SendBufferChunk();
	~SendBufferChunk();

	void				Reset();
	SendBufferRef		Open(uint32 allocSize);
	void				Close(uint32 writeSize);

	bool				IsOpen() { return _open; }
	BYTE*				Buffer() { return &_buffer[_usedSize]; }
	uint32				FreeSize() { return static_cast<uint32>(_buffer.size()) - _usedSize; }

private:
	xArray<BYTE, SEND_BUFFER_CHUNK_SIZE>	_buffer = { };
	bool									_open = false;
	uint32									_usedSize = 0;

};

/*-----------------------
	SendBufferManager
-----------------------*/

class SendBufferManager
{
public:
	// 메모리 선점
	SendBufferRef		Open(uint32 size);
	
private:
	// 메모리 반납
	SendBufferChunkRef	Pop();
	void				Push(SendBufferChunkRef buffer);

	static void			PushGlobal(SendBufferChunk* buffer);

private:
	USE_LOCK;
	xVector<SendBufferChunkRef> _sendBufferChunks;
};
#pragma once

/*----------------
	Sendbuffer
----------------*/

class SendBuffer : enable_shared_from_this<SendBuffer>
{
public:
	SendBuffer(int32 bufferSize);
	~SendBuffer();

	BYTE* Buffer() { return _buffer.data(); }
	// 사용할 버퍼 사이즈
	int32 WriteSize() { return _writeSize; }
	// 버퍼 총용량
	int32 Capacity() { return static_cast<int32>(_buffer.size()); }

	//데이터를 복사
	void CopyData(void* data, int32 len);

private:
	xVector<BYTE>	_buffer;
	int32			_writeSize = 0;
};


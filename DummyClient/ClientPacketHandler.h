#pragma once

enum
{
	S_TEST = 1
};

class ClientPacketHandler
{
public:

	static void HandlePacket(BYTE* buffer, int32 len);

	static void Handle_S_TEST(BYTE* buffer, int32 len);
};

template<typename T>
class PacketList
{
public:
	PacketList() :_data(nullptr), _count(0) {}
	PacketList(T* data, uint16 count) : _data(data), _count(count) { }

	T& operator[](uint16 index)
	{
		ASSERT_CRASH(index < _count);
		return _data[index];
	}

	uint16 Count() { return _count; }

private:
	T*			_data;
	uint16		_count;
};
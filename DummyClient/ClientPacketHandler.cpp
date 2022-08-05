#include "pch.h"
#include "ClientPacketHandler.h"
#include "BufferReader.h"

void ClientPacketHandler::HandlePacket(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PacketHeader header;
	br >> header;

	switch (header.id)
	{
	case S_TEST:
		Handle_S_TEST(buffer, len);
		break;
	}

}
#pragma pack(1)
// 패킷 설계 TEMP
// [ PKT_S_TEST ][ BuffsListItem BUffsListItem BuffsListItem ][ victim victim ][ victim victim ]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;

		uint16 victimsOffset;
		uint16 victimsCount;

		bool Validate(BYTE* packetStart, uint16 packetSize, OUT uint32& size)
		{
			if (victimsOffset + victimsCount * sizeof(uint64) > packetSize)
				return false;

			size += victimsCount * sizeof(uint64);
			return true;
		}
	};

	uint16 packetSize; // 공용 헤더
	uint16 packetId; // 공용 헤더
	uint64 id;		// 8
	uint32 hp;		// 4
	uint16 attack;	// 2
	//// 가변 데이터
	//// 1) 문자열 (ex. name)
	//// 2) 그냥 바이트 배열 (ex. 길드 이미지)
	//// 3) 일반 리스트
	//vector<BuffData> buffs;
	uint16 buffsOffset;	// 가변데이터를 위한 셋1: 가변데이터 시작 위치
	uint16 buffsCount;	// 가변데이터를 위한 셋1: 가변데이터 개수

	//wstring name;

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		if (packetSize < size)
			return false;

		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;

		// Buffers 가변 데이터 크기 추가
		size += buffsCount * sizeof(BuffsListItem);

		BuffsList buffList = GetBuffsList();
		for (int32 i = 0; i < buffList.Count(); i++)
		{
			if (buffList[i].Validate((BYTE*)this, packetSize, OUT size) == false)
				return false;
		}

		// 최종 크기 비교
		if (size != packetSize)
			return false;

		return true;
	}

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;
	using BuffsVictimsList = PacketList<uint64>;

	BuffsList GetBuffsList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
	}

	BuffsVictimsList GetBuffsVictimList(BuffsListItem* buffsItem)
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsItem->victimsOffset;
		return BuffsVictimsList(reinterpret_cast<uint64*>(data), buffsItem->victimsCount);
	}
};
#pragma pack()

void ClientPacketHandler::Handle_S_TEST(BYTE* buffer, int32 len)
{
	BufferReader br(buffer, len);

	PKT_S_TEST* pkt = reinterpret_cast<PKT_S_TEST*>(buffer);
	//PKT_S_TEST pkt;
	//br >> pkt;

	if (pkt->Validate() == false)
		return;

	//cout << "ID: " << id << " HP: " << hp << " ATT: " << attack << endl;

	
	//for (int32 i = 0; i < pkt->buffsCount; i++)
	//{
	//	//br >> buffs[i].buffId >> buffs[i].remainTime;
	//	br >> buffs[i];
	//}
	PKT_S_TEST::BuffsList buffs = pkt->GetBuffsList();

	cout << "BufCount : " << buffs.Count() << endl;
	//for (int32 i = 0; i < buffs.Count(); i++)
	//{
	//	cout << "BufInfo : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	//}

	//for (auto it = buffs.begin(); it != buffs.end(); ++it)
	//{
	//	cout << "BufInfo : " << it->buffId << " " << it->remainTime << endl;
	//}

	for (auto& buff : buffs)
	{
		cout << "BufInfo : " << buff.buffId << " " << buff.remainTime << endl;

		PKT_S_TEST::BuffsVictimsList victims = pkt->GetBuffsVictimList(&buff);

		cout << "Victim Count : " << victims.Count() << endl;

		for (auto& victim : victims)
		{
			cout << "Victim : " << victim << endl;
		}
	}

	//wstring name;
	//uint16 nameLen;
	//br >> nameLen;
	//name.resize(nameLen);

	//br.Read((void*)name.data(), nameLen * sizeof(WCHAR));

	//wcout.imbue(std::locale("kor"));
	//wcout << name << endl;
}

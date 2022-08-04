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
// ��Ŷ ���� TEMP
// [ PKT_S_TEST ][ BuffsListItem BUffsListItem BuffsListItem ]
struct PKT_S_TEST
{
	struct BuffsListItem
	{
		uint64 buffId;
		float remainTime;
	};

	uint16 packetSize; // ���� ���
	uint16 packetId; // ���� ���
	uint64 id;		// 8
	uint32 hp;		// 4
	uint16 attack;	// 2
	//// ���� ������
	//// 1) ���ڿ� (ex. name)
	//// 2) �׳� ����Ʈ �迭 (ex. ��� �̹���)
	//// 3) �Ϲ� ����Ʈ
	//vector<BuffData> buffs;
	uint16 buffsOffset;	// ���������͸� ���� ��1: ���������� ���� ��ġ
	uint16 buffsCount;	// ���������͸� ���� ��1: ���������� ����

	//wstring name;

	bool Validate()
	{
		uint32 size = 0;
		size += sizeof(PKT_S_TEST);
		if (packetSize < size)
			return false;

		size += buffsCount * sizeof(BuffsListItem);
		if (size != packetSize)
			return false;

		if (buffsOffset + buffsCount * sizeof(BuffsListItem) > packetSize)
			return false;

		return true;
	}

	using BuffsList = PacketList<PKT_S_TEST::BuffsListItem>;

	BuffsList GetBuffsList()
	{
		BYTE* data = reinterpret_cast<BYTE*>(this);
		data += buffsOffset;
		return BuffsList(reinterpret_cast<PKT_S_TEST::BuffsListItem*>(data), buffsCount);
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
	for (int32 i = 0; i < buffs.Count(); i++)
	{
		cout << "BufInfo : " << buffs[i].buffId << " " << buffs[i].remainTime << endl;
	}

	//wstring name;
	//uint16 nameLen;
	//br >> nameLen;
	//name.resize(nameLen);

	//br.Read((void*)name.data(), nameLen * sizeof(WCHAR));

	//wcout.imbue(std::locale("kor"));
	//wcout << name << endl;
}

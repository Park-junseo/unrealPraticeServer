#include "pch.h"
#include "GameSession.h"
#include "GameSessionManager.h"

void GameSession::OnConnected()
{
	GSessionManager.Add(static_pointer_cast<GameSession>(shared_from_this()));
}

void GameSession::OnDisconnected()
{
	GSessionManager.Remove(static_pointer_cast<GameSession>(shared_from_this()));
}

int32 GameSession::OnRecv(BYTE* buffer, int32 len)
{
	//// Echo
	//cout << "OnRecv Len = " << len << endl;
	//Send(buffer, len);
	//return len;

	//// Echo
	//SendBufferRef sendBuffer = MakeShared<SendBuffer>(4096);
	//sendBuffer->CopyData(buffer, len);

	// Echo
	cout << "OnRecv Len = " << len << endl;

	SendBufferRef sendBuffer = GSendBufferManager->Open(4096);
	::memcpy(sendBuffer->Buffer(), buffer, len);
	sendBuffer->Close(len);
	
	//Send(sendBuffer);
	// BroadCast·Î º¯°æ
	GSessionManager.Broadcast(sendBuffer);
	return len;
}

void GameSession::OnSend(int32 len)
{
	cout << "OnSend Len = " << len << endl;
}
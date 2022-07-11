#include "pch.h"
#include "LAccountManager.h"
#include "PlayerManager.h"

LAccountManager GAccountManager;

void LAccountManager::AccountThenPlayer()
{
	WRITE_LOCK;
	GPlayerManager.Lock();
}

void LAccountManager::Lock()
{
	WRITE_LOCK;
}
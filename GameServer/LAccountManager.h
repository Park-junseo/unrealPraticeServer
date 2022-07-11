#pragma once
class LAccountManager
{
	USE_LOCK;

public:
	void AccountThenPlayer();
	void Lock();
};

extern LAccountManager GAccountManager;


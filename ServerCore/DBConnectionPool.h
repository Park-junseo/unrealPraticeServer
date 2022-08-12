#pragma once
#include "DBConnection.h"

/*----------------------
	DBConnectionPool
----------------------*/

class DBConnectionPool
{
public:
	DBConnectionPool();
	~DBConnectionPool();

	bool					Connect(int32 connectionCount, const WCHAR* connectionString);
	void					Clear();

	DBConnection*			Pop();
	void					Push(DBConnection* connection);

private:
	USE_LOCK;
	SQLHENV					_enviroment = SQL_NULL_HANDLE;
	xVector<DBConnection*>	_connections;
};


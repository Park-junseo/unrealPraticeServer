#pragma once
#include <sql.h>
#include <sqlext.h>

class DBConnection
{
public:
	bool Connect(SQLHENV env, const WCHAR* connectionString);
};


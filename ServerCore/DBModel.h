#pragma once
#include "Types.h"

NAMESPACE_BEGIN(DBModel)

USING_SHARED_PTR(Column);
USING_SHARED_PTR(Index);
USING_SHARED_PTR(Table);
USING_SHARED_PTR(Procedure);

/*-------------
	DataType
--------------*/

enum class DataType
{
	None = 0,
	TinyInt = 48,
	SmallInt = 52,
	Int = 56,
	Real = 59,
	DateTime = 61,
	Float = 62,
	Bit = 104,
	Numeric = 108,
	BigInt = 127,
	VarBinary = 165,
	Varchar = 167,
	Binary = 173,
	NVarChar = 231,
};

/*-------------
	Column
--------------*/

class Column
{
public:
	xString				CreateText();

public:
	xString				_name;
	int32				_columnId = 0; // DB
	DataType			_type = DataType::None;
	xString				_typeText;
	int32				_maxLength = 0;
	bool				_nullable = false;
	bool				_identity = false;
	int64				_seedValue = 0;
	int64				_incrementValue = 0;
	xString				_default;
	xString				_defaultConstraintName; // DB
};

/*-----------
	Index
------------*/

enum class IndexType
{
	Clustered = 1,
	NonClustered = 2
};

class Index
{
public:
	xString				GetUniqueName();
	xString				CreateName(const xString& tableName);
	xString				GetTypeText();
	xString				GetKeyText();
	xString				CreateColumnsText();
	bool				DependsOn(const xString& columnName);

public:
	xString				_name; // DB
	int32				_indexId = 0; // DB
	IndexType			_type = IndexType::NonClustered;
	bool				_primaryKey = false;
	bool				_uniqueConstraint = false;
	xVector<ColumnRef>	_columns;
};

/*-----------
	Table
------------*/

class Table
{
public:
	ColumnRef			FindColumn(const xString& columnName);

public:
	int32				_objectId = 0; // DB
	xString				_name;
	xVector<ColumnRef>	_columns;
	xVector<IndexRef>	_indexes;
};

/*----------------
	Procedures
-----------------*/

struct Param
{
	xString				_name;
	xString				_type;
};

class Procedure
{
public:
	xString				GenerateCreateQuery();
	xString				GenerateAlterQuery();
	xString				GenerateParamString();

public:
	xString				_name;
	xString				_fullBody; // DB
	xString				_body; // XML
	xVector<Param>		_parameters;  // XML
};

/*-------------
	Helpers
--------------*/

class Helpers
{
public:
	static xString		Format(const WCHAR* format, ...);
	static xString		DataType2String(DataType type);
	static xString		RemoveWhiteSpace(const xString& str);
	static DataType		String2DataType(const WCHAR* str, OUT int32& maxLen);
};

NAMESPACE_END
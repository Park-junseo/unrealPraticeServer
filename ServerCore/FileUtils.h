#pragma once
#include <vector>
#include "Types.h"

/*-----------------
	FileUtils
------------------*/

class FileUtils
{
public:
	static xVector<BYTE>		ReadFile(const WCHAR* path);
	static xString				Convert(string str);
};
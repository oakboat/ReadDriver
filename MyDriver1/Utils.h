#pragma once
#include "LazyImport.h"

namespace Utils
{
	void* GetSystemModule(const char* moduleName);
	void* GetSystemModuleExport(const char* moduleName, const char* functionName);
};


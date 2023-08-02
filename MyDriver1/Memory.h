#pragma once
#include "LazyImport.h"

namespace Memory
{
	int ReadMemory(PEPROCESS target_process, void* source, void* target, size_t size);
	int WriteMemory(PEPROCESS target_process, void* source, void* target, size_t size);
};


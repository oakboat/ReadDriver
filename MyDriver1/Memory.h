#pragma once
#include "def.h"

namespace Memory
{
	bool ReadMemory(PEPROCESS target_process, void* source, void* target, size_t size);
	bool WriteMemory(PEPROCESS target_process, void* source, void* target, size_t size);
};


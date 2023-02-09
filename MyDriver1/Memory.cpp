#include "Memory.h"

namespace Memory
{
	bool ReadMemory(PEPROCESS process, void* source, void* target, size_t size)
	{
		if (!process)
		{
			return false;
		}

		size_t bytes = 0;
		NTSTATUS status = MmCopyVirtualMemory(process, source, IoGetCurrentProcess(), target, size, KernelMode, &bytes);
		if (!NT_SUCCESS(status) || !bytes)
		{
			return false;
		}
		return true;
	}
	bool WriteMemory(PEPROCESS target_process, void* source, void* target, size_t size)
	{
		if (!target_process)
		{
			return false;
		}

		size_t bytes = 0;
		NTSTATUS status = MmCopyVirtualMemory(IoGetCurrentProcess(), source, target_process, target, size, KernelMode, &bytes);
		if (!NT_SUCCESS(status) || !bytes)
		{
			return false;
		}
		return true;
	}
}
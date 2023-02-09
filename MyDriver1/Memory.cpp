#include "Memory.h"

namespace Memory
{
	int ReadMemory(PEPROCESS process, void* source, void* target, size_t size)
	{
		if (!process)
		{
			return 0;
		}

		size_t bytes = 0;
		NTSTATUS status = LazyImport::MmCopyVirtualMemory(process, source, IoGetCurrentProcess(), target, size, KernelMode, &bytes);
		if (!NT_SUCCESS(status) || !bytes)
		{
			return 0;
		}
		return 1;
	}

	int WriteMemory(PEPROCESS target_process, void* source, void* target, size_t size)
	{
		if (!target_process)
		{
			return 0;
		}

		size_t bytes = 0;
		NTSTATUS status = LazyImport::MmCopyVirtualMemory(IoGetCurrentProcess(), source, target_process, target, size, KernelMode, &bytes);
		if (!NT_SUCCESS(status) || !bytes)
		{
			return 0;
		}
		return 1;
	}
}
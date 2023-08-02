#include "Utils.h"

namespace Utils
{
	void * GetSystemModule(const char * moduleName)
	{
		NTSTATUS status = STATUS_UNSUCCESSFUL;
		ULONG size = 0;
		PRTL_PROCESS_MODULES modules = NULL;
		void* addresss = 0;
		LazyImport::ZwQuerySystemInformation(SystemModuleInformation, 0, size, &size);
		if (!size)
		{
			KdPrint(("Cant Get Size"));
			return 0;
		}
		modules = reinterpret_cast<PRTL_PROCESS_MODULES>(ExAllocatePool(NonPagedPool, size));
		if (!modules)
		{
			KdPrint(("Cant Get Allocate Modules"));
			return 0;
		}
		RtlZeroMemory(modules, size);
		status = LazyImport::ZwQuerySystemInformation(SystemModuleInformation, modules, size, &size);
		if (!NT_SUCCESS(status))
		{
			KdPrint(("Cant Get Get Modules"));
			ExFreePool(modules);
			return 0;
		}
		PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
		if (!module)
		{
			KdPrint(("Cant Get Get Module"));
			ExFreePool(modules);
			return 0;
		}
		for (ULONG i = 0; i < modules->NumberOfModules; i++)
		{
			if (strcmp(reinterpret_cast<char*>(module[i].FullPathName) + module[i].OffsetToFileName, moduleName) == 0)
			{
				addresss = module[i].ImageBase;
				break;
			}
		}
		ExFreePool(modules);
		return addresss;
	}
	void* GetSystemModuleExport(const char* moduleName, const char* functionName)
	{
		void* module = GetSystemModule(moduleName);
		if (!module)
		{
			KdPrint(("Cant Get Module"));
		}
		return LazyImport::RtlFindExportedRoutineByName(module, functionName);
	}
	void* FindPattern(char* base, size_t size, const char* pattern, const char* mask)
	{
		size_t PatternLength = strlen(mask);
		for (size_t i = 0; i < size - PatternLength; i++)
		{
			bool found = true;
			for (size_t j = 0; j < PatternLength; j++)
			{
				if (mask[j] != '?' && base[i + j] != pattern[j])
				{
					found = false;
				}
			}
			if (found)
			{
				return base + i;
			}
		}
	}
}
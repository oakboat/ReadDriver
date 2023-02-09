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
			dbg("Cant Get Size");
			return 0;
		}
		modules = (PRTL_PROCESS_MODULES)ExAllocatePool(NonPagedPool, size);
		if (!modules)
		{
			dbg("Cant Get Allocate Modules");
			return 0;
		}
		RtlZeroMemory(modules, size);
		status = LazyImport::ZwQuerySystemInformation(SystemModuleInformation, modules, size, &size);
		if (!NT_SUCCESS(status))
		{
			dbg("Cant Get Get Modules");
			ExFreePool(modules);
			return 0;
		}
		PRTL_PROCESS_MODULE_INFORMATION module = modules->Modules;
		if (!module)
		{
			dbg("Cant Get Get Module");
			ExFreePool(modules);
			return 0;
		}
		for (ULONG i = 0; i < modules->NumberOfModules; i++)
		{
			if (strcmp((char*)module[i].FullPathName + module[i].OffsetToFileName, moduleName) == 0)
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
			dbg("Cant Get Module");
		}
		return LazyImport::RtlFindExportedRoutineByName(module, functionName);
	}
}
#include "LazyImport.h"

namespace LazyImport
{
	ZwQuerySystemInformation_t ZwQuerySystemInformation = nullptr;
	RtlFindExportedRoutineByName_t RtlFindExportedRoutineByName = nullptr;
	PsGetProcessPeb_t PsGetProcessPeb = nullptr;
	PsGetProcessWow64Process_t PsGetProcessWow64Process = nullptr;
	MmCopyVirtualMemory_t MmCopyVirtualMemory = nullptr;

	int Init()
	{
		UNICODE_STRING RoutineName;
		RoutineName = RTL_CONSTANT_STRING(L"ZwQuerySystemInformation");
		ZwQuerySystemInformation = reinterpret_cast<ZwQuerySystemInformation_t>(MmGetSystemRoutineAddress(&RoutineName));
		if (!ZwQuerySystemInformation)
		{
			KdPrint(("Error load ZwQuerySystemInformation"));
			return 0;
		}
		RoutineName = RTL_CONSTANT_STRING(L"RtlFindExportedRoutineByName");
		RtlFindExportedRoutineByName = reinterpret_cast<RtlFindExportedRoutineByName_t>(MmGetSystemRoutineAddress(&RoutineName));
		if (!RtlFindExportedRoutineByName)
		{
			KdPrint(("Error load RtlFindExportedRoutineByName"));
			return 0;
		}

		RoutineName = RTL_CONSTANT_STRING(L"PsGetProcessPeb");
		PsGetProcessPeb = reinterpret_cast<PsGetProcessPeb_t>(MmGetSystemRoutineAddress(&RoutineName));
		if (!PsGetProcessPeb)
		{
			KdPrint(("Error load PsGetProcessPeb"));
			return 0;
		}
		RoutineName = RTL_CONSTANT_STRING(L"PsGetProcessWow64Process");
		PsGetProcessWow64Process = reinterpret_cast<PsGetProcessWow64Process_t>(MmGetSystemRoutineAddress(&RoutineName));
		if (!PsGetProcessWow64Process)
		{
			KdPrint(("Error load PsGetProcessWow64Process"));
			return 0;
		}
		RoutineName = RTL_CONSTANT_STRING(L"MmCopyVirtualMemory");
		MmCopyVirtualMemory = reinterpret_cast<MmCopyVirtualMemory_t>(MmGetSystemRoutineAddress(&RoutineName));
		if (!MmCopyVirtualMemory)
		{
			KdPrint(("Error load MmCopyVirtualMemory"));
			return 0;
		}
		return 1;
	}
}


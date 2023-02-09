#pragma once
#include <ntifs.h>
#include "def.h"

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation = 0,
	SystemProcessorInformation = 1,
	SystemPerformanceInformation = 2,
	SystemTimeOfDayInformation = 3,
	SystemPathInformation = 4,
	SystemProcessInformation = 5,
	SystemCallCountInformation = 6,
	SystemDeviceInformation = 7,
	SystemProcessorPerformanceInformation = 8,
	SystemFlagsInformation = 9,
	SystemCallTimeInformation = 10,
	SystemModuleInformation = 11,
} SYSTEM_INFORMATION_CLASS, * PSYSTEM_INFORMATION_CLASS;

typedef struct _RTL_PROCESS_MODULE_INFORMATION
{
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT OffsetToFileName;
	UCHAR FullPathName[MAXIMUM_FILENAME_LENGTH];
} RTL_PROCESS_MODULE_INFORMATION, * PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES
{
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, * PRTL_PROCESS_MODULES;

namespace LazyImport
{
	using ZwQuerySystemInformation_t =
		NTSTATUS
		(*)
		(
			ULONG InfoClass,
			PVOID Buffer,
			ULONG Length,
			PULONG ReturnLength
			);
	using RtlFindExportedRoutineByName_t =
		PVOID
		(NTAPI*)
		(
			PVOID ImageBase,
			PCCH RoutineName
			);

	using GetSystemModuleExport_t =
		NTSTATUS
		(NTAPI*)(
			_In_ PEPROCESS FromProcess,
			_In_ PVOID FromAddress,
			_In_ PEPROCESS ToProcess,
			_Out_ PVOID ToAddress,
			_In_ SIZE_T BufferSize,
			_In_ KPROCESSOR_MODE PreviousMode,
			_Out_ PSIZE_T NumberOfBytesCopied
			);

	using PsGetProcessPeb_t =
		PPEB
		(NTAPI*)
		(
			_In_ PEPROCESS Process
			);

	using PsGetProcessWow64Process_t =
		PVOID
		(NTAPI*)
		(
			_In_ PEPROCESS Process
			);

	using MmCopyVirtualMemory_t =
		NTSTATUS
		(NTAPI*)
		(
			_In_ PEPROCESS FromProcess,
			_In_ PVOID FromAddress,
			_In_ PEPROCESS ToProcess,
			_Out_ PVOID ToAddress,
			_In_ SIZE_T BufferSize,
			_In_ KPROCESSOR_MODE PreviousMode,
			_Out_ PSIZE_T NumberOfBytesCopied
			);
	
	extern ZwQuerySystemInformation_t ZwQuerySystemInformation;
	extern RtlFindExportedRoutineByName_t RtlFindExportedRoutineByName;
	extern PsGetProcessPeb_t PsGetProcessPeb;
	extern PsGetProcessWow64Process_t PsGetProcessWow64Process;
	extern MmCopyVirtualMemory_t MmCopyVirtualMemory;
	int Init();
};
#pragma once
#include <ntifs.h>
#ifdef DEBUG
#define dbg(fmt, ...) DbgPrint(fmt, __VA_ARGS__)
#else
#define dbg(fmt, ...) 
#endif

constexpr auto GET_PROCESS = 1;
constexpr auto GET_PROCESS_PEB32 = 2;
constexpr auto GET_PROCESS_PEB = 3;
constexpr auto READ_BUFFER = 4;
constexpr auto WRITE_BUFFER = 5;

typedef struct _WDATA
{
	__int64 operation;
	__int64 pid;
	__int64 address;
	__int64 size;
	void* buffer;
}WDATA, * PWDATA;

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


extern "C"
NTKERNELAPI
NTSTATUS
ZwQuerySystemInformation(
	ULONG InfoClass,
	PVOID Buffer,
	ULONG Length,
	PULONG ReturnLength
);

extern "C"
NTKERNELAPI
PVOID
NTAPI
RtlFindExportedRoutineByName(
	PVOID ImageBase,
	PCCH RoutineName
);

extern "C"
NTKERNELAPI
PPEB
NTAPI
PsGetProcessPeb(_In_ PEPROCESS Process);

extern "C"
NTKERNELAPI
PVOID
NTAPI
PsGetProcessWow64Process(_In_ PEPROCESS Process);

extern "C"
NTKERNELAPI
NTSTATUS
NTAPI
MmCopyVirtualMemory(
	_In_ PEPROCESS FromProcess,
	_In_ PVOID FromAddress,
	_In_ PEPROCESS ToProcess,
	_Out_ PVOID ToAddress,
	_In_ SIZE_T BufferSize,
	_In_ KPROCESSOR_MODE PreviousMode,
	_Out_ PSIZE_T NumberOfBytesCopied
);
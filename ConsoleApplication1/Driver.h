#pragma once
#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

typedef struct _UNICODE_STRING32
{
    USHORT Length;
    USHORT MaximumLength;
    ULONG Buffer;
} UNICODE_STRING32, * PUNICODE_STRING32;

typedef struct _PEB32
{
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    UCHAR BitField;
    ULONG Mutant;
    ULONG ImageBaseAddress;
    ULONG Ldr;
    ULONG ProcessParameters;
    ULONG SubSystemData;
    ULONG ProcessHeap;
    ULONG FastPebLock;
    ULONG AtlThunkSListPtr;
    ULONG IFEOKey;
    ULONG CrossProcessFlags;
    ULONG UserSharedInfoPtr;
    ULONG SystemReserved;
    ULONG AtlThunkSListPtr32;
    ULONG ApiSetMap;
} PEB32, * PPEB32;

typedef struct _PEB_LDR_DATA32
{
    ULONG Length;
    BOOLEAN Initialized;
    ULONG SsHandle;
    LIST_ENTRY32 InLoadOrderModuleList;
    LIST_ENTRY32 InMemoryOrderModuleList;
    LIST_ENTRY32 InInitializationOrderModuleList;
    ULONG EntryInProgress;
} PEB_LDR_DATA32, * PPEB_LDR_DATA32;

typedef struct _LDR_DATA_TABLE_ENTRY32
{
    LIST_ENTRY32 InLoadOrderLinks;
    LIST_ENTRY32 InMemoryOrderModuleList;
    LIST_ENTRY32 InInitializationOrderModuleList;
    ULONG DllBase;
    ULONG EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING32 FullDllName;
    UNICODE_STRING32 BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union
    {
        LIST_ENTRY32 HashLinks;
        ULONG SectionPointer;
    };
    ULONG CheckSum;
    union
    {
        ULONG TimeDateStamp;
        ULONG LoadedImports;
    };
    ULONG EntryPointActivationContext;
    ULONG PatchInformation;
} LDR_DATA_TABLE_ENTRY32, * PLDR_DATA_TABLE_ENTRY32;

typedef struct _PEB64
{
    UCHAR InheritedAddressSpace;
    UCHAR ReadImageFileExecOptions;
    UCHAR BeingDebugged;
    UCHAR BitField;
    ULONG64 Mutant;
    ULONG64 ImageBaseAddress;
    ULONG64 Ldr;
    ULONG64 ProcessParameters;
    ULONG64 SubSystemData;
    ULONG64 ProcessHeap;
    ULONG64 FastPebLock;
    ULONG64 AtlThunkSListPtr;
    ULONG64 IFEOKey;
    ULONG64 CrossProcessFlags;
    ULONG64 UserSharedInfoPtr;
    ULONG SystemReserved;
    ULONG AtlThunkSListPtr32;
    ULONG64 ApiSetMap;
} PEB64, * PPEB64;

typedef struct _PEB_LDR_DATA64
{
    ULONG Length;
    BOOLEAN Initialized;
    ULONG64 SsHandle;
    LIST_ENTRY64 InLoadOrderModuleList;
    LIST_ENTRY64 InMemoryOrderModuleList;
    LIST_ENTRY64 InInitializationOrderModuleList;
    ULONG64 EntryInProgress;
} PEB_LDR_DATA64, * PPEB_LDR_DATA64;

typedef struct _UNICODE_STRING64
{
    USHORT Length;
    USHORT MaximumLength;
    ULONG64 Buffer;
} UNICODE_STRING64, * PUNICODE_STRING64;

typedef struct _LDR_DATA_TABLE_ENTRY64
{
    LIST_ENTRY64 InLoadOrderLinks;
    LIST_ENTRY64 InMemoryOrderModuleList;
    LIST_ENTRY64 InInitializationOrderModuleList;
    ULONG64 DllBase;
    ULONG64 EntryPoint;
    ULONG SizeOfImage;
    UNICODE_STRING64 FullDllName;
    UNICODE_STRING64 BaseDllName;
    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    union
    {
        LIST_ENTRY64 HashLinks;
        ULONG64 SectionPointer;
    };
    ULONG CheckSum;
    union
    {
        ULONG TimeDateStamp;
        ULONG64 LoadedImports;
    };
    ULONG64 EntryPointActivationContext;
    ULONG64 PatchInformation;
} LDR_DATA_TABLE_ENTRY64, * PLDR_DATA_TABLE_ENTRY64;

static HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

inline void print(LPCTSTR fmt, ...)
{
	TCHAR buffer[100];
	va_list ap;
	va_start(ap, fmt);
	wvsprintf(buffer, fmt, ap);
	WriteConsole(hOut, buffer, lstrlen(buffer), nullptr, nullptr);
	va_end(ap);
}

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

typedef __int64(__fastcall* Comm_t)(__int64 data);

class Driver
{
public:
	Comm_t comm = nullptr;
	uint64_t pid = 0;
	uint64_t process = 0;
	bool Init(uint64_t pid);
	bool Call(PWDATA data);
	uint64_t GetModuleAddress(const wchar_t* moduleName);
    uint64_t GetModuleAddress32(const wchar_t* moduleName);
	template<class T>
	T Read(uint64_t address);
	template<class T>
	bool Write(uint64_t address, T data);
	bool ReadBuffer(uint64_t address, void* bufefr, size_t size);
	bool WriteBuffer(uint64_t address, void* bufefr, size_t size);
};

inline bool Driver::Call(PWDATA data)
{
    __int64 mask = ((__int64)rand() % 0xff + 0x1) << 56;
	return comm((__int64)data | mask);
}

template<class T>
inline T Driver::Read(uint64_t address)
{
    T buffer{ 0 };
	WDATA w{ 0 };
	w.operation = READ_BUFFER;
	w.pid = process;
	w.address = address;
	w.size = sizeof(buffer);
	w.buffer = &buffer;

	if (!Call(&w))
	{
		print(TEXT("��ȡʧ��\n"));
	}
	return buffer;
}

template<class T>
inline bool Driver::Write(uint64_t address, T data)
{
	WDATA w{ 0 };
	w.operation = WRITE_BUFFER;
	w.pid = process;
	w.address = address;
	w.size = sizeof(data);
	w.buffer = &data;
	if (!Call(&w))
	{
		print(TEXT("д��ʧ��\n"));
		return false;
	}
	return true;
}
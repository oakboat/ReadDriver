#pragma once
#include "def.h"
#include "LazyImport.h"
#include "Memory.h"
#include <intrin.h>
#include <ntifs.h>

namespace Hook
{
	constexpr auto GET_PROCESS = 1;
	constexpr auto GET_PROCESS_PEB32 = 2;
	constexpr auto GET_PROCESS_PEB = 3;
	constexpr auto READ_BUFFER = 4;
	constexpr auto WRITE_BUFFER = 5;

	typedef struct _WDATA
	{
		__int64 operation;
		union {
			__int64 pid;
			__int64 process;
		};
		__int64 address;
		__int64 size;
		void* buffer;
	}WDATA, * PWDATA;

	NTSTATUS StartHook(void** HookAddress);
	__int64 __fastcall hooked_function(__int64 data);
	__int64 Handler(__int64 data);
	using oFunction_t = __int64(__fastcall*) ();
};
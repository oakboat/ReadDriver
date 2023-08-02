#pragma once
#include "LazyImport.h"
#include "Memory.h"
#include <intrin.h>
#include <ntifs.h>
#include <cstdint>

namespace Hook
{
	constexpr auto MAGIC = 0x52ABC;
	constexpr auto SET_PROCESS = 1 * 0x52ABC;
	constexpr auto GET_PROCESS_PEB32 = 2 * 0x52ABC;
	constexpr auto GET_PROCESS_PEB = 3 * 0x52ABC;
	constexpr auto READ_BUFFER = 4 * 0x52ABC;
	constexpr auto WRITE_BUFFER = 5 * 0x52ABC;

	typedef struct _WDATA
	{
		uint64_t operation;
		union {
			uint64_t pid;
			uintptr_t address;
		};
		uint64_t size;
		void* buffer;
	}WDATA, * PWDATA;

	NTSTATUS StartHook(void** HookAddress);
	__int64 __fastcall hooked_function(__int64 data);
	__int64 Handler(__int64 data);
	using oFunction_t = __int64(__fastcall*) ();
};
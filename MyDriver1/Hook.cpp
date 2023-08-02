#include "Hook.h"

namespace Hook
{
	static oFunction_t oFunction = nullptr;
	static PEPROCESS process = nullptr;
	static uint64_t BaseAddress = 0;

	NTSTATUS StartHook(void** HookAddress)
	{
		oFunction = reinterpret_cast<oFunction_t>(InterlockedExchangePointer(HookAddress, &Hook::hooked_function));
		return STATUS_SUCCESS;
	}

	__int64 __fastcall hooked_function(__int64 data)
	{
		uintptr_t* override = (uintptr_t*)_AddressOfReturnAddress();
		uintptr_t OriginRet = *override;
		*override = 0xFFFF123456789ABC;
		__int64 result =  Handler(data);
		*override = OriginRet;
		return result;
	}

	__int64 Handler(__int64 _data)
	{
		if (_data != MAGIC)
		{
			if (_data !=0 && (_data & 0xffff000000000000) == 0 && MmIsAddressValid(reinterpret_cast<void*>(_data)))
			{
				KdPrint(("Get BaseAddress"));
				BaseAddress = _data;
			}
			return oFunction();
		}
		PWDATA data = reinterpret_cast<PWDATA>(BaseAddress);
		if (!MmIsAddressValid(data))
		{
			return 1;
		}
		switch (data->operation)
		{
		case SET_PROCESS:
		{
			KdPrint(("Set Process"));
			if (!data->pid)
			{
				return 1;
			}
			if (!NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(data->pid), &process)))
			{
				return 1;
			}
			ObReferenceObject(process);
		}
		break;
		case GET_PROCESS_PEB:
		{
			data->address = reinterpret_cast<uintptr_t>(LazyImport::PsGetProcessPeb(process));
			if (!data->address)
			{
				return 1;
			}
		}
		break;
		case GET_PROCESS_PEB32:
		{
			data->address = reinterpret_cast<uintptr_t>(LazyImport::PsGetProcessWow64Process(process));
			if (!data->address)
			{
				return 1;
			}
		}
		break;
		case READ_BUFFER:
		{
			if (!process)
			{
				return 1;
			}
			bool status = Memory::ReadMemory(process, reinterpret_cast<void*>(data->address), data->buffer, data->size);
			return !status;
		}
		break;
		case WRITE_BUFFER:
		{
			if (!process)
			{
				return 1;
			}
			bool status = Memory::WriteMemory(process, data->buffer, reinterpret_cast<void*>(data->address), data->size);
			return !status;
		}
		break;
		case GET_BASE_ADDR:
		{
			if (!process)
			{
				return 1;
			}
			data->address = reinterpret_cast<uintptr_t>(LazyImport::PsGetProcessSectionBaseAddress(process));
		}
		break;
		default:
			return 1;
			break;
		}
		return 0;
	}
}
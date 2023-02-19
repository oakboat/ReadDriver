#include "Hook.h"

namespace Hook
{
	oFunction_t g_oFunction;

	NTSTATUS StartHook(void** HookAddress)
	{
		g_oFunction = reinterpret_cast<oFunction_t>(InterlockedExchangePointer(HookAddress, &Hook::hooked_function));
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
		if ((_data & 0xff00000000000000) == 0)
		{
			return g_oFunction();
		}
		if ((_data & 0x00ff000000000000) != 0)
		{
			return g_oFunction();
		}
		_data &= 0x0000ffffffffffff;
		PWDATA data = reinterpret_cast<PWDATA>(_data);
		if (!MmIsAddressValid(data))
		{
			return 1;
		}
		PEPROCESS process = reinterpret_cast<PEPROCESS>(data->process);
		switch (data->operation)
		{
		case GET_PROCESS:
		{
			if (!NT_SUCCESS(PsLookupProcessByProcessId(reinterpret_cast<HANDLE>(data->pid), &process)))
			{
				return 1;
			}
			data->process = reinterpret_cast<uint64_t>(process);
			return 0;
		}
		break;
		case GET_PROCESS_PEB:
		{
			if (!data->process)
			{
				return 1;
			}
			data->address = reinterpret_cast<uintptr_t>(LazyImport::PsGetProcessPeb(process));
			if (!data->address)
			{
				return 1;
			}
			ObReferenceObject(process);
			return 0;
		}
		break;
		case GET_PROCESS_PEB32:
		{
			if (!data->process)
			{
				return 1;
			}
			data->address = reinterpret_cast<uintptr_t>(LazyImport::PsGetProcessWow64Process(process));
			if (!data->address)
			{
				return 1;
			}
			ObReferenceObject(process);
			return 0;
		}
		break;
		case READ_BUFFER:
		{
			if (!process)
			{
				return 1;
			}
			bool status = Memory::ReadMemory(process, reinterpret_cast<void*>(data->address), data->buffer, data->size);
			ObReferenceObject(process);
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
			ObReferenceObject(process);
			return !status;
		}
		break;
		default:
			break;
		}
		return 1;
	}
}
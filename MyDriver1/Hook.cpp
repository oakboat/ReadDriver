#include "Hook.h"
#include "Memory.h"
#include <intrin.h>

namespace Hook
{
	__int64 __fastcall hooked_function(__int64 data)
	{
		uintptr_t* override = (uintptr_t*)_AddressOfReturnAddress();
		uintptr_t originRet = *override;
		*override = 0xFFFF123456789ABC;
		__int64 result =  Handler(data);
		*override = originRet;
		return result;
	}

	__int64 Handler(__int64 _data)
	{
		if ((_data & 0xff00000000000000) == 0)
		{
			return 0;
		}
		if ((_data & 0x00ff000000000000) != 0)
		{
			return 0;
		}
		_data &= 0x0000ffffffffffff;
		PWDATA data = (PWDATA)(_data);
		dbg("%p\n", data);
		switch (data->operation)
		{
		case GET_PROCESS:
		{
			PEPROCESS process = nullptr;
			if (!NT_SUCCESS(PsLookupProcessByProcessId((HANDLE)data->pid, &process)))
			{
				dbg("Cant Get Process");
				return 0;
			}
			data->pid = (__int64)process;
			return 1;
		}
		case GET_PROCESS_PEB:
		{
			if (!data->pid)
			{
				return 0;
			}
			data->address = (__int64)PsGetProcessPeb((PEPROCESS)data->pid);
			if (!data->address)
			{
				dbg("Cant Get Peb\n");
				return 0;
			}
			ObReferenceObject((PEPROCESS)data->pid);
			return 1;
		}
		case GET_PROCESS_PEB32:
		{
			if (!data->pid)
			{
				return 0;
			}
			data->address = (__int64)PsGetProcessWow64Process((PEPROCESS)data->pid);
			if (!data->address)
			{
				dbg("Cant Get Peb32\n");
				return 0;
			}
			ObReferenceObject((PEPROCESS)data->pid);
			return 1;
		}
		case READ_BUFFER:
		{
			if (!data->pid)
			{
				return 0;
			}
			bool status = Memory::ReadMemory((PEPROCESS)data->pid, (void*)data->address, data->buffer, data->size);
			ObReferenceObject((PEPROCESS)data->pid);
			return status;
		}
		case WRITE_BUFFER:
		{
			if (!data->pid)
			{
				return 0;
			}
			bool status = Memory::WriteMemory((PEPROCESS)data->pid, data->buffer, (void*)data->address, data->size);
			ObReferenceObject((PEPROCESS)data->pid);
			return status;
		}
		default:
			break;
		}
		return 0;
	}
}
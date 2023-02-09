#include "Driver.h"
#include <stddef.h>

bool Driver::Init(uint64_t pid)
{
	LoadLibraryA("user32.dll");
	LoadLibraryA("win32u.dll");
	comm = (Comm_t)GetProcAddress(GetModuleHandleA("win32u.dll"), "NtUserRegisterTouchPadCapable");
	if (!comm)
	{
		print(TEXT("不能获取函数\n"));
		return false;
	}
	this->pid = pid;
	WDATA w{ 0 };
	w.operation = GET_PROCESS;
	w.pid = pid;
	if (!Call(&w))
	{
		print(TEXT("设置进程失败\n"));
		return false;
	}
	process = w.pid;
	srand(time(nullptr));
	return true;
}

uint64_t Driver::GetModuleAddress(const wchar_t* moduleName)
{
	WDATA w{ 0 };
	w.operation = GET_PROCESS_PEB;
	w.pid = process;
	if (!Call(&w))
	{
		print(TEXT("获取PEB失败\n"));
		return 0;
	}

	PEB64 peb{ 0 };
	ReadBuffer(w.address, &peb, sizeof(peb));

	PEB_LDR_DATA64 ldr{ 0 };
	ReadBuffer((uintptr_t)peb.Ldr, &ldr, sizeof(ldr));

	PLDR_DATA_TABLE_ENTRY64 pModEntry{ 0 };
	for (PLIST_ENTRY pCur = (PLIST_ENTRY)ldr.InMemoryOrderModuleList.Flink; (uint8_t*)pCur != (uint8_t*)peb.Ldr + offsetof(PEB_LDR_DATA64, InMemoryOrderModuleList); pCur = (PLIST_ENTRY)pModEntry->InMemoryOrderModuleList.Flink)
	{
		LDR_DATA_TABLE_ENTRY64 curData{ 0 };
		ReadBuffer((uintptr_t)pCur, &curData, sizeof(curData));

		pModEntry = CONTAINING_RECORD(&curData, LDR_DATA_TABLE_ENTRY64, InMemoryOrderModuleList);
		if (pModEntry->BaseDllName.Buffer)
		{
			wchar_t wszBuff[260]{ 0 };
			ReadBuffer((uintptr_t)pModEntry->BaseDllName.Buffer, wszBuff, (SIZE_T)(pModEntry->BaseDllName.Length + 2));

			//printf("%p: %S\n", pModEntry->DllBase, wszBuff);

			if (_wcsicmp(moduleName, wszBuff) == 0)
			{
				return (uintptr_t)pModEntry->DllBase;
			}
		}
	}
	return 0;
}

uint64_t Driver::GetModuleAddress32(const wchar_t* moduleName)
{
	WDATA w{ 0 };
	w.operation = GET_PROCESS_PEB32;
	w.pid = process;
	if (!Call(&w))
	{
		print(TEXT("获取PEB32失败\n"));
		return 0;
	}

	PEB32 peb{ 0 };
	ReadBuffer(w.address, &peb, sizeof(peb));

	PEB_LDR_DATA32 ldr{ 0 };
	ReadBuffer((uintptr_t)peb.Ldr, &ldr, sizeof(ldr));

	PLDR_DATA_TABLE_ENTRY32 pModEntry{ 0 };
	for (PLIST_ENTRY32 pCur = (PLIST_ENTRY32)ldr.InMemoryOrderModuleList.Flink; (uint8_t*)pCur != (uint8_t*)peb.Ldr + offsetof(PEB_LDR_DATA32, InMemoryOrderModuleList); pCur = (PLIST_ENTRY32)pModEntry->InMemoryOrderModuleList.Flink)
	{
		LDR_DATA_TABLE_ENTRY32 curData{ 0 };
		ReadBuffer((uintptr_t)pCur, &curData, sizeof(curData));

		pModEntry = CONTAINING_RECORD(&curData, LDR_DATA_TABLE_ENTRY32, InMemoryOrderModuleList);
		if (pModEntry->BaseDllName.Buffer)
		{
			wchar_t wszBuff[260]{ 0 };
			ReadBuffer((uintptr_t)pModEntry->BaseDllName.Buffer, wszBuff, (SIZE_T)(pModEntry->BaseDllName.Length + 2));

			//printf("%p: %S\n", pModEntry->DllBase, wszBuff);

			if (_wcsicmp(moduleName, wszBuff) == 0)
			{
				return (uintptr_t)pModEntry->DllBase;
			}
		}
	}
	return 0;
}

bool Driver::ReadBuffer(uint64_t address, void* bufefr, size_t size)
{
	WDATA w{ 0 };
	w.operation = READ_BUFFER;
	w.pid = process;
	w.address = address;
	w.size = size;
	w.buffer = bufefr;
	if (!Call(&w))
	{
		print(TEXT("读取失败\n"));
		return false;
	}
	return true;
}

bool Driver::WriteBuffer(uint64_t address, void* bufefr, size_t size)
{
	WDATA w{ 0 };
	w.operation = WRITE_BUFFER;
	w.pid = process;
	w.address = address;
	w.size = size;
	w.buffer = bufefr;
	if (!Call(&w))
	{
		print(TEXT("写入失败\n"));
		return false;
	}
	return true;
}

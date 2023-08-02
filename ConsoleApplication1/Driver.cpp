#include "Driver.h"

DriverDef::WDATA Driver::wBuf{0};
std::mutex Driver::mtx;

bool Driver::Init(uint64_t pid)
{
	auto library1 = skCrypt("user32.dll");
	auto library2 = skCrypt("win32u.dll");
	LoadLibraryA(library1);
	LoadLibraryA(library2);
	library1.clear();
	HMODULE module = GetModuleHandleA(library2);
	if (!module)
	{
		return false;
	}
	library2.clear();
	auto FunctionName = skCrypt("NtUserRegisterTouchPadCapable");
	comm = reinterpret_cast<DriverDef::Comm_t>(GetProcAddress(module, FunctionName));
	FunctionName.clear();
	if (!comm)
	{
		printf("不能获取函数\n");
		return false;
	}
	this->pid = pid;
	comm(reinterpret_cast<__int64>(&wBuf));
	DriverDef::WDATA w{ 0 };
	w.operation = DriverDef::SET_PROCESS;
	w.pid = pid;
	if (!Call(w))
	{
		printf("设置进程失败\n");
		return false;
	}
	return true;
}

uintptr_t Driver::GetModuleAddress(const wchar_t* moduleName)
{
	DriverDef::WDATA w{ 0 };
	w.operation = DriverDef::GET_PROCESS_PEB;
	if (!Call(w))
	{
		printf("获取PEB失败\n");
		return 0;
	}

	DriverDef::PEB64 peb{ 0 };
	ReadBuffer(w.address, &peb, sizeof(peb));

	DriverDef::PEB_LDR_DATA64 ldr{ 0 };
	ReadBuffer((uintptr_t)peb.Ldr, &ldr, sizeof(ldr));

	DriverDef::PLDR_DATA_TABLE_ENTRY64 pModEntry{ 0 };
	for (PLIST_ENTRY pCur = (PLIST_ENTRY)ldr.InMemoryOrderModuleList.Flink; (uint8_t*)pCur != (uint8_t*)peb.Ldr + offsetof(DriverDef::PEB_LDR_DATA64, InMemoryOrderModuleList); pCur = (PLIST_ENTRY)pModEntry->InMemoryOrderModuleList.Flink)
	{
		DriverDef::LDR_DATA_TABLE_ENTRY64 curData{ 0 };
		ReadBuffer((uintptr_t)pCur, &curData, sizeof(curData));

		pModEntry = CONTAINING_RECORD(&curData, DriverDef::LDR_DATA_TABLE_ENTRY64, InMemoryOrderModuleList);
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

uintptr_t Driver::GetModuleAddress32(const wchar_t* moduleName)
{
	DriverDef::WDATA w{ 0 };
	w.operation = DriverDef::GET_PROCESS_PEB32;
	if (!Call(w))
	{
		printf("获取PEB32失败\n");
		return 0;
	}

	DriverDef::PEB32 peb{ 0 };
	ReadBuffer(w.address, &peb, sizeof(peb));

	DriverDef::PEB_LDR_DATA32 ldr{ 0 };
	ReadBuffer((uintptr_t)peb.Ldr, &ldr, sizeof(ldr));

	DriverDef::PLDR_DATA_TABLE_ENTRY32 pModEntry{ 0 };
	for (PLIST_ENTRY32 pCur = (PLIST_ENTRY32)ldr.InMemoryOrderModuleList.Flink; (uint8_t*)pCur != (uint8_t*)peb.Ldr + offsetof(DriverDef::PEB_LDR_DATA32, InMemoryOrderModuleList); pCur = (PLIST_ENTRY32)pModEntry->InMemoryOrderModuleList.Flink)
	{
		DriverDef::LDR_DATA_TABLE_ENTRY32 curData{ 0 };
		ReadBuffer((uintptr_t)pCur, &curData, sizeof(curData));

		pModEntry = CONTAINING_RECORD(&curData, DriverDef::LDR_DATA_TABLE_ENTRY32, InMemoryOrderModuleList);
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

bool Driver::ReadBuffer(uintptr_t address, void* bufefr, size_t size)
{
	DriverDef::WDATA w{ 0 };
	w.operation = DriverDef::READ_BUFFER;
	w.address = address;
	w.size = size;
	w.buffer = bufefr;
	if (!Call(w))
	{
		printf("读取失败\n");
		return false;
	}
	return true;
}

bool Driver::WriteBuffer(uintptr_t address, void* bufefr, size_t size)
{
	DriverDef::WDATA w{ 0 };
	w.operation = DriverDef::WRITE_BUFFER;
	w.address = address;
	w.size = size;
	w.buffer = bufefr;
	if (!Call(w))
	{
		printf("写入失败\n");
		return false;
	}
	return true;
}

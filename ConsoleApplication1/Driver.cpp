#include "Driver.h"

bool Driver::Init(uint64_t pid)
{
	LoadLibraryA("user32.dll");
	LoadLibraryA("win32u.dll");
	HMODULE module = GetModuleHandleA("win32u.dll");
	if (!module)
	{
		return false;
	}
	const char CryptFunctionName[] = "\x3d\x63\x44\x62\x54\x61\x41\x54\x56\x58\x62\x63\x54\x61"
		"\x43\x5e\x64\x52\x57\x3f\x50\x53\x32\x50\x5f\x50\x51\x5b\x54";
	std::string FunctionName(sizeof(CryptFunctionName), '\0');
	for (size_t i = 0; i < strlen(CryptFunctionName); i++)
	{
		FunctionName[i] = CryptFunctionName[i] + 0x11;
	}
	comm = reinterpret_cast<DriverDef::Comm_t>(GetProcAddress(module, FunctionName.c_str()));
	if (!comm)
	{
		printf("不能获取函数\n");
		return false;
	}
	this->pid = pid;
	DriverDef::WDATA w{ 0 };
	w.operation = DriverDef::GET_PROCESS;
	w.pid = pid;
	if (Call(&w))
	{
		printf("设置进程失败\n");
		return false;
	}
	process = w.pid;
	srand(time(nullptr));
	return true;
}

uintptr_t Driver::GetModuleAddress(const wchar_t* moduleName)
{
	DriverDef::WDATA w{ 0 };
	w.operation = DriverDef::GET_PROCESS_PEB;
	w.process = process;
	if (Call(&w))
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
	w.process = process;
	if (Call(&w))
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
	w.process = process;
	w.address = address;
	w.size = size;
	w.buffer = bufefr;
	if (Call(&w))
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
	w.process = process;
	w.address = address;
	w.size = size;
	w.buffer = bufefr;
	if (Call(&w))
	{
		printf("写入失败\n");
		return false;
	}
	return true;
}

#include "Utils.h"
#include "Hook.h"

extern "C" NTSTATUS Entry()
{
	char target[] = { 0x66, 0x58, 0x5d, 0x22, 0x21, 0x5a, 0x1d, 0x62, 0x68, 0x62, 0x0 };
	for (size_t i = 0; i < strlen(target); i++)
	{
		target[i] += 0x11;
	}
	void* win32k = Utils::GetSystemModule(target);
	if (!win32k) {
		dbg("win32k not found!");
		return STATUS_FAILED_DRIVER_ENTRY;
	}
	uintptr_t hookAddress = (uintptr_t)win32k+0x664E0;
	dbg("Data Pointer: %p", hookAddress);
	dbg("1: %p", *reinterpret_cast<uintptr_t*>(hookAddress));
	void* ret = InterlockedExchangePointer((volatile PVOID*)hookAddress, &Hook::hooked_function);
	dbg("2: %p", *reinterpret_cast<uintptr_t*>(hookAddress));
	return STATUS_SUCCESS;
}
#include "def.h"
#include "LazyImport.h"
#include "Utils.h"
#include "Hook.h"

extern "C" NTSTATUS Entry()
{
	dbg("TEst");
	if (!LazyImport::Init())
	{
		return STATUS_UNSUCCESSFUL;
	}
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

	uintptr_t HookAddress = reinterpret_cast<uintptr_t>(win32k)+0x664E0;
	return Hook::StartHook(reinterpret_cast<void**>(HookAddress));
}
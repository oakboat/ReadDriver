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
	const char CryptTrage[] = "\x66\x58\x5d\x22\x21\x5a\x1d\x62\x68\x62";
	char target[sizeof(CryptTrage)]{ 0 };
	for (size_t i = 0; i < strlen(CryptTrage); i++)
	{
		target[i] = CryptTrage[i] + 0x11;
	}
	void* win32k = Utils::GetSystemModule(target);
	if (!win32k) {
		dbg("win32k not found!");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	uintptr_t HookAddress = reinterpret_cast<uintptr_t>(win32k)+0x664E0;
	return Hook::StartHook(reinterpret_cast<void**>(HookAddress));
}
#include "LazyImport.h"
#include "Utils.h"
#include "Hook.h"
#include "../3rd party/skCrypter/skCrypter.h"

extern "C" NTSTATUS Entry()
{
	KdPrint(("Load Success"));
	if (!LazyImport::Init())
	{
		return STATUS_UNSUCCESSFUL;
	}

	auto target = skCrypt("win32k.sys");
	void* win32k = Utils::GetSystemModule(target);
	if (!win32k) {
		KdPrint(("win32k not found!"));
		return STATUS_FAILED_DRIVER_ENTRY;
	}
	target.clear();
	
	//NtUserRegisterTouchPadCapable
	uintptr_t HookAddress = reinterpret_cast<uintptr_t>(win32k)+0x664E0;
	return Hook::StartHook(reinterpret_cast<void**>(HookAddress));
}

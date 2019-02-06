#include "stdafx.h"

#include "torque.h"
#include "detours/detours.h"

#define PATCH_ADDR_1 0x4A1B2F  // jz      short loc_4A1B39    (hex: 74 08)
#define PATCH_ADDR_2 0x4A1B33  // jz      loc_4A1D33          (hex: 0F 84 FA 01 00 00)


DWORD WINAPI init(LPVOID args)
{
	if (!torque_init())
		return 0;


	int error = WriteProcessMemory(GetCurrentProcess(), (void*)PATCH_ADDR_1, "\x90\x90", 2, NULL);

	if ( !error )
	{
		Printf("\nPackageAnyFunction.dll: Patch failed! (Error Code: %d)", GetLastError());
		return GetLastError();
	}

	/* Copypaste - who cares */

	error = WriteProcessMemory(GetCurrentProcess(), (void*)PATCH_ADDR_2, "\xE9\xFB\x01\x00\x00\x90", 6, NULL);
		
	if ( !error )
	{
		Printf("\nPackageAnyFunction.dll: Patch failed! (Error Code: %d)", GetLastError());
		return GetLastError();
	}

	Printf("\nPackageAnyFunction.dll: Patch successful!");

	return 0;
}

bool deinit()
{
	return true;
}

// Entry point
int WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID reserved)
{
	if (reason == DLL_PROCESS_ATTACH)
	{
		CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)init, NULL, 0, NULL);
	}
	else if (reason == DLL_PROCESS_DETACH)
	{
		return deinit();
	}

	return true;
}

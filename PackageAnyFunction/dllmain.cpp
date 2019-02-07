#include "stdafx.h"

#include "torque.h"
#include "detours/detours.h"

#define PATCH_ADDR_1 0x4A164B  // jz      short loc_4A1669    (hex: 74 1C)
#define PATCH_ADDR_2 0x4A166B  // jnz     short loc_4A169F    (hex: 75 32)
#define PATCH_ADDR_3 0x4A1B2F  // jz      short loc_4A1B39    (hex: 74 08)
#define PATCH_ADDR_4 0x4A1B33  // jz      loc_4A1D33          (hex: 0F 84 FA 01 00 00)


DWORD patchGame (int address, const char* patchString, int patchSize)
{
	int error;
	DWORD lastError;

	error = WriteProcessMemory(GetCurrentProcess(), (void*)address, patchString, patchSize, NULL);

	if ( !error )
	{
		lastError = GetLastError();

		Printf("\nPackageAnyFunction.dll: Patch at %d failed! (Error Code: %d)", address, lastError);
		return lastError;
	}
	
	return 0;
}

DWORD WINAPI init(LPVOID args)
{
	if (!torque_init())
		return 0;


	DWORD error; 

	// Skip engine-defined check #1 (method)
	if ( (error = patchGame(PATCH_ADDR_1, "\xEB\x1C", 2)) != 0 )
	{
		return error;
	}

	// Skip engine-defined check #2 (function)
	if ( (error = patchGame(PATCH_ADDR_2, "\xEB\x32", 2)) != 0 )
	{
		return error;
	}

	// NOP out a jump-if-zero operation
	if ( (error = patchGame(PATCH_ADDR_3, "\x90\x90", 2)) != 0 )
	{
		return error;
	}

	// Change a jump-if-zero to a jump and add a NOP at the end to compensate for loss of a cycle
	if ( (error = patchGame(PATCH_ADDR_4, "\xE9\xFB\x01\x00\x00\x90", 6)) != 0 )
	{
		return error;
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

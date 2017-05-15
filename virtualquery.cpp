#include <windows.h>
#include <iostream>

void VirtualTest(HMODULE);
MEMORY_BASIC_INFORMATION GetPreviousAllocation(HMODULE);

/*
To Skip VQEX calls correctly:

Redirect VirtualQueryEx

Get AllocationBase of allocation before module base address
Get highest address related to module
Get MEMORY_BASIC_INFORMATION of allocation after module
- If previous and following allocations are the same type, combine RegionSizes
- Else extend previous RegionSize to cover our own

*/

MEMORY_BASIC_INFORMATION GetPreviousAllocation(HMODULE hModule) {
	MEMORY_BASIC_INFORMATION mbi;

	VirtualQuery((LPCVOID)(hModule - 4097), &mbi, sizeof(mbi));
	return mbi;
}

MEMORY_BASIC_INFORMATION GetFollowingAllocation(HMODULE hModule) {
	MEMORY_BASIC_INFORMATION mbi;

	int offset;

	VirtualQuery((LPCVOID)hModule, &mbi, sizeof(mbi));

	offset = (DWORD)mbi.BaseAddress;

	while (mbi.AllocationBase == hModule) {
		offset += mbi.RegionSize;
		VirtualQuery((LPCVOID)offset, &mbi, sizeof(mbi));
	}

	return mbi;
}

void VirtualTest(HMODULE hModule) {
	int i = 0;

	HANDLE processHandle = GetCurrentProcess();

	MEMORY_BASIC_INFORMATION mbi;

	char messageBoxTitle[100000];
	memset(messageBoxTitle, NULL, sizeof(messageBoxTitle));
	
	sprintf_s(messageBoxTitle, "DLL Base: %p\n", (void*) hModule);
	
	while (i < 0x80000000) {
		VirtualQueryEx(processHandle, (LPCVOID) i, &mbi, sizeof(mbi));
		
		//if (mbi.AllocationBase == hModule) {
			char thisLine[100];
			sprintf_s(thisLine, "Start: %p, Size: %i, aBase: %p, BaseA: %p, State: %s\n", (void*)i, mbi.RegionSize, (void*)mbi.AllocationBase, (void*)mbi.BaseAddress, mbi.State==MEM_FREE?"MEM_FREE":mbi.State==MEM_COMMIT?"MEM_COMMIT":mbi.State==MEM_RESERVE?"MEM_RESERVE":"NO_IDEA");
			strcat_s(messageBoxTitle, thisLine);
		//}
		i += mbi.RegionSize;
	}
	
	MessageBoxA(NULL, messageBoxTitle, "Test", MB_OK);
	memset(messageBoxTitle, NULL, sizeof(messageBoxTitle));

	MEMORY_BASIC_INFORMATION prev = GetPreviousAllocation(hModule);
	MEMORY_BASIC_INFORMATION next = GetFollowingAllocation(hModule);
	MEMORY_BASIC_INFORMATION fake = prev;

	if (prev.State == MEM_FREE) {
		if (next.State == MEM_FREE) {
			int difference = (DWORD)next.BaseAddress - (DWORD) prev.BaseAddress;
			fake.RegionSize = difference + next.RegionSize;
		}
		else {
			int difference = (DWORD)next.BaseAddress - (DWORD) prev.BaseAddress;
			fake.RegionSize = difference;
		}
	}

	sprintf_s(messageBoxTitle, "Test: %i\nOur Base: %p\n \
							   Previous Size: %i, Previous Base: %p, Previous Type: %s\n \
							   Next Size: %i, Next Base: %p, Next Type: %s\n \
							   Fake Size: %i, Fake Base: %p, Fake aBase: %p, Fake Type: %s",
		fake.State,
		(void *) hModule,
		prev.RegionSize,
		(void *) prev.BaseAddress,
		prev.State==MEM_FREE?"MEM_FREE":prev.State==MEM_COMMIT?"MEM_COMMIT":prev.State==MEM_RESERVE?"MEM_RESERVE":"NO_IDEA",
		next.RegionSize,
		(void *) next.BaseAddress,
		next.State==MEM_FREE?"MEM_FREE":next.State==MEM_COMMIT?"MEM_COMMIT":next.State==MEM_RESERVE?"MEM_RESERVE":"NO_IDEA",
		fake.RegionSize,
		(void *) fake.BaseAddress,
		(void *) fake.AllocationBase,
		fake.State==MEM_FREE?"MEM_FREE":fake.State==MEM_COMMIT?"MEM_COMMIT":fake.State==MEM_RESERVE?"MEM_RESERVE":"NO_IDEA"
	);
	
	MessageBoxA(NULL, messageBoxTitle, "Test", MB_OK);

}
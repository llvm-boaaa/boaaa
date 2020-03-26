#include "boaaa/dynamic_interface.h" 
#include "boaaa/lv/stdlvafx.h"

using namespace boaaa;

//dll entry point, windows only.
#if defined _WIN32 && defined BUILD_SHARED_LIB
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif

/*
 * tryed to use include_versions, to include DLInterfaceXX, couldn't get it to work, maybe because in sourcefiles...
 *
 * don't try it, i have lost 2h of testing.
 */

#ifdef LLVM_VERSION_40
#include "boaaa/lv/40/Interface_40.h"
#endif // LLVM_VERSION_40
#ifdef LLVM_VERSION_50
#include "boaaa/lv/50/Interface_50.h"
#endif // !LLVM_VERSION_50
#ifdef LLVM_VERSION_60
#include "boaaa/lv/60/Interface_60.h"
#endif // LLVM_VERSION_60
#ifdef LLVM_VERSION_71
#include "boaaa/lv/71/Interface_71.h"
#endif // !LLVM_VERSION_71
#ifdef LLVM_VERSION_80
#include "boaaa/lv/80/Interface_80.h"
#endif //!LLVM_VERSION_80
#ifdef LLVM_VERSION_90
#include "boaaa/lv/90/Interface_90.h"
#endif //!LLVM_VERSION_90

DLIFp genDLIF() {
	return DLIFp(
#ifdef NOT_SUPPORTED_LLVM_VERSION
		nullptr //nullpointer return if unsupported llvm version
#else //init DLInterfaceXX
#ifdef LLVM_VERSION_40
		new DLInterface40()
#endif // LLVM_VERSION_40
#ifdef LLVM_VERSION_50
		new DLInterface50()
#endif //LLVM_Version_50
#ifdef LLVM_VERSION_60
		new DLInterface60()
#endif // LLVM_VERSION_60
#ifdef LLVM_VERSION_71
		new DLInterface71()
#endif // LLVM_Version_71
#ifdef LLVM_VERSION_80
		new DLInterface80()
#endif // LLVM_VERSION_80
#ifdef LLVM_VERSION_90
		new DLInterface90()
#endif // LLVM_VERSION_90
#endif // NOT_SUPPORTED_LLVM_VERSION
	);
}

void delDLFI(DLIFp dlfip) {
	dlfip.reset();
		
}

extern "C" __export DL_Info GETDL_INFO() {
	DL_Info info;
	info.version = LLVM_VERSION;
	info.gen = &genDLIF;
	info.del = &delDLFI;
	return info;
}
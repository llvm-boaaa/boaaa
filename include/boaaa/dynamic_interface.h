#ifndef LLVM_DYN_INTERFACE_H
#define LLVM_DYN_INTERFACE_H

#include "export.h"
#include <inttypes.h>
#include <memory>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

namespace boaaa {

	struct DL_Info;
	class DLInterface;

	//Dynamic LIbrary InterFace pointer
	typedef std::unique_ptr<DLInterface> DLIFp;

	typedef void* (*parseAAResult)();
	typedef DLIFp(*generateInterfaceFunc)();
	typedef void (*deleteInterfaceFunc)(DLIFp dlifp);

	typedef uint8_t llvm_version;

	struct DL_Info 
	{
		llvm_version version;
		generateInterfaceFunc gen;
		deleteInterfaceFunc del;
	};

	//defile macro GETDL_INFO and GETDL_INFOFUNC
	#define INCLUDE_HEADER "GETDL_INFO.inc"
	#include "boaaa/MacroHelp.inc"

	typedef DL_Info(*GETDL_INFOFUNC)();

	extern "C" __export DL_Info GETDL_INFO();

	class __export DLInterface 
	{

	public:
		virtual void onLoad() = 0;
		virtual void onUnload() = 0;

	};
}

#include "minwindef_undef.inc"

#endif
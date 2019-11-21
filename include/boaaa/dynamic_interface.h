#ifndef LLVM_DYN_INTERFACE_H
#define LLVM_DYN_INTERFACE_H

#include "export.h"
#include <inttypes.h>
#include <memory>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif


#define STRINGIFY(MACRO) #MACRO
#define GETDL_INFO getDL_INFO

namespace boaaa {

	class DLInterface;
	//Dynamic LIbrary InterFace pointer
	typedef std::unique_ptr<DLInterface> DLIFp;

	typedef void* (*parseAAResult)();
	typedef DLIFp(*generateInterfaceFunc)();
	typedef void (*deleteInterfaceFunc)(DLIFp dlifp);

	typedef uint8_t llvm_version;

	struct DL_Info {
		llvm_version version;
		generateInterfaceFunc gen;
		deleteInterfaceFunc del;
	};

	extern "C" __export DL_Info GETDL_INFO();

	class __export DLInterface {

	public:
		virtual void onLoad() = 0;
		virtual void onUnload() = 0;

	};
}

#endif
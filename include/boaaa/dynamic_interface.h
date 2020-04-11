#ifndef LLVM_DYN_INTERFACE_H
#define LLVM_DYN_INTERFACE_H

#include <inttypes.h>
#include <memory>
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#endif

#include "boaaa/export.h"
#include "boaaa/vp/StringRefVersionParser.h"

#include "boaaa/commandline_types.h"

namespace boaaa {

	struct DL_Info;
	class DLInterface;

	//Dynamic LIbrary InterFace pointer
	typedef std::shared_ptr<DLInterface> DLIFp;

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
	protected:
		DLInterface() {}
	public:

		virtual ~DLInterface() = default;

		virtual void onLoad() = 0;
		virtual void onUnload() = 0;

		virtual void registerStringRefVPM(StringRefVPM* manager) = 0;
		virtual void setBasicOStream(std::ostream& ostream, bool del = false) = 0;
		virtual boaaa::cl_aa_store getAvailableAAs() = 0;
		
		virtual bool loadModule(uint64_t module_file_hash) = 0;
		virtual bool loadModule(uint64_t module_file_prefix, uint64_t module_file_hash) = 0;
		virtual void unloadModule() = 0;
		virtual bool runAnalysis(boaaa::aa_id analysis) = 0;


		virtual llvm_version getVersion()
		{
			return -10;
		}
		//only defined in tool, so test ist allways public, 
		//but only callable from outside, when DEBUG_DLL_Test is defined
#ifndef DEBUG_DLL_TEST
	private:
#endif
		virtual void test(uint64_t* hash = nullptr, uint8_t num = 0) = 0;
	};
}

#endif
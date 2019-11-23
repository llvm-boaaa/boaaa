#ifndef LLVM_VERSION_MANAGER_H
#define LLVM_VERSION_MANAGER_H

#include "boaaa/dynamic_interface.h"
#include "boaaa/lvm/DLHandle.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringMap.h"


#ifdef _WIN32
#include "boaaa/lvm/WindowsDLHandle.h"
using _DLHandle = boaaa::WindowsDLHandle;
#else
#include "boaaa/lvm/UnixDLHandle.h"
using _DLHandle = boaaa::UnixDLHandle;
#endif

using namespace llvm;

namespace boaaa {

	const std::string base_folder = "../lib";

	class LLVM_Info {
	public:
		DL_Info info;
		StringRef filename;
		std::unique_ptr<DLInterface> inst;
		uint8_t ref_counter;
		std::unique_ptr<_DLHandle> handle;

		LLVM_Info(StringRef filename, std::unique_ptr<DLInterface> inst, std::unique_ptr<_DLHandle> handle, DL_Info info) 
			: info(info), filename(filename), inst(std::move(inst)), ref_counter(0), handle(std::move(handle)) { };

		~LLVM_Info() { inst.release(); handle.release(); };
	};

	class LLVMVersionManager
	{
	private:
		std::unique_ptr<StringMap<LLVM_Info*>> m_dl_map;

	public:
		LLVMVersionManager() : m_dl_map(std::unique_ptr<StringMap<LLVM_Info*>>
			(new StringMap<LLVM_Info*>())) { };

		//LLVMVersionManager(const LLVMVersionManager&) {}

		~LLVMVersionManager() {};

		//static auto getInstance() -> LLVMVersionManager&;

		std::unique_ptr<DLInterface> loadDL(StringRef filename) {
			return loadDL(filename, base_folder);
		};
		
		std::unique_ptr<DLInterface> loadDL(StringRef filename, StringRef folder);

		void mayUnload(std::unique_ptr<DLInterface> inst, StringRef filename);
	};
}

#endif
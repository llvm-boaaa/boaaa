#ifndef LLVM_VERSION_MANAGER_H
#define LLVM_VERSION_MANAGER_H

#include "llvm/ADT/StringRef.h"
#include "boaaa/dynamic_interface.h"

using namespace llvm;

namespace boaaa {

	struct LLVM_Info {
		DL_Info info;
		StringRef filename;
		std::unique_ptr<DLInterface> inst;
	};

	class LLVMVersionManager
	{
	private:
		LLVMVersionManager() { };

		LLVMVersionManager(const LLVMVersionManager&){}
	public:
		~LLVMVersionManager();

		static auto getInstance() -> LLVMVersionManager&;

		std::unique_ptr<DLInterface> loadDL(StringRef filename) {

		}

		std::unique_ptr<DLInterface> loadDL(StringRef foulder, StringRef filename) {

		}
	};
}

#endif
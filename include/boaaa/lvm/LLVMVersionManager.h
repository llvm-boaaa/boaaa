#ifndef LLVM_VERSION_MANAGER_H
#define LLVM_VERSION_MANAGER_H

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringMap.h"

#include <iostream>

#include "boaaa/export.h"
#include "boaaa/dynamic_interface.h"
#include "boaaa/EvaluationResult.h"
#include "boaaa/lvm/DLHandle.h"

#include "StringRefVersionParserCurrent.h"

using namespace llvm;

namespace boaaa {

	const std::string base_folder = "../lib";

	class LLVM_Info {
	public:
		DL_Info info;
		StringRef filename;
		std::shared_ptr<DLInterface> inst;
		uint8_t ref_counter;
		std::unique_ptr<DLHandle> handle;

		LLVM_Info(StringRef filename, std::shared_ptr<DLInterface> inst, std::unique_ptr<DLHandle> handle, DL_Info info) 
			: info(info), filename(filename), inst(inst), ref_counter(0), handle(std::move(handle)) { };

		~LLVM_Info() { handle.reset(nullptr); };
	};

	class LLVMVersionManager : public DLInterface, public StringRefVPCur
	{
	private:
		struct manager_context {
			std::ostream* basic_ostream;
			bool del_strm_after_use;

			manager_context()
			{
				basic_ostream = &std::cout;
				del_strm_after_use = false;
			}
		};
		std::unique_ptr<StringMap<LLVM_Info*>> m_dl_map;
		manager_context context;

	public:
		LLVMVersionManager()
		{
			m_dl_map = std::unique_ptr<StringMap<LLVM_Info*>>(new StringMap<LLVM_Info*>());
			onLoad();
		}
		//LLVMVersionManager(const LLVMVersionManager&) {}

		~LLVMVersionManager() { onUnload(); };

		//static auto getInstance() -> LLVMVersionManager&;

		std::shared_ptr<DLInterface> loadDL(StringRef filename) {
			return loadDL(filename, base_folder);
		};
		
		std::shared_ptr<DLInterface> loadDL(StringRef filename, StringRef folder);

		void mayUnload(std::shared_ptr<DLInterface> inst, StringRef filename);

		//DLInterface-functions

		void registerStringRefVPM(StringRefVPM* manager) override;
		void setBasicOStream(std::ostream& ostream, bool del = false) override;
		boaaa::cl_aa_store getAvailableAAs() override;

	private:
		void onLoad() override;
		void onUnload() override;

		void test(uint64_t* hash = nullptr, uint8_t num = 0) override;
		bool loadModule(uint64_t module_file_hash) override;
		bool loadModule(uint64_t module_file_prefix, 
						uint64_t module_file_hash) override;
		void unloadModule() override;
		bool runAnalysis(boaaa::aa_id analysis) override { return false; }
		bool runAnalysis(boaaa::aa_id analysis, EvaluationResult& er) override { return false; }

		void storeAAResults(bool store) override { }

		ModuleResult* getModuleResult() override
		{
			return nullptr;
		}
	};

}


#endif

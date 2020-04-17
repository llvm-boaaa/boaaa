#ifndef BOAAA_LV_INTERFACE_50_H
#define BOAAA_LV_INTERFACE_50_H

#include "boaaa/dynamic_interface.h"
#include "boaaa/support/raw_type.h"
#include "boaaa/vp/StringRefVersionParser.h"

#include "boaaa/lv/version_context.h"
#include "StringRefVersionParser50.h"

#include "initalizeAAs_50.h"
#include "boaaa/lv/EvaluationPassDefinitions.h"

namespace boaaa {

	class DLInterface50 : public DLInterface {

	public:
		DLInterface50();
		~DLInterface50();

		void onLoad() override;
		void onUnload() override;
		void registerStringRefVPM(StringRefVPM* manager) override;
		void setBasicOStream(std::ostream& ostream, bool del = false) override;
		boaaa::cl_aa_store getAvailableAAs() override;

		bool loadModule(uint64_t module_file_hash) override;
		bool loadModule(uint64_t module_file_prefix, 
						uint64_t module_file_hash) override;
		void unloadModule() override;
		bool runAnalysis(boaaa::aa_id analysis) override;
		bool runAnalysis(boaaa::aa_id analysis, EvaluationResult& er) override;
		
		ModuleResult* getModuleResult() override
		{
			return context.module_result.get();
		}
		
		llvm_version getVersion() override
		{
			return LLVM_VERSION;
		}

		char* getVersionString() override
		{
			return __STRINGIFY(LLVM_VERSION);
		}

		void test(uint64_t* hash = nullptr, uint8_t num = 0) override;
	private:
		version_context context;
	};

}
#endif
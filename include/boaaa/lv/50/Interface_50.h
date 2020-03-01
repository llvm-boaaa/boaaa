#ifndef BOAAA_LV_INTERFACE_50_H
#define BOAAA_LV_INTERFACE_50_H

#include "boaaa/dynamic_interface.h"
#include "boaaa/support/raw_type.h"
#include "boaaa/vp/StringRefVersionParser.h"

#include "boaaa/lv/version_context.h"
#include "StringRefVersionParser50.h"

#include "initalizeAAs_50.h"

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
	
		llvm_version getVersion() override
		{
			return LLVM_VERSION;
		}
		void test(uint64_t* hash = nullptr, uint8_t num = 0) override;
	private:
		version_context context;
	};

}
#endif
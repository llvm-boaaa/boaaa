#ifndef BOAAA_LV_INTERFACE_40_H
#define BOAAA_LV_INTERFACE_40_H

#include "boaaa/dynamic_interface.h"
#include "boaaa/support/raw_type.h"
#include "boaaa/vp/StringRefVersionParser.h"
#include "boaaa/lv/version_context.h"

#include "StringRefVersionParser40.h"

namespace boaaa {

	class DLInterface40 : public DLInterface {

	public:
		DLInterface40();
		~DLInterface40();

		void onLoad() override;
		void onUnload() override;
		void registerStringRefVPM(StringRefVPM* manager) override;
		void setBasicOStream(std::ostream& ostream, bool del = false) override;
		bool loadModule(uint64_t module_file_hash) override;
				
		
		void test(uint64_t* hash = nullptr, uint8_t num = 0) override;

	private:
		version_context context;
	};

}
#endif
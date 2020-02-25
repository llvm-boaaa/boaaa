#ifndef BOAAA_LV_INTERFACE_90_H
#define BOAAA_LV_INTERFACE_90_H

#include "boaaa/dynamic_interface.h"
#include "boaaa/support/raw_type.h"
#include "boaaa/vp/StringRefVersionParser.h"

#include <ostream>

#include "boaaa/lv/version_context.h"
#include "StringRefVersionParser90.h"

namespace boaaa {

	class DLInterface90 : public DLInterface {

	public:
		DLInterface90();
		~DLInterface90();

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
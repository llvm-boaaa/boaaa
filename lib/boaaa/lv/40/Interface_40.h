#ifndef BOAAA_LV_INTERFACE_40_H
#define BOAAA_LV_INTERFACE_40_H

#include "boaaa/dynamic_interface.h"
#include "boaaa/vp/StringRefVersionParser.h"
#include "StringRefVersionParser40.h"
#include "../version_context.h"

namespace boaaa {

	class DLInterface40 : public DLInterface {

	public:
		DLInterface40();
		~DLInterface40();

		void onLoad() override;
		void onUnload() override;
		void registerStringRefVPM(std::shared_ptr<StringRefVPM> manager) override;
		void setBasicOStream(std::ostream& ostream, bool del = false) override;
				
		
		void test(uint64_t* hash = nullptr, uint8_t num = 0) override;

	private:
		version_context context;
	};

}
#endif
#ifndef BOAAA_LV_INTERFACE_50_H
#define BOAAA_LV_INTERFACE_50_H

#include "boaaa/dynamic_interface.h"
#include "boaaa/vp/StringRefVersionParser.h"
#include "StringRefVersionParser50.h"
#include "../version_context.h"

namespace boaaa {

	class DLInterface50 : public DLInterface {

	public:
		DLInterface50();
		~DLInterface50();

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
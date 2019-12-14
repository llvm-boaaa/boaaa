#ifndef BOAAA_LV_INTERFACE_90_H
#define BOAAA_LV_INTERFACE_90_H

#include "boaaa/dynamic_interface.h"
#include "boaaa/vp/StringRefVersionParser.h"
#include "StringRefVersionParser90.h"
#include "../version_context.h"

namespace boaaa {

	class DLInterface90 : public DLInterface {

	public:
		DLInterface90();
		~DLInterface90();

		void onLoad() override;
		void onUnload() override;
		void registerStringRefVPM(StringRefVPM* manager) override;
		void setBasicOStream(std::ostream& ostream, bool del = false) override;


		void test(uint64_t* hash = nullptr, uint8_t num = 0) override;
	private:
		version_context context;
	};

}
#endif
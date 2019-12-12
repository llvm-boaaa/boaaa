#ifndef BOAAA_LV_INTERFACE_90_H
#define BOAAA_LV_INTERFACE_90_H

#include "boaaa/dynamic_interface.h"
#include "boaaa/vp/StringRefVersionParser.h"
#include "StringRefVersionParser90.h"

namespace boaaa {

	class DLInterface90 : public DLInterface {

	public:
		DLInterface90();
		~DLInterface90();

		void onLoad() override;
		void onUnload() override;
		void registerStringRefVPM(std::unique_ptr<StringRefVPM> manager) override;
		
	};

}
#endif
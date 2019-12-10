#ifndef BOAAA_LV_INTERFACE_40_H
#define BOAAA_LV_INTERFACE_40_H

#include "boaaa/dynamic_interface.h"

namespace boaaa {

	class DLInterface40 : public DLInterface {

	public:
		DLInterface40();
		~DLInterface40();

		void onLoad() override;
		void onUnload() override;
		void registerStringRefVPM(std::unique_ptr<StringRefVPM> manager) override;

	};

}
#endif
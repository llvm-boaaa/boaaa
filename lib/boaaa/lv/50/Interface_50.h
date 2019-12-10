#ifndef BOAAA_LV_INTERFACE_50_H
#define BOAAA_LV_INTERFACE_50_H

#include "boaaa/dynamic_interface.h"

namespace boaaa {

	class DLInterface50 : public DLInterface {

	public:
		DLInterface50();
		~DLInterface50();

		void onLoad() override;
		void onUnload() override;
		void registerStringRefVPM(std::unique_ptr<StringRefVPM> manager) override;

	};

}
#endif
#include "boaaa/dynamic_interface.h"

#ifndef BOAAA_LV_INTERFACE_90_H
#define BOAAA_LV_INTERFACE_90_H

namespace boaaa {

	class DLInterface90 : public DLInterface {

	public:
		DLInterface90();
		~DLInterface90();

		void onLoad();
		void onUnload();
		
	};

}
#endif
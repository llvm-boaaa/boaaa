#ifndef __EXPORT_H
#define __EXPORT_H

#if (defined _WIN32) && (!defined NO_EXPORT)
	#if defined BUILD_SHARED_LIB    //export
		#define __export __declspec(dllexport)
	#else						    //import
		#define __export __declspec(dllimport)
	#endif
#else // MAC / Linux
	#define __export
#endif // _WIN32

#endif // __EXPORT_H
#ifndef __EXPORT_H
#define __EXPORT_H

#ifdef _WIN32
	#if defined BUILD_SHARED_LIB    //export
		#define __export __declspec(dllexport)
	#else						    //import
		#define __export __declspec(dllimport)
	#endif
#else // MAC / Linux
	#define __export
#endif // _WIN32

#endif // __EXPORT_H
#ifndef __EXPORT_H
#define __EXPORT_H

#ifdef _WIN32
	#if defined BUILD_SHARED_LIBS
		#define __export __declspec(dllexport)
	#else
#define __export __declspec(dllimport)
	#endif
#else
	#define __export
#endif 

#endif
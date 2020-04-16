#ifndef BOAAA_SUPPORTED_LLVM_VERSION_H
#define BOAAA_SUPPORTED_LLVM_VERSION_H
#define NOT_SUPPORTED_LLVM_VERSION

#ifdef LLVM_VERSION_40
#ifndef NOT_SUPPORTED_LLVM_VERSION
#define LLVM_TWO_VERSIONS_ LLVM_VERSION_40
#endif
#undef NOT_SUPPORTED_LLVM_VERSION
#endif //!LLVM_VERSION_40

#ifdef LLVM_VERSION_50
#ifndef NOT_SUPPORTED_LLVM_VERSION
#define LLVM_TWO_VERSIONS_ LLVM_VERSION_50
#endif
#undef NOT_SUPPORTED_LLVM_VERSION
#endif //!LLVM_VERSION_50

#ifdef LLVM_VERSION_60
#ifndef NOT_SUPPORTED_LLVM_VERSION
#define LLVM_TWO_VERSIONS_ LLVM_VERSION_60
#endif
#undef NOT_SUPPORTED_LLVM_VERSION
#endif //!LLVM_VERSION_60

#ifdef LLVM_VERSION_70
#ifndef NOT_SUPPORTED_LLVM_VERSION
#define LLVM_TWO_VERSIONS_ LLVM_VERSION_70
#endif
#undef NOT_SUPPORTED_LLVM_VERSION
#endif //!LLVM_VERSION_70

#ifdef LLVM_VERSION_71
#ifndef NOT_SUPPORTED_LLVM_VERSION
#define LLVM_TWO_VERSIONS_ LLVM_VERSION_71
#endif
#undef NOT_SUPPORTED_LLVM_VERSION
#endif //!LLVM_VERSION_71

#ifdef LLVM_VERSION_80
#ifndef NOT_SUPPORTED_LLVM_VERSION
#define LLVM_TWO_VERSIONS_ LLVM_VERSION_80
#endif
#undef NOT_SUPPORTED_LLVM_VERSION
#endif //!LLVM_VERSION_80

#ifdef LLVM_VERSION_90
#ifndef NOT_SUPPORTED_LLVM_VERSION
#define LLVM_TWO_VERSIONS_ LLVM_VERSION_90
#endif
#undef NOT_SUPPORTED_LLVM_VERSION
#endif //!LLVM_VERSION_90

#ifdef LLVM_VERSION_100
#ifndef NOT_SUPPORTED_LLVM_VERSION
#define LLVM_TWO_VERSIONS_ LLVM_VERSION_100
#endif
#undef NOT_SUPPORTED_LLVM_VERSION
#endif //!LLVM_VERSION_90

//error handling, register version above bye writing:
/*
#ifdef LLVM_VERSION_XX
#ifndef NOT_SUPPORTED_LLVM_VERSION
#define LLVM_TWO_VERSIONS_ LLVM_VERSION_XX
#endif
#undef NOT_SUPPORTED_LLVM_VERSION
#endif //!LLVM_VERSION_XX
*/
// and following in sup_LLVM_VERSION_help.inc
/*
#ifdef LLVM_VERSION_XX
#if LLVM_VERSION != LLVM_VERSION_XX
#define LLVM_TWO_VERSIONS LLVM_VERSION_XX
#endif
#endif //!LLVM_VERSION_XX
*/

#ifdef NOT_SUPPORTED_LLVM_VERSION
#ifndef LLVM_VERSION
#define LLVM_VERSION "unknown"
#endif
#error "LLVM_" LLVM_VERSION "is not implemented or not registered in this file:" __FILE__
#endif

#ifdef LLVM_TWO_VERSIONS_

#include "boaaa/lv/sup_LLVM_VERSION_help.inc"
#include "boaaa/support/__STRINGIFY.h"
#define LLVM_VERSION_PRINT __STRINGIFY(LLVM_VERSION)
#define LLVM_TWO_VERSIONS_PRINT __STRINGIFY(LLVM_TWO_VERSIONS)

#ifndef __COMPILE_ERROR_LOCK
#define __COMPILE_ERROR_LOCK
//print error at compile time
namespace {
	constexpr int compile_error() {
		static_assert(false, "minimal two versions of LLVM defined: LLVM_" LLVM_VERSION_PRINT " and LLVM_" LLVM_TWO_VERSIONS_PRINT " but it has to be one.");
		return 0;
	}

	const int __error__ = compile_error(); //evaluate to create error at compile time
}
#endif
//nicer way to print error, but currently not supported
//#include "boaaa/support/compile_error.h"
//COMPILE_ERROR_FUNC(char*, int, char*, int, char*) COMPILE_ERROR_PARAM("minimal two versions of LLVM defined: LLVM_", LLVM_VERSION, " and LLVM_", LLVM_TWO_VERSIONS, " but it has to be one.")
#undef LLVM_VERSION_PRINT
#undef LLVM_TWO_VERSIONS_PRINT
#endif

#endif //BOAAA_SUPPORTED_LLVM_VERSION_H

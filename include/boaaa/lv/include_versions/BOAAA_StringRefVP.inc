#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/BOAAA_StringRefVP.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_40_INCLUDE "boaaa/lv/40/StringRefVersionParser40.h"
#define LLVM_VERSION_50_INCLUDE "boaaa/lv/50/StringRefVersionParser50.h"
#define LLVM_VERSION_90_INCLUDE "boaaa/lv/90/StringRefVersionParser90.h"

#define LLVM_VERSION_UNIFY using BOAAAStringRefVP = 

#define LLVM_VERSION_40_UNIFY boaaa::StringRefVP40
#define LLVM_VERSION_50_UNIFY boaaa::StringRefVP50
#define LLVM_VERSION_90_UNIFY boaaa::StringRefVP90

#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
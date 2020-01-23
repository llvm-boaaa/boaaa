#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/BOAAA_StringRefVP.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_40_INCLUDE "boaaa/lv/40/StringRefVersionParser40.h"
#define LLVM_VERSION_50_INCLUDE "boaaa/lv/50/StringRefVersionParser50.h"
#define LLVM_VERSION_90_INCLUDE "boaaa/lv/90/StringRefVersionParser90.h"

#define LLVM_VERSION_UNIFY using BOAAAStringRefVP = typename

#define LLVM_VERSION_40_UNIFY std::add_pointer<boaaa::StringRefVP40>::type
#define LLVM_VERSION_50_UNIFY std::add_pointer<boaaa::StringRefVP50>::type
#define LLVM_VERSION_90_UNIFY std::add_pointer<boaaa::StringRefVP90>::type

#define LLVM_VERSION_UNIFY_FINALIZE ;
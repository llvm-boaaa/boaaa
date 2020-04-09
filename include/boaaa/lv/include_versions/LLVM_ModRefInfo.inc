#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_ModRefInfo.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/Analysis/AliasAnalysis.h"

#define LLVM_VERSION_UNIFY using LLVMModRefInfo =
#define LLVM_VERSION_REST_UNIFY llvm::ModRefInfo
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
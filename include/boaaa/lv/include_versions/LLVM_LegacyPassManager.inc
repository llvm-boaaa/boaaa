#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_LegacyPassManager.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/IR/LegacyPassManagers.h"

#define LLVM_VERSION_UNIFY using LLVMLegacyPassManager =
#define LLVM_VERSION_REST_UNIFY llvm::legacy::PassManager
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
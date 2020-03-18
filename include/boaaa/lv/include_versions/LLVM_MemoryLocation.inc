#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_MemoryLocation.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/Analysis/MemoryLocation.h"

#define LLVM_VERSION_UNIFY using LLVMMemoryLocation =
#define LLVM_VERSION_REST_UNIFY llvm::MemoryLocation
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
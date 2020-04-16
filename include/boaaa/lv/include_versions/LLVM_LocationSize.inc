#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_LocationSize.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/Analysis/MemoryLocation.h"

#define LLVM_VERSION_UNIFY using LLVMLocationSize =
#define LLVM_VERSION_40_UNIFY uint64_t
#define LLVM_VERSION_50_UNIFY uint64_t
#define LLVM_VERSION_60_UNIFY uint64_t
#define LLVM_VERSION_70_UNIFY uint64_t
#define LLVM_VERSION_71_UNIFY uint64_t
#define LLVM_VERSION_80_UNIFY llvm::LocationSize
#define LLVM_VERSION_90_UNIFY llvm::LocationSize
#define LLVM_VERSION_100_UNIFY llvm::LocationSize
#define LLVM_VERSION_REST_UNIFY uint64_t
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
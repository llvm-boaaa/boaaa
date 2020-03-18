#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_LoadInstr.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/IR/Instructions.h"

#define LLVM_VERSION_UNIFY using LLVMLoadInst =
#define LLVM_VERSION_REST_UNIFY llvm::LoadInst
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
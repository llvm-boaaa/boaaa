#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_Use.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/IR/Use.h"

#define LLVM_VERSION_UNIFY using LLVMUse =
#define LLVM_VERSION_REST_UNIFY llvm::Use
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
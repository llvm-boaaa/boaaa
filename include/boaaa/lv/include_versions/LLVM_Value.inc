#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_Value.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/IR/Value.h"

#define LLVM_VERSION_UNIFY using LLVMValue =
#define LLVM_VERSION_REST_UNIFY llvm::Value
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
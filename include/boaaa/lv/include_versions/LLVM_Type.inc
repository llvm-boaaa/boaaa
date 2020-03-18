#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_Type.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/IR/Type.h"

#define LLVM_VERSION_UNIFY using LLVMType =
#define LLVM_VERSION_REST_UNIFY llvm::Type
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
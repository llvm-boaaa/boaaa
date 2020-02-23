#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_FunctionPass.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_UNIFY using LLVMFunctionPass =
#define LLVM_VERSION_REST_UNIFY llvm::FunctionPass
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
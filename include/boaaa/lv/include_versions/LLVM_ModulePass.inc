#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_ModulePass.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_UNIFY using LLVMModulePass =
#define LLVM_VERSION_REST_UNIFY llvm::ModulePass
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
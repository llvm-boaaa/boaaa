#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_RegisterPass.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_UNIFY template<typename T> using LLVMRegisterPass =
#define LLVM_VERSION_REST_UNIFY llvm::RegisterPass<T>
#define LLVM_VERSION_UNIFY_FINALIZE ;


#define BOAAA_MACROLIST_PUSP_MACRO_INCLUDE "boaaa/lv/include_versions/pushMacros.inc"
#define BOAAA_MACROLIST_POP_MACRO_INCLUDE  "boaaa/lv/include_versions/popMacros.inc"
#include "unify_LLVM_VERSION.def"
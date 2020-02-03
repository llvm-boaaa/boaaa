#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_SmallSetVector.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/ADT/SetVector.h"

#define LLVM_VERSION_UNIFY template<typename T, unsigned N> using LLVMSmallSetVector =
#define LLVM_VERSION_REST_UNIFY llvm::SmallSetVector<T, N>
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
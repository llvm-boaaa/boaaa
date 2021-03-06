#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_SmallVector.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/ADT/SmallVector.h"

#define LLVM_VERSION_UNIFY template<typename T, unsigned N> using LLVMSmallVector =
#define LLVM_VERSION_REST_UNIFY llvm::SmallVector<T, N>
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
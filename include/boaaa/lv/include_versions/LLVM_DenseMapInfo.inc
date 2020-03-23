#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_DenseMapInfo.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/ADT/DenseMapInfo.h"

#define LLVM_VERSION_UNIFY template<class T> using LLVMDenseMapInfo =
#define LLVM_VERSION_REST_UNIFY llvm::DenseMapInfo<T>
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
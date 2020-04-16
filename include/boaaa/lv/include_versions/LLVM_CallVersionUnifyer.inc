#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_CallVersionUnifyer.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_40_INCLUDE "llvm/IR/CallSite.h"
#define LLVM_VERSION_50_INCLUDE "llvm/IR/CallSite.h"
#define LLVM_VERSION_60_INCLUDE "llvm/IR/CallSite.h"
#define LLVM_VERSION_70_INCLUDE "llvm/IR/CallSite.h"
#define LLVM_VERSION_71_INCLUDE "llvm/IR/CallSite.h"
#define LLVM_VERSION_80_INCLUDE "llvm/IR/InstrTypes.h"
#define LLVM_VERSION_90_INCLUDE "llvm/IR/InstrTypes.h"
#define LLVM_VERSION_100_INCLUDE "llvm/IR/InstrTypes.h"

#define LLVM_VERSION_UNIFY using LLVMCallUnifyer =

#define LLVM_VERSION_40_UNIFY llvm::CallSite
#define LLVM_VERSION_50_UNIFY llvm::CallSite
#define LLVM_VERSION_60_UNIFY llvm::CallSite
#define LLVM_VERSION_70_UNIFY llvm::CallSite
#define LLVM_VERSION_71_UNIFY llvm::CallSite
#define LLVM_VERSION_80_UNIFY llvm::CallBase
#define LLVM_VERSION_90_UNIFY llvm::CallBase
#define LLVM_VERSION_100_UNIFY llvm::CallBase
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
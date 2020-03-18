#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/LLVM_Instruction.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_REST_INCLUDE "llvm/IR/Instruction.h"

#define LLVM_VERSION_UNIFY using LLVMInstruction =
#define LLVM_VERSION_REST_UNIFY llvm::Instruction
#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
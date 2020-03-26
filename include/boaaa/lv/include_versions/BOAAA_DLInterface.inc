#define LLVM_VERSION_CALL_FILE "boaaa/lv/include_versions/BOAAA_DLInterface.inc"
#define LLVM_VERSION_CALL_LINE "2"

#define LLVM_VERSION_40_INCLUDE "boaaa/lv/40/Interface_40.h"
#define LLVM_VERSION_50_INCLUDE "boaaa/lv/50/Interface_50.h"
#define LLVM_VERSION_60_INCLUDE "boaaa/lv/60/Interface_60.h"
#define LLVM_VERSION_71_INCLUDE "boaaa/lv/71/Interface_71.h"
#define LLVM_VERSION_80_INCLUDE "boaaa/lv/80/Interface_80.h"
#define LLVM_VERSION_90_INCLUDE "boaaa/lv/90/Interface_90.h"

#define LLVM_VERSION_UNIFY using BOAAADLInterface = 

#define LLVM_VERSION_40_UNIFY boaaa::DLInterface40
#define LLVM_VERSION_50_UNIFY boaaa::DLInterface50
#define LLVM_VERSION_60_UNIFY boaaa::DLInterface60
#define LLVM_VERSION_71_UNIFY boaaa::DLInterface71
#define LLVM_VERSION_80_UNIFY boaaa::DLInterface80
#define LLVM_VERSION_90_UNIFY boaaa::DLInterface90

#define LLVM_VERSION_UNIFY_FINALIZE ;

#include "boaaa/lv/unify_LLVM_VERSION.def"
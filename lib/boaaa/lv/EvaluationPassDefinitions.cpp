#include "boaaa/lv/EvaluationPassDefinitions.h"

#include "boaaa/lv/initalize_pass_end_boaaa.h"

using namespace llvm;
/*
 * Instanziation of all EvaluationPasses in Alphabetic order (LLVM-BUILT-IN)
 */

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ANDERSAA

BOAAA_CREATE_EVAL_PASS_SOURCE(AndersAAEvalWrapperPass, "anders-aa-eval", "Anders Alias Analysis Evaluator",
							  CFLAndersAAWrapperPass)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ANDERSAA
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++BASIC-AA

BOAAA_CREATE_EVAL_PASS_SOURCE(BasicAAEvalWrapperPass, "basic-aa-eval", "BASIC AA Alias Analysis Evaluator",
							  BasicAAWrapperPass)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++BASIC-AA
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++OBJ-CARC
using namespace objcarc;
BOAAA_CREATE_EVAL_PASS_SOURCE(ObjCARCAAEvalWrapperPass, "objcarc-aa-eval", "ObjCARC Alias Analysis Evaluator",
							  ObjCARCAAWrapperPass)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++OBJ-CARC
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SVECAA

BOAAA_CREATE_EVAL_PASS_SOURCE(SCEVAAEvalWrapperPass, "scev-aa-eval", "SVEC Alias Analysis Evaluator",
						      SCEVAAWrapperPass)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SVECAA
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ScopedNoAliasAA

BOAAA_CREATE_EVAL_PASS_SOURCE(ScopedNoAliasEvalWrapperPass,	"sna-aa-eval", "Scoped no Alias Alias Analysis Evaluator",
							  ScopedNoAliasAAWrapperPass)

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ScopedNoAliasAA
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SteensAA

BOAAA_CREATE_EVAL_PASS_SOURCE(SteensAAEvalWrapperPass, "steens-aa-eval", "Steens Alias Analysis Evaluator",
							  CFLSteensAAWrapperPass)

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SteensAA
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++TypeBasedAA

BOAAA_CREATE_EVAL_PASS_SOURCE(TypeBasedAAEvalWrapperPass, "tbaa-aa-eval", "Type based Alias Analysis Alias Analysis Evaluator",
							  TypeBasedAAWrapperPass)

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++TypeBasedAA

//======================================================================================================
//||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||
//======================================================================================================

/*
 * Instanziation of all EvaluationPasses in Alphabetic order (LLVM-EXTENDING)
 */

#ifdef LLVM_VERSION_50
 //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SEA-DSAAA
#ifdef SEA_DSA
using namespace sea_dsa;

BOAAA_CREATE_EVAL_PASS_SOURCE(ContextSensitiveSeaDsaEvalWrapperPass, "cs-sea-dsa-eval",	"Sea Dsa Context Sensitive Alias Analysis Evaluator",
							  ContextSensitiveSeaDsaWrapperPass)

BOAAA_CREATE_EVAL_PASS_SOURCE(ContextSensitiveBottomUpTopDownSeaDsaEvalWrapperPass, "cs-bu-sea-dsa-eval", "Sea Dsa Context Sensitive Bottom Up Alias Analysis Evaluator",
							  ContextSensitiveBottomUpTopDownSeaDsaWrapperPass)

BOAAA_CREATE_EVAL_PASS_SOURCE(BottomUpSeaDsaEvalWrapperPass, "bu-sea-dsa-eval",	"Sea Dsa Bottum Up Alias Analysis Evaluator",
							  BottomUpSeaDsaWrapperPass)

BOAAA_CREATE_EVAL_PASS_SOURCE(ContextInsensitiveSeaDsaEvalWrapperPass, "cis-sea-dsa-eval", "Sea Dsa Context Insensitive Alias Analysis Evaluator",
							  ContextInsensitiveSeaDsaWrapperPass)

BOAAA_CREATE_EVAL_PASS_SOURCE(FlatMemorySeaDsaEvalWrapperPass, "fm-sea-dsa-eval", "Sea Dsa Flat Memory Alias Analysis Evaluator",
							  FlatMemorySeaDsaWrapperPass)

#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SEA-DSAAA
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SFSAA

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SFSAA
#endif //!LLVM_VERSION_50

BOAAA_CREATE_EVAL_PASS_SOURCE(ClangEvalWrapperPass, "CLANG", "Clang Analysis Pipeline", 
					          BasicAAWrapperPass, ScopedNoAliasAAWrapperPass, TypeBasedAAWrapperPass)
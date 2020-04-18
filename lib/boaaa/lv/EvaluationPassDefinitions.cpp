#include "boaaa/lv/EvaluationPassDefinitions.h"

#include "boaaa/lv/initalize_pass_end_boaaa.h"

using namespace llvm;
/*
 * Instanziation of all EvaluationPasses in Alphabetic order (LLVM-BUILT-IN)
 */

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ANDERSAA

BOAAA_CREATE_EVAL_PASS_SOURCE(AndersAAEvalWrapperPass, CFLAndersAAWrapperPass, 
	"anders-aa-eval", "Anders Alias Analysis Evaluator")

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ANDERSAA
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++BASIC-AA

BOAAA_CREATE_EVAL_PASS_SOURCE(BasicAAEvalWrapperPass, BasicAAWrapperPass, 
	"basic-aa-eval", "BASIC AA Alias Analysis Evaluator")

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++BASIC-AA
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++OBJ-CARC
using namespace objcarc;
BOAAA_CREATE_EVAL_PASS_SOURCE(ObjCARCAAEvalWrapperPass, ObjCARCAAWrapperPass,
	"objcarc-aa-eval", "ObjCARC Alias Analysis Evaluator")

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++OBJ-CARC
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SVECAA

BOAAA_CREATE_EVAL_PASS_SOURCE(SCEVAAEvalWrapperPass, SCEVAAWrapperPass,
	"scev-aa-eval", "SVEC Alias Analysis Evaluator")

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SVECAA
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ScopedNoAliasAA

BOAAA_CREATE_EVAL_PASS_SOURCE(ScopedNoAliasEvalWrapperPass, ScopedNoAliasAAWrapperPass,
	"sna-aa-eval", "Scoped no Alias Alias Analysis Evaluator")

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ScopedNoAliasAA
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SteensAA

BOAAA_CREATE_EVAL_PASS_SOURCE(SteensAAEvalWrapperPass, CFLSteensAAWrapperPass,
	"steens-aa-eval", "Steens Alias Analysis Evaluator")

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SteensAA
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++TypeBasedAA

BOAAA_CREATE_EVAL_PASS_SOURCE(TypeBasedAAEvalWrapperPass, TypeBasedAAWrapperPass,
	"tbaa-aa-eval", "Type based Alias Analysis Alias Analysis Evaluator")

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

BOAAA_CREATE_EVAL_PASS_SOURCE(SeaDsaEvalWrapperPass, SeaDsaWrapperPass, "sea-dsa", "Sea-Dsa Alias Analysis")

#endif
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SEA-DSAAA
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SFSAA

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SFSAA
#endif //!LLVM_VERSION_50
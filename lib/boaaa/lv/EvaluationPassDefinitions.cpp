#include "boaaa/lv/EvaluationPassDefinitions.h"
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"

#include "boaaa/lv/initalize_pass_end_boaaa.h"

using namespace boaaa;

using namespace llvm;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SVECAA

char SVECAAEvalWrapperPass::ID = 0;

INITIALIZE_PASS_BEGIN(SVECAAEvalWrapperPass, "svec-aa-eval", "SVEC Alias Analysis Evaluator", false, false)
INITIALIZE_PASS_DEPENDENCY(SCEVAAWrapperPass)
INITIALIZE_PASS_END_BOAAA(SVECAAEvalWrapperPass, "svec-aa-eval", "SVEC Alias Analysis Evaluator", false, false)


SVECAAEvalWrapperPass::SVECAAEvalWrapperPass() : impl(), LLVMModulePass(ID) 
{
    initializeSVECAAEvalWrapperPassPass(*PassRegistry::getPassRegistry());
}

bool SVECAAEvalWrapperPass::runOnModule(LLVMModule& M) 
{
    auto& WrapperPass = getAnalysis<SCEVAAWrapperPass>();
    std::unique_ptr<llvm::AAResults> result;
    result->addAAResult(WrapperPass.getResult());
    impl.evaluateAAResult(std::move(result), M);
    return false;
}

void SVECAAEvalWrapperPass::getAnalysisUsage(llvm::AnalysisUsage& AU) const
{
    AU.setPreservesAll();
    AU.addRequired<SCEVAAWrapperPass>();
}

LLVMModulePass* boaaa::createSVECAAEVALWrapperPass()
{
    return new SVECAAEvalWrapperPass();
}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SVECAA
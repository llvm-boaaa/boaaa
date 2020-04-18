#include "boaaa/lv/50/SeaDsaWrapper.h"

#ifdef SEA_DSA
using namespace llvm;

SeaDsaResult::SeaDsaResult() { }
SeaDsaResult::~SeaDsaResult() { }

AliasResult SeaDsaResult::alias(const MemoryLocation& LocA, const MemoryLocation& LocB)
{
    return MayAlias;
}


bool SeaDsaWrapperPass::runOnModule(Module& M)
{
    Result.reset(new SeaDsaResult());
    sea_dsa::DsaAnalysis& dsa = getAnalysis<sea_dsa::DsaAnalysis>();    
    sea_dsa::GlobalAnalysis& glob = dsa.getDsaAnalysis();

    return false;
}

char SeaDsaWrapperPass::ID = 0;

INITIALIZE_PASS(SeaDsaWrapperPass, "sea-dsa-wrapper", "SeaDsa Wrapper that implements llvm standart analysis interface", false, true)                                                        

SeaDsaWrapperPass::SeaDsaWrapperPass() : ModulePass(ID) {
    initializeSeaDsaWrapperPassPass(*PassRegistry::getPassRegistry());
}

SeaDsaWrapperPass::~SeaDsaWrapperPass() { }

ModulePass* createSeaDsaWrapperPass() {
    return new SeaDsaWrapperPass();
}

void SeaDsaWrapperPass::getAnalysisUsage(AnalysisUsage& AU) const
{
    AU.addRequired<sea_dsa::DsaAnalysis>();
    AU.setPreservesAll();
}

#endif //!SEA_DSA
#ifndef BOAAA_SEA_DSA_WRAPPER_H
#define BOAAA_SEA_DSA_WRAPPER_H

#include "llvm/PassRegistry.h"
#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"

#ifdef SEA_DSA

#include "sea_dsa/DsaAnalysis.hh"

using namespace llvm;

namespace llvm
{
	class SeaDsaResult : public AAResultBase<SeaDsaResult> {
		friend AAResultBase<SeaDsaResult>;

        sea_dsa::DsaAnalysis& analysis;
        sea_dsa::GlobalAnalysis& global;

	public:
        SeaDsaResult(sea_dsa::DsaAnalysis& Analysis);
        ~SeaDsaResult();

        AliasResult alias(const MemoryLocation& LocA, const MemoryLocation& LocB);

        /* dont implemented maybe when it is easy later */
        //ModRefInfo getModRefInfo(ImmutableCallSite CS, const MemoryLocation& Loc);

        //ModRefInfo getModRefInfo(ImmutableCallSite CS1, ImmutableCallSite CS2);

        /// Chases pointers until we find a (constant global) or not.
        //bool pointsToConstantMemory(const MemoryLocation& Loc, bool OrLocal);

        /// Get the location associated with a pointer argument of a callsite.
        //ModRefInfo getArgModRefInfo(ImmutableCallSite CS, unsigned ArgIdx);

        /// Returns the behavior when calling the given call site.
        //FunctionModRefBehavior getModRefBehavior(ImmutableCallSite CS);

        /// Returns the behavior when calling the given function. For use when the
        /// call site is not known.
        //FunctionModRefBehavior getModRefBehavior(const Function* F);
	};

	class SeaDsaWrapperPass : public ModulePass
	{
        std::unique_ptr<SeaDsaResult> Result;

    public:
        static char ID;

        SeaDsaWrapperPass();
        ~SeaDsaWrapperPass();

        SeaDsaResult& getResult() { return *Result; }
        const SeaDsaResult& getResult() const { return *Result; }

        void getAnalysisUsage(AnalysisUsage& AU) const override;
		bool runOnModule(Module& M) override;
	};

    ModulePass* createSeaDsaWrapperPass();
    void initializeSeaDsaWrapperPassPass(PassRegistry &Registry);
}

#endif //!Sea_DSA

#endif
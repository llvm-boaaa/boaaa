#ifndef BOAAA_LV_EVALUATION_PASS_DEFINITIONS_H
#define BOAAA_LV_EVALUATION_PASS_DEFINITIONS_H

#include "include_versions/LLVM_include_Pass_h.inc"
#include "include_versions/LLVM_Module.inc"
#include "include_versions/LLVM_ModulePass.inc"
#include "include_versions/LLVM_FunctionPass.inc"

#include "boaaa/lv/EvaluationPass.h"
#include "boaaa/lv/TimePass.h"
#include "boaaa/lv/version_context.h"
#include "boaaa/support/select_type.h"

//include analysis
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/CFLAndersAliasAnalysis.h"
#include "llvm/Analysis/CFLSteensAliasAnalysis.h"
#include "llvm/Analysis/ObjCARCAliasAnalysis.h"
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"
#include "llvm/Analysis/ScopedNoAliasAA.h"
#include "llvm/Analysis/TypeBasedAliasAnalysis.h"

namespace boaaa {

	namespace detail
	{
		template<class PASS>
		class EvalModulePass : public LLVMModulePass
		{
		private:
			EvaluationPassImpl* impl;
			version_context* context;

		public:
			EvalModulePass(char ID) : LLVMModulePass(ID) { impl = new EvaluationPassImpl(); }
			~EvalModulePass() { delete impl; }

			bool runOnModule(LLVMModule& M) override
			{
				auto& TLI = getAnalysis<llvm::TargetLibraryInfoWrapperPass>();
				auto& WrapperPass = getAnalysis<boaaa::TimePass<PASS>>();
				llvm::AAResults result(TLI.getTLI());
				result.addAAResult(WrapperPass.getResult());
				impl->evaluateAAResultOnModule(M, result, context->relevant_pointers);
				return false;
			}

			void getAnalysisUsage(llvm::AnalysisUsage& AU) const override
			{
				AU.addRequired<llvm::TargetLibraryInfoWrapperPass>();
				AU.addRequired<boaaa::TimePass<PASS>>();
				AU.setPreservesAll();
			}

			void setContext(version_context* _context) { context = _context; }

			void printResult(std::ostream& stream) {
				impl->printResult(stream);
			}

			void printToEvalRes(EvaluationResult& er) {
				impl->printToEvalRes(er);
			}
		};

		template<class PASS>
		class EvalFunctionPass : public LLVMFunctionPass
		{
		private:
			EvaluationPassImpl* impl;
			version_context* context;

		public:
			EvalFunctionPass(char ID) : LLVMFunctionPass(ID) { impl = new EvaluationPassImpl(); }
			~EvalFunctionPass() { delete impl; }

			bool runOnFunction(LLVMFunction& F) override
			{
				auto& TLI = getAnalysis<llvm::TargetLibraryInfoWrapperPass>();
				auto& WrapperPass = getAnalysis<boaaa::TimePass<PASS>>();
				llvm::AAResults result(TLI.getTLI());
				result.addAAResult(WrapperPass.getResult());
				impl->evaluateAAResultOnFunction(F, result, *context->relevant_pointers[F.getGUID()]);
				return false;
			}

			void getAnalysisUsage(llvm::AnalysisUsage& AU) const override
			{
				AU.addRequired<llvm::TargetLibraryInfoWrapperPass>();
				AU.addRequired<boaaa::TimePass<PASS>>();
				AU.setPreservesAll();
			}

			void setContext(version_context* _context) { context = _context; }

			void printResult(std::ostream& stream) {
				impl->printResult(stream);
			}

			void printToEvalRes(EvaluationResult& er) {
				impl->printToEvalRes(er);
			}
		};

		template<typename PASS>
		struct select_eval_pass {
			using type = typename select_type_reverse_for<std::is_base_of, PASS,
														  LLVMModulePass, EvalModulePass<PASS>,
														  LLVMFunctionPass, EvalFunctionPass<PASS>>::type;
		};

		template<class PASS>
		using select_eval_pass_t = typename select_eval_pass<PASS>::type;

		template<typename PASS>
		struct select_base_pass {
			using type = typename select_type_reverse<std::is_base_of, PASS,
													  LLVMModulePass,
													  LLVMFunctionPass>::type;
		};

		template<class PASS>
		using select_base_pass_t = typename select_base_pass<PASS>::type;
	}
}

#ifndef BOAAA_CREATE_EVAL_PASS_HEADER
#define BOAAA_CREATE_EVAL_PASS_HEADER(passname, analysisname)										\
	class passname : public boaaa::detail::select_eval_pass_t<analysisname>							\
	{																								\
	public:																							\
		static char ID;																				\
		passname();																					\
	};																								\
	boaaa::detail::select_base_pass_t<analysisname>*												\
		create##passname();																			\
	void initialize##passname##Pass(PassRegistry& Registry);
#endif

#ifndef BOAAA_CREATE_EVAL_PASS_SOURCE 
#define BOAAA_CREATE_EVAL_PASS_SOURCE(passname, analysisname, arg, help)							\
    char passname::ID = 0;                                                                          \
    INITIALIZE_PASS_BEGIN(passname, arg, help, false, true)                                         \
    INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfoWrapperPass)                                        \
    INITIALIZE_PASS_DEPENDENCY(analysisname)                                                        \
    INITIALIZE_PASS_END(passname, arg, help, false, true)											\
    passname::passname()																			\
			: boaaa::detail::select_eval_pass_t<analysisname>(ID)									\
    { initialize##passname##Pass(*PassRegistry::getPassRegistry()); }                               \
    boaaa::detail::select_base_pass_t<analysisname>* create##passname() { return new passname(); }
#endif

#ifndef BOAAA_CREATE_EVAL_PASS_SOURCE_ADD_INST
#define BOAAA_CREATE_EVAL_PASS_SOURCE_ADD_INST(passname, analysisname, arg, help, inst)				\
    char passname::ID = 0;                                                                          \
    INITIALIZE_PASS_BEGIN(passname, arg, help, false, true)                                         \
    INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfoWrapperPass)                                        \
    INITIALIZE_PASS_DEPENDENCY(analysisname)                                                        \
    INITIALIZE_PASS_END(passname, arg, help, false, true)											\
    passname::passname()																			\
			: boaaa::detail::select_eval_pass_t<analysisname>(ID)									\
    { initialize##passname##Pass(*PassRegistry::getPassRegistry()); inst; }                         \
    boaaa::detail::select_base_pass_t<analysisname>* create##passname() { return new passname(); }

#endif

#ifndef BOAAA_CREATE_RENAME_PASS
#define BOAAA_CREATE_RENAME_PASS(newname, passname, inst)											\
class newname : public passname																		\
{																									\
public:																								\
	newname() : passname() { inst; }																\
};																									\
inline void initialize##newname##Pass(PassRegistry& Registry) { initialize##passname##Pass(Registry); }	

#endif

namespace llvm
{
	/*
	 * Instanziation of all EvaluationPasses in Alphabetic order (LLVM-BUILT-IN)
	 */

	 //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++AndersAA

	BOAAA_CREATE_EVAL_PASS_HEADER(AndersAAEvalWrapperPass,			CFLAndersAAWrapperPass)

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++AndersAA
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++BASIC-AA
	BOAAA_CREATE_EVAL_PASS_HEADER(BasicAAEvalWrapperPass,			BasicAAWrapperPass)

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++BASIC-AA
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++OBJ-CARC
	using namespace objcarc;
	BOAAA_CREATE_EVAL_PASS_HEADER(ObjCARCAAEvalWrapperPass,			ObjCARCAAWrapperPass)

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++OBJ-CARC
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SCEVAA

	BOAAA_CREATE_EVAL_PASS_HEADER(SCEVAAEvalWrapperPass,			SCEVAAWrapperPass)

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SCEVAA
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ScopedNoAliasAA

	BOAAA_CREATE_EVAL_PASS_HEADER(ScopedNoAliasEvalWrapperPass,		ScopedNoAliasAAWrapperPass)

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ScopedNoAliasAA
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SteensAA

	BOAAA_CREATE_EVAL_PASS_HEADER(SteensAAEvalWrapperPass,			CFLSteensAAWrapperPass)

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SteensAA
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++TypeBasedAA

	BOAAA_CREATE_EVAL_PASS_HEADER(TypeBasedAAEvalWrapperPass,		TypeBasedAAWrapperPass)

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++TypeBasedAA

	//======================================================================================================
	//||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||==||
	//======================================================================================================

	/*
	* Instanziation of all EvaluationPasses in Alphabetic order (LLVM-EXTENDING)
	*/

#ifdef LLVM_VERSION_50
} //!llvm namespace
	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SEA-DSAAA
#ifdef SEA_DSA

#include "sea_dsa/DsaAnalysis.hh"
#include "50/SeaDsaWrapper.h"

namespace llvm {
	using namespace sea_dsa;

	namespace {
		inline void changeDsaOption(GlobalAnalysisKind kind) {
			reinterpret_cast<cl::opt<::sea_dsa::GlobalAnalysisKind>*>(cl::getRegisteredOptions().find("sea-dsa")->second)->setValue(kind);
		}
	}


	//rename passes, so they get show correctly in JSON export
	BOAAA_CREATE_RENAME_PASS(ContextSensitiveSeaDsaWrapperPass,					SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::CONTEXT_SENSITIVE))

	BOAAA_CREATE_RENAME_PASS(ContextSensitiveBottomUpTopDownSeaDsaWrapperPass,	SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::BUTD_CONTEXT_SENSITIVE))

	BOAAA_CREATE_RENAME_PASS(BottomUpSeaDsaWrapperPass,							SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::BU))

	BOAAA_CREATE_RENAME_PASS(ContextInsensitiveSeaDsaWrapperPass,				SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::CONTEXT_INSENSITIVE))

	BOAAA_CREATE_RENAME_PASS(FlatMemorySeaDsaWrapperPass,						SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::FLAT_MEMORY))

	//create all evaluationpasses for sea_dsa
	BOAAA_CREATE_EVAL_PASS_HEADER(ContextSensitiveSeaDsaEvalWrapperPass,				ContextSensitiveSeaDsaWrapperPass)
		
	BOAAA_CREATE_EVAL_PASS_HEADER(ContextSensitiveBottomUpTopDownSeaDsaEvalWrapperPass,	ContextSensitiveBottomUpTopDownSeaDsaWrapperPass)
		
	BOAAA_CREATE_EVAL_PASS_HEADER(BottomUpSeaDsaEvalWrapperPass,						BottomUpSeaDsaWrapperPass)

	BOAAA_CREATE_EVAL_PASS_HEADER(ContextInsensitiveSeaDsaEvalWrapperPass,				ContextInsensitiveSeaDsaWrapperPass)

	BOAAA_CREATE_EVAL_PASS_HEADER(FlatMemorySeaDsaEvalWrapperPass,						FlatMemorySeaDsaWrapperPass)

}
#endif

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SEA-DSAAA
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SFSAA


namespace llvm {


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SFSAA
#endif //!LLVM_VERSION_50
}

#endif
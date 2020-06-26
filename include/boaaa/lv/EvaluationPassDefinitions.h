#ifndef BOAAA_LV_EVALUATION_PASS_DEFINITIONS_H
#define BOAAA_LV_EVALUATION_PASS_DEFINITIONS_H

#include "include_versions/LLVM_include_Pass_h.inc"
#include "include_versions/LLVM_Module.inc"
#include "include_versions/LLVM_ModulePass.inc"
#include "include_versions/LLVM_FunctionPass.inc"

#include "boaaa/lv/EvaluationPass.h"
#include "boaaa/lv/TimePass.h"
#include "boaaa/lv/version_context.h"
#include "boaaa/support/consume_macro.h"
#include "boaaa/support/data_store.h"
#include "boaaa/support/select_type.h"

//include analysis
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/CFLAndersAliasAnalysis.h"
#include "llvm/Analysis/CFLSteensAliasAnalysis.h"
#include "llvm/Analysis/ObjCARCAliasAnalysis.h"
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"
#include "llvm/Analysis/ScopedNoAliasAA.h"
#include "llvm/Analysis/TypeBasedAliasAnalysis.h"
#include "llvm/Analysis/GlobalsModRef.h"

namespace boaaa {

	namespace detail
	{
		template<class PASS>
		class AnalysisGetter
		{
			PASS* Pass;
		public:
			AnalysisGetter() : Pass(nullptr) { }

			void addAAResult(LLVMFunctionPass* pass, LLVMAAResults& result) 
			{
				if (Pass == nullptr)
					Pass = &pass->getAnalysis<PASS>();
				result.addAAResult(Pass->getResult());
			}
		};

		template<size_t N, class ...PASSES>
		struct EvalRecursivePasses;

		template<size_t N, class ...PASSES>
		struct EvalRecursivePasses
		{
		private:
			using store = _data_store<TimePass<PASSES>...>;
		public:
			using type = typename get_helper<N, store>::type;
			typedef data_store<AnalysisGetter<TimePass<PASSES>>...> getter;

			static void getAnalysisUsage(llvm::AnalysisUsage& AU)
			{
				EvalRecursivePasses<N - 1, PASSES...>::getAnalysisUsage(AU);
				AU.addRequired<type>();
			}

			static void addAAResult(LLVMFunctionPass* FP, LLVMAAResults& result, getter& passes)
			{
				EvalRecursivePasses<N - 1, PASSES...>::addAAResult(FP, result, passes);
				AnalysisGetter<type> getter = passes.get<N>();
				getter.addAAResult(FP, result);
			}
		};

		template<class ...PASSES>
		struct EvalRecursivePasses<0, PASSES...>
		{
		private:
			using store = _data_store<TimePass<PASSES>...>;
		public:
			using type = typename get_helper<0, store>::type;
			typedef data_store<AnalysisGetter<TimePass<PASSES>>...> getter;

			static void getAnalysisUsage(llvm::AnalysisUsage& AU)
			{
				AU.addRequired<type>();
			}

			static void addAAResult(LLVMFunctionPass* FP, LLVMAAResults& result, getter& passes)
			{
				AnalysisGetter<type> getter = passes.get<0>();
				getter.addAAResult(FP, result);
			}
		};

		template<class ...PASSES>
		class EvalPass : public LLVMFunctionPass
		{
		private:
			static constexpr size_t num = sizeof...(PASSES);

			typedef data_store<AnalysisGetter<TimePass<PASSES>>...> _getter;
			EvaluationPassImpl* impl;
			version_context* context;
			_getter getter;
			aa_id id;

			void checkIfSetsAreInitalized(version_context* _context, aa_id id) {
				_raw_type_inst(_context->alias_sets)::const_iterator it_aa = _context->alias_sets.find(id);
				if (it_aa == _context->alias_sets.end()) {
					_context->alias_sets.insert({ id, std::make_unique<evaluation_sets>() });
				}
				_raw_type_inst(_context->no_alias_sets)::const_iterator it_no_aa = _context->no_alias_sets.find(id);
				if (it_no_aa == _context->no_alias_sets.end()) {
					_context->no_alias_sets.insert({ id, std::make_unique<evaluation_explicite_sets>() });
				}
			}

		public:
			EvalPass(char ID) : LLVMFunctionPass(ID), impl(new EvaluationPassImpl()), context(nullptr), getter({ AnalysisGetter<TimePass<PASSES>>()... }) { }
			~EvalPass() { delete impl; }

			bool runOnFunction(LLVMFunction& F) override
			{
				auto& TLI = getAnalysis<llvm::TargetLibraryInfoWrapperPass>();
				LLVMAAResults result(TLI.getTLI());
				EvalRecursivePasses<num - 1,PASSES...>::addAAResult(static_cast<LLVMFunctionPass*>(this), result, getter);
				impl->evaluateAAResultOnFunction(F, result, *context->relevant_pointers[F.getGUID()]);
				return false;
			}

			void getAnalysisUsage(llvm::AnalysisUsage& AU) const override
			{
				AU.addRequired<llvm::TargetLibraryInfoWrapperPass>();
				EvalRecursivePasses<num - 1, PASSES...>::getAnalysisUsage(AU);
				AU.setPreservesAll();
			}

			void setContext(version_context* _context, aa_id _id) { 
				context = _context;
				id = _id;
				if (context->store_aa_results) {
					checkIfSetsAreInitalized(context, id);
					impl->setSets(context->alias_sets[id].get(), context->no_alias_sets[id].get());
				}
			}

			void printResult(std::ostream& stream) {
				impl->printResult(stream);
			}

			void printToEvalRes(EvaluationResult& er) {
				impl->printToEvalRes(er);
			}
		};

		template<typename PASS>
		struct select_eval_pass {
			using type = typename EvalPass<PASS>;
		};

		template<class PASS>
		using select_eval_pass_t = typename select_eval_pass<PASS>::type;

		template<typename PASS>
		struct select_base_pass {
			using type = typename LLVMFunctionPass;
		};

		template<class PASS>
		using select_base_pass_t = typename select_base_pass<PASS>::type;
	}
}

#ifndef BOAAA_CREATE_EVAL_PASS_HEADER_START
#define BOAAA_CREATE_EVAL_PASS_HEADER_START(passname, ...) NOTHING									\
	class passname : public boaaa::detail::EvalPass<__VA_ARGS__>									\
	{																								\
	public:																							\
		using timepass = boaaa::ConcatTimePass<__VA_ARGS__											
#endif

#ifndef BOAAA_CREATE_EVAL_PASS_HEADER_ADDITIONAL_TIME_ANALYSIS
#define BOAAA_CREATE_EVAL_PASS_HEADER_ADDITIONAL_TIME_ANALYSIS(...) NOTHING BOAAA_COMMA __VA_ARGS__
#endif

#ifndef BOAAA_CREATE_EVAL_PASS_HEADER_END
#define BOAAA_CREATE_EVAL_PASS_HEADER_END(passname, ...) 											\
															>;										\
		static char ID;																				\
		passname();																					\
		timepass* createTimePass();																	\
};																									\
LLVMFunctionPass * create##passname();																\
	void initialize##passname##Pass(PassRegistry& Registry);
#endif

#ifndef BOAAA_CREATE_EVAL_PASS_HEADER
#define BOAAA_CREATE_EVAL_PASS_HEADER(passname, ...) NOTHING										\
BOAAA_CREATE_EVAL_PASS_HEADER_START(passname, __VA_ARGS__)											\
BOAAA_CREATE_EVAL_PASS_HEADER_END(passname, __VA_ARGS__)
#endif

#ifndef BOAAA_CREATE_EVAL_PASS_SOURCE 
#define BOAAA_CREATE_EVAL_PASS_SOURCE(passname, arg, help, ...) NOTHING 							\
    char passname::ID = 0;                                                                          \
    INITIALIZE_PASS_BEGIN(passname, arg, help, false, true)                                         \
    INITIALIZE_PASS_DEPENDENCY(TargetLibraryInfoWrapperPass)                                        \
    BOAAA_CONSUME(INITIALIZE_PASS_DEPENDENCY, __VA_ARGS__)	    									\
    INITIALIZE_PASS_END(passname, arg, help, false, true)											\
    passname::passname() : boaaa::detail::EvalPass<__VA_ARGS__>(ID)									\
    { initialize##passname##Pass(*PassRegistry::getPassRegistry()); }                               \
	passname::timepass* passname::createTimePass() {return new passname::timepass(); }				\
    LLVMFunctionPass* create##passname() { return new passname(); }
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

	namespace 
	{
		//void enableTBAA() {
			//reinterpret_cast<cl::opt<bool>*>(cl::getRegisteredOptions().find("enable-tbaa")->second)->setValue(false);
		//}
	}


	//clang -cc1 -O1 -no-struct-path-tbaa -disable-llvm-optzns foo.cpp -emit-llvm -o -
	// -g generate metadata
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

		inline void changeTDBU() {
			reinterpret_cast<cl::opt<bool>*>(cl::getRegisteredOptions().find("sea-dsa-no-td-copying-opt")->second)->setValue(true);
		}
	}


	//rename passes, so they get show correctly in JSON export
	BOAAA_CREATE_RENAME_PASS(ContextSensitiveSeaDsaWrapperPass, SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::CONTEXT_SENSITIVE))

	BOAAA_CREATE_RENAME_PASS(ContextSensitiveBottomUpTopDownSeaDsaWrapperPass, SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::BUTD_CONTEXT_SENSITIVE); changeTDBU())

	BOAAA_CREATE_RENAME_PASS(BottomUpSeaDsaWrapperPass,							SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::BU))

	BOAAA_CREATE_RENAME_PASS(ContextInsensitiveSeaDsaWrapperPass,				SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::CONTEXT_INSENSITIVE))

	BOAAA_CREATE_RENAME_PASS(FlatMemorySeaDsaWrapperPass,						SeaDsaWrapperPass, changeDsaOption(sea_dsa::GlobalAnalysisKind::FLAT_MEMORY))

	//create all evaluationpasses for sea_dsa
	BOAAA_CREATE_EVAL_PASS_HEADER_START(ContextSensitiveSeaDsaEvalWrapperPass,				    ContextSensitiveSeaDsaWrapperPass)
	BOAAA_CREATE_EVAL_PASS_HEADER_ADDITIONAL_TIME_ANALYSIS(DsaAnalysis)
	BOAAA_CREATE_EVAL_PASS_HEADER_END(ContextSensitiveSeaDsaEvalWrapperPass,				    ContextSensitiveSeaDsaWrapperPass)
		
	BOAAA_CREATE_EVAL_PASS_HEADER_START(ContextSensitiveBottomUpTopDownSeaDsaEvalWrapperPass,	ContextSensitiveBottomUpTopDownSeaDsaWrapperPass)
	BOAAA_CREATE_EVAL_PASS_HEADER_ADDITIONAL_TIME_ANALYSIS(DsaAnalysis)
	BOAAA_CREATE_EVAL_PASS_HEADER_END(ContextSensitiveBottomUpTopDownSeaDsaEvalWrapperPass,     ContextSensitiveBottomUpTopDownSeaDsaWrapperPass)
		
	BOAAA_CREATE_EVAL_PASS_HEADER_START(BottomUpSeaDsaEvalWrapperPass,						    BottomUpSeaDsaWrapperPass)
	BOAAA_CREATE_EVAL_PASS_HEADER_ADDITIONAL_TIME_ANALYSIS(DsaAnalysis)
	BOAAA_CREATE_EVAL_PASS_HEADER_END(BottomUpSeaDsaEvalWrapperPass,                            BottomUpSeaDsaWrapperPass)

	BOAAA_CREATE_EVAL_PASS_HEADER_START(ContextInsensitiveSeaDsaEvalWrapperPass,				ContextInsensitiveSeaDsaWrapperPass)
	BOAAA_CREATE_EVAL_PASS_HEADER_ADDITIONAL_TIME_ANALYSIS(DsaAnalysis)
	BOAAA_CREATE_EVAL_PASS_HEADER_END(ContextInsensitiveSeaDsaEvalWrapperPass,                  ContextInsensitiveSeaDsaWrapperPass)

	BOAAA_CREATE_EVAL_PASS_HEADER_START(FlatMemorySeaDsaEvalWrapperPass,						FlatMemorySeaDsaWrapperPass)
	BOAAA_CREATE_EVAL_PASS_HEADER_ADDITIONAL_TIME_ANALYSIS(DsaAnalysis)
	BOAAA_CREATE_EVAL_PASS_HEADER_END(FlatMemorySeaDsaEvalWrapperPass,						    FlatMemorySeaDsaWrapperPass)

}
#endif

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SEA-DSAAA
    //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SFSAA


namespace llvm {

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++SFSAA
#endif //!LLVM_VERSION_50


	/*
	 * Instanziation of all EvaluationPasses in Alphabetic order (COMBINED)
	 */

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++CLANG_DEFAULT

	BOAAA_CREATE_EVAL_PASS_HEADER(ClangEvalWrapperPass, BasicAAWrapperPass, ScopedNoAliasAAWrapperPass, TypeBasedAAWrapperPass, GlobalsAAWrapperPass)


	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++CLANG_DEFAULT
}

#endif
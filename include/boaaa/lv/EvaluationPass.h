#ifndef EVALUATION_PASS_H
#define EVALUATION_PASS_H

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ combine versions

//define LLVMAAResults
#include "include_versions/LLVM_AAResults.inc"
//define LLVMAliasResult
#include "include_versions/LLVM_AliasResult.inc"
//define LLVMCallUnifyer
#include "include_versions/LLVM_CallVersionUnifyer.inc"
//define LLVMDenseMapInfo
#include "include_versions/LLVM_DenseMapInfo.inc"
//define LLVMFunction
#include "include_versions/LLVM_Function.inc"
//define LLVMModule
#include "include_versions/LLVM_Module.inc"
//include llvm/Pass.h
#include "include_versions/LLVM_include_Pass_h.inc"
//define LLVMInstruction
#include "include_versions/LLVM_Instruction.inc"
//define LLVMLoadInst
#include "include_versions/LLVM_LoadInst.inc"
//define LLVMLocationSize
#include "include_versions/LLVM_LocationSize.inc"
//define LLVMMemoryLocation
#include "include_versions/LLVM_MemoryLocation.inc"
//define LLVMModRefInfo
#include "include_versions/LLVM_ModRefInfo.inc"
//define LLVMModulePass
#include "include_versions/LLVM_ModulePass.inc"
//define LLVMRegisterPass
#include "include_versions/LLVM_RegisterPass.inc"
//define LLVMSetVector
#include "include_versions/LLVM_SetVector.inc"
//define LLVMSmallVector
#include "include_versions/LLVM_SmallVector.inc"
//define LLVMSmallSetVector
#include "include_versions/LLVM_SmallSetVector.inc"
//define LLVMStoreInst
#include "include_versions/LLVM_StoreInst.inc"
//define LLVMType
#include "include_versions/LLVM_Type.inc"
//define LLVMUse
#include "include_versions/LLVM_Use.inc"
//define LLVMValue
#include "include_versions/LLVM_Value.inc"

#include "llvm/IR/InstIterator.h"
#include "llvm/IR/Constants.h"
#include "llvm/Analysis/AliasAnalysis.h"

#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"

#include "boaaa/EvaluationResult.h"
#include "boaaa/lv/EvaluationContainer.h"
#include "boaaa/lv/version_context.h"
#include "boaaa/support/AutoDeleter.h"
#include "boaaa/support/UnionFind.h"
#include "boaaa/support/UnionFindMap.h"
#include "boaaa/support/raw_type.h"

#include <iostream>
#include <chrono>

#ifdef LLVM_VERSION_ERROR_CODE
LLVM_VERSION_ERROR_CODE
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {

	class EvaluationPassImpl
	{
    public:
		using timestamp = typename std::chrono::time_point<std::chrono::high_resolution_clock>;

	private:

		uint16_t m_nanos_alias;
		uint16_t m_micros_alias;
		uint16_t m_millis_alias;
		uint64_t m_seconds_alias;
		uint16_t m_nanos_modref;
		uint16_t m_micros_modref;
		uint16_t m_millis_modref;
		uint64_t m_seconds_modref;

		void addTime(timestamp start, timestamp end, uint16_t& nanos, uint16_t& micros, uint16_t& millis, uint64_t& seconds) {
			nanos += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count() % 1000;
			if (nanos >= 1000) {
				nanos -= 1000U;
				micros++;
			}
			micros += std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() % 1000;
			if (micros >= 1000) {
				micros -= 1000U;
				millis++;
			}

			millis += std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() % 1000;
			if (millis >= 1000) {
				millis -= 1000U;
				seconds++;
			}

			seconds += std::chrono::duration_cast<std::chrono::seconds>(end - start).count();
		}

		timestamp begin() {
			return std::chrono::high_resolution_clock::now();
		}

		void addAliasTime(timestamp start) {
			timestamp end = std::chrono::high_resolution_clock::now();
			addTime(start, end, m_nanos_alias, m_micros_alias, m_millis_alias, m_seconds_alias);
		}

		void addModRefTime(timestamp start) {
			timestamp end = std::chrono::high_resolution_clock::now();
			addTime(start, end, m_nanos_modref, m_micros_modref, m_millis_modref, m_seconds_modref);
		}

	public:
		EvaluationPassImpl() : FunctionCount(), NoAliasCount(), MayAliasCount(), PartialAliasCount(), 
			MustAliasCount(), NoModRefCount(), ModCount(), RefCount(), ModRefCount(), MustCount(), 
			MustRefCount(), MustModCount(), MustModRefCount(), 
			evaluated(false), AliasSetCount(), NoAliasSetCount(),
			MeanAlias(), VarAlias(), MeanNoAlias(), VarNoAlias(),
			m_nanos_alias(), m_micros_alias(), m_millis_alias(), m_seconds_alias(),
			m_nanos_modref(), m_micros_modref(), m_millis_modref(),m_seconds_modref(),
			delete_aa_set(false), delete_no_aa_set(false), alias_set(nullptr), no_alias_set(nullptr) {};

		~EvaluationPassImpl() { deleteSets(); }

		static void scanPointers(LLVMModule& M, evaluation_storage& storage);

		void deleteSets() {
			if (alias_set && delete_aa_set) {
				delete alias_set;
				delete_aa_set = false;
			}
			if (no_alias_set && delete_no_aa_set) {
				delete no_alias_set;
				delete_no_aa_set = false;
			}
		}

		void setSets(evaluation_sets* alias, evaluation_explicite_sets* no_alias) {
			deleteSets();
			alias_set = alias;
			no_alias_set = no_alias;
		}

        void evaluateAAResultOnModule(LLVMModule& M, LLVMAAResults &AAResult, evaluation_storage& storage);
		void evaluateAAResultOnFunction(LLVMFunction& F, LLVMAAResults& AAResult, EvaluationContainer& container);

		void evaluateResult();

		void printResult(::std::ostream &stream);
		void printToEvalRes(EvaluationResult& er);

		//UFM& getUnionFindMap() { return m_ufm; }

	private:
		void checkInitSets() 
		{
			if (!alias_set)
			{
				delete_aa_set = true;
				alias_set = new evaluation_sets();
			}
			if (!no_alias_set)
			{
				delete_no_aa_set = true;
				no_alias_set = new evaluation_explicite_sets();
			}
		}

		//assumes alias_set and no_alias_set are initalized
		unionfind_map* initAASet(uint64_t GUID)
		{
			return alias_set->insert(std::make_pair(
				GUID, std::unique_ptr<unionfind_map>(new unionfind_map())
				)).first->second.get();
		}

		evaluation_sets* initNoAASets(uint64_t GUID, size_t num)
		{
			evaluation_sets* set =  no_alias_set->insert(std::make_pair(
				GUID, std::unique_ptr<evaluation_sets>(new evaluation_sets())
				)).first->second.get();
			//insert number of unionfindmaps as pointers into the container
			for (int i = 0; i < num; i++) 
			{
				set->insert(std::make_pair(i, std::unique_ptr<unionfind_map>(new unionfind_map())));
			}
			return set;
		}

		int64_t FunctionCount;
		int64_t NoAliasCount, MayAliasCount, PartialAliasCount, MustAliasCount;
		int64_t NoModRefCount, ModCount, RefCount, ModRefCount;
		int64_t MustCount, MustRefCount, MustModCount, MustModRefCount;

		evaluation_sets* alias_set;
		evaluation_explicite_sets* no_alias_set;
		bool delete_aa_set;
		bool delete_no_aa_set;

		bool evaluated;
		uint64_t AliasSetCount;
		uint64_t NoAliasSetCount;
		double   MeanAlias;
		double   VarAlias;
		double   MeanNoAlias;
		double   VarNoAlias;
	};
}


#endif // !EVALUATION_PASS_H
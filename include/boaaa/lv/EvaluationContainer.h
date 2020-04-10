#ifndef BOAAA_LV_EVALUATIONCONTAINER_H
#define BOAAA_LV_EVALUATIONCONTAINER_H

#include "include_versions/LLVM_CallVersionUnifyer.inc"
#include "include_versions/LLVM_SmallVector.inc"
#include "include_versions/LLVM_Value.inc"

#include "boaaa/support/AutoDeleter.h"

#include <vector>

namespace boaaa {

	struct EvaluationContainer {

		LLVMValue **            pointers;
		LLVMValue **            loads;
		LLVMValue **	         stores;
		LLVMCallUnifyer **      calls;
		boaaa::support::AutoDeleter<LLVMCallUnifyer**>* autodeleter;

		size_t num_pointers;
		size_t num_loads;
		size_t num_stores;
		size_t num_calls;

		enum class PLS {
			P,
			L,
			S
		};

		EvaluationContainer(size_t _num_pointers = 0, size_t _num_loads = 0, size_t _num_stores = 0, size_t _num_calls = 0)
			: num_pointers(_num_pointers), num_loads(_num_loads), num_stores(_num_stores), num_calls(_num_calls), autodeleter(nullptr)
		{
			pointers = _num_pointers	? new LLVMValue * 			[num_pointers]	: nullptr;
			loads    = _num_loads		? new LLVMValue * 			[num_loads]		: nullptr;
			stores   = _num_stores      ? new LLVMValue *  			[num_stores]    : nullptr;
			calls    = _num_calls		? new LLVMCallUnifyer * 	[num_calls]	    : nullptr;
		}
			
		~EvaluationContainer() {
			if (pointers)
				delete[] pointers;
			if (loads)
				delete[] loads;
			if (stores)
				delete[] stores;
			if (autodeleter)
				delete autodeleter; //may delete every pointer bevore deleting calls
			if (calls)
			    delete[] calls;
		}
		
		//use only for containers that contian LLVMValue*, otherwise set all values manually
		bool store(PLS pls, std::vector<LLVMValue*>::iterator it, std::vector<LLVMValue*>::iterator end) {
			size_t i = 0;
			size_t num;
			switch (pls) {
			case PLS::P: {
				for (num = num_pointers; it != end && i < num; ++i, ++it)
					pointers[i] = *it;

			} break;
			case PLS::L: {
				for (num = num_loads; it != end && i < num; ++i, ++it)
					loads[i] = *it;
			} break;
			case PLS::S: {
				for (num = num_stores; it != end && i < num; ++i, ++it)
					stores[i] = *it;
			} break;
			default:
				assert(false);
				num = 0; //remove warning below
			}
			return it == end && i == num;
		}

		bool storeCalls(LLVMSmallVector<LLVMCallUnifyer*, 16>::iterator it, LLVMSmallVector<LLVMCallUnifyer*, 16>::iterator end) {
			size_t i = 0;
			for (; it != end && i < num_calls; ++i, ++it)
				calls[i] = *it;
			return it == end && i == num_calls;
		}
	};
}

#endif //!BOAAA_LV_EVALUATIONCONTAINER_H
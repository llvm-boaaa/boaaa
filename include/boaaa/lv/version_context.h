#ifndef BOAAA_VERSION_CONTEXT_H
#define BOAAA_VERSION_CONTEXT_H

#include "boaaa/commandline_types.h"
#include "boaaa/lv/sup_LLVM_VERSION.h"
#include "boaaa/lv/EvaluationContainer.h"
#include "boaaa/vp/StringRefVersionParser.h"
#include "boaaa/support/UnionFindMap.h"

namespace boaaa {
	struct version_context;
#ifdef NOT_SUPPORTED_LLVM_VERSION
	struct version_context {};
}
#else
}

#include "boaaa/EvaluationResult.h"
#include "boaaa/lv/include_versions/BOAAA_StringRefVP.inc"
#include "boaaa/lv/include_versions/LLVM_Module.inc"
#include "boaaa/lv/include_versions/LLVM_LLVMContext.inc"

#ifdef LLVM_VERSION_ERROR_CODE
LLVM_VERSION_ERROR_CODE
#endif

#include <memory>
#include <type_traits>
#include <set>

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

namespace boaaa {
	
	typedef std::map<uint64_t, std::unique_ptr<boaaa::EvaluationContainer>>				evaluation_storage;

	typedef support::UnionFindMap<size_t>												unionfind_map;
	typedef std::map<size_t,   std::unique_ptr<unionfind_map>>							evaluation_sets;
	typedef std::map<size_t,   std::unique_ptr<evaluation_sets>>						evaluation_explicite_sets;
	
	typedef std::map<aa_id,    std::unique_ptr<evaluation_sets>>						evaluation_aa_sets;
	typedef std::map<aa_id,    std::unique_ptr<evaluation_explicite_sets>>				evaluation_no_aa_sets;

	struct version_context
	{
//-------------------------------------------------------LLVM_VERSION dependent
		
		BOAAAStringRefVP* string_ref_vp;
		std::unique_ptr<LLVMModule> loaded_module;
		std::unique_ptr<LLVMLLVMContext> context_to_module;
		evaluation_storage relevant_pointers;


//-----------------------------------------------------LLVM_VERSION independent
		evaluation_aa_sets    alias_sets;
		evaluation_no_aa_sets no_alias_sets;
		std::ostream* basic_ostream;
		bool del_strm_after_use;
		std::unique_ptr<ModuleResult> module_result;
		
		version_context() : relevant_pointers(), alias_sets(), no_alias_sets()
		{
			string_ref_vp = nullptr;
			loaded_module.reset(nullptr);
			context_to_module.reset(nullptr);
			module_result.reset(nullptr);
            basic_ostream = nullptr;
			del_strm_after_use = false;
		}

		~version_context() {
			if (string_ref_vp)
				delete string_ref_vp;
			loaded_module.reset(nullptr);
			context_to_module.reset(nullptr);
			module_result.reset(nullptr);
			if (del_strm_after_use)
				delete basic_ostream;
		}
	};
}

#endif // !NOT_SUPPORTED_LLVM_VERSION

#endif // !BOAAA_VERSION_CONTEXT

#include "boaaa/lv/40/Interface_40.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/SourceMgr.h"

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AliasAnalysisEvaluator.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/CFLAndersAliasAnalysis.h"
#include "llvm/Analysis/CFLSteensAliasAnalysis.h"
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"
#include "llvm/Analysis/ScopedNoAliasAA.h"

#include "boaaa/lv/CountPass.h"
#include "boaaa/lv/EvaluationPass.h"
#include "boaaa/lv/EvaluationPassDefinitions.h"
#include "boaaa/lv/AnalysisDiscrepancyChecker.h"

using namespace boaaa;

void registerPasses();

DLInterface40::DLInterface40()
{

}

DLInterface40::~DLInterface40()
{

}

void DLInterface40::onLoad()
{
	registerPasses();
	context.string_ref_vp = new StringRefVP40();
}

void DLInterface40::onUnload()
{
	delete context.string_ref_vp;
}

void DLInterface40::registerStringRefVPM(StringRefVPM* manager)
{
	context.string_ref_vp->registerVPM(manager);
}

void DLInterface40::setBasicOStream(std::ostream& ostream, bool del)
{
	if (context.del_strm_after_use)
		delete context.basic_ostream;
	context.del_strm_after_use = del;
	context.basic_ostream = &ostream;
}

boaaa::cl_aa_store DLInterface40::getAvailableAAs()
{
	return boaaa::getInitalizedAAs_40();
}


bool loadModuleHelper(llvm::StringRef ref, boaaa::version_context& context)
{
	context.context_to_module.reset(new LLVMLLVMContext());
	context.module_result.reset(new ModuleResult());
	llvm::SMDiagnostic Err;

	context.loaded_module = llvm::parseIRFile(ref, Err, *context.context_to_module);
	if (!context.loaded_module) {
		*(context.basic_ostream)  << "Error: while loading LLVMModule " << ref.str()
								  << " \nMSG  : " << Err.getMessage().str() << "\n";
		return false;
	}

	//evaluate countpass result
	llvm::legacy::PassManager pm;
	CountPass* cp = new CountPass();
	pm.add(cp);
	pm.run(*context.loaded_module);
	cp->printResult(*context.basic_ostream);
	cp->printToModuleRes(*context.module_result);
	//dont delete pass, get deleted in PM
	//delete cp;

	//scan pointers for evaluation
	context.relevant_pointers = std::map<uint64_t, std::unique_ptr<EvaluationContainer>>();
	EvaluationPassImpl::scanPointers(*context.loaded_module, context.relevant_pointers);

	return true;
}

bool DLInterface40::loadModule(uint64_t module_file_hash)
{
	_raw_type_inst(context.string_ref_vp)::store_t storeBC = context.string_ref_vp->generateStorage();
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(module_file_hash, storeBC);

	return loadModuleHelper(bc_ref, context);
}

bool DLInterface40::loadModule(uint64_t module_file_prefix, uint64_t module_file_hash)
{
	using store = typename _raw_type_inst(context.string_ref_vp)::store_t;
	store storePrefix = context.string_ref_vp->generateStorage();
	store storeBC = context.string_ref_vp->generateStorage();

	llvm::StringRef prefix = context.string_ref_vp->parseRegistered(module_file_prefix, storePrefix);
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(module_file_hash, storeBC);

	std::string filename = prefix.str() + "40" + bc_ref.str();

	return loadModuleHelper(filename, context);
}

void DLInterface40::unloadModule()
{
	boaaa::AnalysisDiscrepancyChecker adc;
	std::cout << "start discrapency check\n";
	adc.checkAnalysis(context.alias_sets, context.no_alias_sets);
	std::cout << "stop discrapency check\n";

	context.alias_sets.clear();
	context.no_alias_sets.clear();

	context.relevant_pointers.clear();

	context.loaded_module.reset(nullptr);
	context.context_to_module.reset(nullptr);
	context.module_result.reset(nullptr);
}

template<typename F>
bool runAnalysisHelp(F& run, boaaa::aa_id analysis)
{
	using LLV = boaaa::LLVM_40_AA;

	if ((analysis & version_mask) != LLVM_VERSIONS::LLVM_40) return false;
	switch (analysis)
	{
	case LLV::CFL_ANDERS:
		run(new llvm::AndersAAEvalWrapperPass(),
			new boaaa::TimePass<llvm::CFLAndersAAWrapperPass>());
		break;
	case LLV::BASIC:
		run(new llvm::BasicAAEvalWrapperPass(),
			new boaaa::TimePass<llvm::BasicAAWrapperPass>());
		break;
	case LLV::OBJ_CARC:
		run(new llvm::ObjCARCAAEvalWrapperPass(),
			new boaaa::TimePass<llvm::ObjCARCAAWrapperPass>());
		break;
	case LLV::SCEV:
		run(new llvm::SCEVAAEvalWrapperPass(),
			new boaaa::TimePass<llvm::SCEVAAWrapperPass>());
		break;
	case LLV::SCOPEDNA:
		run(new llvm::ScopedNoAliasEvalWrapperPass(),
			new boaaa::TimePass<llvm::ScopedNoAliasAAWrapperPass>());
		break;
	case LLV::CFL_STEENS:
		run(new llvm::SteensAAEvalWrapperPass(),
			new boaaa::TimePass<llvm::CFLSteensAAWrapperPass>());
		break;
	case LLV::TBAA:
		run(new llvm::TypeBasedAAEvalWrapperPass(),
			new boaaa::TimePass<llvm::TypeBasedAAWrapperPass>());
		break;
	}
	return true;
}

bool DLInterface40::runAnalysis(boaaa::aa_id analysis)
{
	auto run = [&](auto* pass, auto* timepass) -> void {
		llvm::legacy::PassManager pm;
		pass->setContext(&context);
		pm.add(timepass);
		pm.add(pass);
		pm.run(*context.loaded_module);
		if (*context.basic_ostream) {
			timepass->printResult(*context.basic_ostream);
			pass->printResult(*context.basic_ostream);
		}
	};

	return runAnalysisHelp(run, analysis);
}

bool DLInterface40::runAnalysis(boaaa::aa_id analysis, EvaluationResult& er)
{
	auto run = [&](auto* pass, auto* timepass) -> void {
		llvm::legacy::PassManager pm;
		pass->setContext(&context);
		pm.add(timepass);
		pm.add(pass);
		pm.run(*context.loaded_module);
		if (*context.basic_ostream) {
			timepass->printResult(*context.basic_ostream);
			pass->printResult(*context.basic_ostream);
		}
		timepass->printToEvalRes(er);
		pass->printToEvalRes(er);
	};

	return runAnalysisHelp(run, analysis);
}

void DLInterface40::test(uint64_t* hash, uint8_t num)
{
	_raw_type_inst(context.string_ref_vp)::store_t storeSR = context.string_ref_vp->generateStorage();
	llvm::StringRef ref = context.string_ref_vp->parseRegistered(hash[0], storeSR);
	*(context.basic_ostream) << LLVM_VERSION << " " << ref.str() << std::endl;

	/*
	std::unique_ptr<llvm::Module> module = llvm::parseIRFile(bc_ref, Err, Context);

	llvm::legacy::PassManager pm;

	llvm::TargetLibraryInfoWrapperPass* tli = new llvm::TargetLibraryInfoWrapperPass();
	llvm::SCEVAAWrapperPass* scev = new llvm::SCEVAAWrapperPass();
	llvm::BasicAAWrapperPass* basic_aa = new llvm::BasicAAWrapperPass();
	pm.add(tli);
	pm.add(basic_aa);
	//pm.add(basic_aa);

	pm.run(*module);

	llvm::AAResults result(tli->getTLI());
	result.addAAResult(basic_aa->getResult());

	boaaa::AAResultEvaluationPassImpl impl;
	int i = 0;
	for (LLVMFunction& F : *module)
	{
		i++;
		//i == 116/121 other error in find next
		if (i == 2 || i == 25 || i == 26 || i == 27 || i == 28 || i == 46 || i == 50 || i == 65 || i == 83 || i == 110 || i == 111 || i == 112 || i == 114 || i == 115 || i == 116 || i == 119 || i == 120 || i == 121 || i >= 124) continue;
		impl.evaluateAAResultOnFunction(result, F);
	}

	//impl.evaluateAAResult(result, *module);	
	impl.printResult(*(context.basic_ostream));
	
	*/
}

void registerPasses()
{
	llvm::PassRegistry& registry = *llvm::PassRegistry::getPassRegistry();
	
	//llvm specific
		llvm::initializeAnalysis(registry);
		llvm::initializeBasicAAWrapperPassPass(registry);
		llvm::initializeCFLAndersAAWrapperPassPass(registry);
		llvm::initializeCFLSteensAAWrapperPassPass(registry);
		llvm::initializeObjCARCAAWrapperPassPass(registry);
		llvm::initializeSCEVAAWrapperPassPass(registry);
		llvm::initializeScopedNoAliasAAWrapperPassPass(registry);
		llvm::initializeTypeBasedAAWrapperPassPass(registry);


	//extern


	//boaaa specific


}
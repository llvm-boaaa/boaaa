#include "boaaa/lv/50/Interface_50.h"

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
#include "llvm/IR/LegacyPassManagers.h"
#include "llvm/Passes/PassBuilder.h"

#include "boaaa/lv/CountPass.h"
#include "boaaa/lv/EvaluationPass.h"
#include "boaaa/lv/EvaluationPassDefinitions.h"

using namespace boaaa;
void registerPasses();
DLInterface50::DLInterface50()
{

}

DLInterface50::~DLInterface50()
{

}

void DLInterface50::onLoad()
{
	registerPasses();
	context.string_ref_vp = new StringRefVP50();
}

void DLInterface50::onUnload()
{
	delete context.string_ref_vp;
}

void DLInterface50::registerStringRefVPM(StringRefVPM* manager)
{
	context.string_ref_vp->registerVPM(manager);
}

void DLInterface50::setBasicOStream(std::ostream& ostream, bool del)
{
	if(context.del_strm_after_use)
		delete context.basic_ostream;
	context.del_strm_after_use = del;
	context.basic_ostream = &ostream;
}

boaaa::cl_aa_store DLInterface50::getAvailableAAs()
{
	return boaaa::getInitalizedAAs_50();
}

bool DLInterface50::loadModule(uint64_t module_file_hash)
{
	_raw_type_inst(context.string_ref_vp)::store_t storeBC = context.string_ref_vp->generateStorage();
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(module_file_hash, storeBC);

	context.context_to_module.reset(new LLVMLLVMContext());
	llvm::SMDiagnostic Err;

	context.loaded_module = llvm::parseIRFile(bc_ref, Err, *context.context_to_module);
	if (!context.loaded_module) {
		*(context.basic_ostream) << "Error: while loading LLVMModule " << bc_ref.str() 
			<< " \nMSG  : " << Err.getMessage().str() << "\n";
		return false;
	}

	//evaluate countpass result
	llvm::legacy::PassManager pm;
	CountPass* cp = new CountPass();
	pm.add(cp);
	pm.run(*context.loaded_module);
	cp->printResult(*context.basic_ostream);
	//dont delete pass, get deleted in PM
	//delete cp;

	//scan pointers for evaluation
	context.relevant_pointers = std::map<uint64_t, std::unique_ptr<EvaluationContainer>>();
	EvaluationPassImpl::scanPointers(*context.loaded_module, context.relevant_pointers);

	return true;
}

bool DLInterface50::loadModule(uint64_t module_file_prefix, uint64_t module_file_hash)
{
	using store = typename _raw_type_inst(context.string_ref_vp)::store_t;
	store storePrefix = context.string_ref_vp->generateStorage();
	store storeBC = context.string_ref_vp->generateStorage();

	llvm::StringRef prefix = context.string_ref_vp->parseRegistered(module_file_prefix, storePrefix);
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(module_file_hash, storeBC);

	std::string filename = prefix.str() + "50" + bc_ref.str();

	context.context_to_module.reset(new LLVMLLVMContext());
	llvm::SMDiagnostic Err;

	context.loaded_module = llvm::parseIRFile(filename, Err, *context.context_to_module);
	if (!context.loaded_module) {
		*(context.basic_ostream) << "Error: while loading LLVMModule " << filename
			<< " \nMSG  : " << Err.getMessage().str() << "\n";
		return false;
	}

	//evaluate countpass result
	llvm::legacy::PassManager pm;
	CountPass* cp = new CountPass();
	pm.add(cp);
	pm.run(*context.loaded_module);
	cp->printResult(*context.basic_ostream);
	//dont delete pass, get deleted in PM
	//delete cp;

	//scan pointers for evaluation
	context.relevant_pointers = std::map<uint64_t, std::unique_ptr<EvaluationContainer>>();
	EvaluationPassImpl::scanPointers(*context.loaded_module, context.relevant_pointers);

	return true;
}

bool DLInterface50::runAnalysis(boaaa::aa_id analysis)
{
	using LLV = boaaa::LLVM_50_AA;

	auto run = [=](auto* pass, auto* timepass) -> auto {
		llvm::legacy::PassManager pm;
		pass->setContext(&context);
		pm.add(timepass);
		pm.add(pass);
		pm.run(*context.loaded_module);
		timepass->printResult(*context.basic_ostream);
		pass->printResult(*context.basic_ostream);
		return pass;
	};

	if ((analysis & version_mask) != LLVM_VERSIONS::LLVM_50) return false;
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

void DLInterface50::test(uint64_t* hash, uint8_t num)
{
	_raw_type_inst(context.string_ref_vp)::store_t storeSR = context.string_ref_vp->generateStorage();
	llvm::StringRef ref = context.string_ref_vp->parseRegistered(hash[0], storeSR);
	*(context.basic_ostream) << LLVM_VERSION << " " << ref.str() << std::endl;

	_raw_type_inst(context.string_ref_vp)::store_t storeBC = context.string_ref_vp->generateStorage();
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(hash[1], storeBC);

	llvm::LLVMContext Context;
	llvm::SMDiagnostic Err;

	/*
	std::unique_ptr<llvm::Module> module = llvm::parseIRFile(bc_ref, Err, Context);

	llvm::legacy::PassManager pm;
	//basic_aa.add(llvm::createBasicAAWrapperPass());
	//basic_aa.add(llvm::createCFLAndersAAWrapperPass());
	//basic_aa.add(llvm::createCFLSteensAAWrapperPass());
	//basic_aa.add(llvm::createScopedNoAliasAAWrapperPass());
	llvm::TargetLibraryInfoWrapperPass* tli = new llvm::TargetLibraryInfoWrapperPass();
	llvm::SCEVAAWrapperPass* scev = new llvm::SCEVAAWrapperPass();
	llvm::BasicAAWrapperPass* basic_aa = new llvm::BasicAAWrapperPass();
	pm.add(tli);
	pm.add(basic_aa);
	//pm.add(basic_aa);

	//pm.add(new llvm::ScalarEvolutionWrapperPass());
	//pm.add(llvm::createSCEVAAWrapperPass());
	//pm.add(boaaa::createSVECAAEVALWrapperPass());
	//pm.add(llvm::createAAEvalPass());
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

		//seadsa - copyied from main
		llvm::initializeCallGraphWrapperPassPass(registry);
		// XXX: porting to 5.0
		//  llvm::initializeCallGraphPrinterPass(Registry);
		llvm::initializeCallGraphViewerPass(registry);
		// XXX: not sure if needed anymore
		llvm::initializeGlobalsAAWrapperPassPass(registry);

		//sfs


	//boaaa specific


}
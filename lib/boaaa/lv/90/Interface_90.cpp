#include "boaaa/lv/90/Interface_90.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/SourceMgr.h"

#include "boaaa/support/dump_ostream.h"

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AliasAnalysisEvaluator.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/CFLAndersAliasAnalysis.h"
#include "llvm/Analysis/CFLSteensAliasAnalysis.h"
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"
#include "llvm/Analysis/ScopedNoAliasAA.h"
#include "llvm/IR/LegacyPassManagers.h"

#include "boaaa/lv/CountPass.h"
#include "boaaa/lv/EvaluationPass.h"
#include "boaaa/lv/EvaluationPassDefinitions.h"
#include "boaaa/lv/TimePass.h"

using namespace boaaa;

void registerPasses();

DLInterface90::DLInterface90()
{

}

DLInterface90::~DLInterface90()
{
	
}

void DLInterface90::onLoad()
{
	registerPasses();
	context.string_ref_vp = new StringRefVP90();
}

void DLInterface90::onUnload()
{
	delete context.string_ref_vp;
}

void DLInterface90::registerStringRefVPM(StringRefVPM* manager)
{
	context.string_ref_vp->registerVPM(manager);
}

void DLInterface90::setBasicOStream(std::ostream& ostream, bool del)
{
	if (context.del_strm_after_use)
		delete context.basic_ostream;
	context.del_strm_after_use = del;
	context.basic_ostream = &ostream;
}

boaaa::cl_aa_store DLInterface90::getAvailableAAs()
{
	return boaaa::getInitalizedAAs_90();
}

bool DLInterface90::loadModule(uint64_t module_file_hash)
{
	_raw_type_inst(context.string_ref_vp)::store_t storeBC = context.string_ref_vp->generateStorage();
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(module_file_hash, storeBC);

	context.context_to_module.reset(new LLVMLLVMContext());
	llvm::SMDiagnostic Err;

	context.loaded_module = llvm::parseIRFile(bc_ref, Err, *context.context_to_module);
	if (!context.loaded_module) {
		*(context.basic_ostream) << "Error: while loading LLVMModule " << bc_ref.str() << " \nMSG  : " << Err.getMessage().str() << "\n";
		return false;
	}
	return true;
}

bool DLInterface90::runAnalysis(boaaa::aa_id analysis)
{
	if ((analysis & version_mask) != LLVM_VERSIONS::LLVM_90) return false;

	

	return true;
}

void DLInterface90::test(uint64_t* hash, uint8_t num)
{
	_raw_type_inst(context.string_ref_vp)::store_t storeSR = context.string_ref_vp->generateStorage();
	llvm::StringRef ref = context.string_ref_vp->parseRegistered(hash[0], storeSR);
	*(context.basic_ostream) << LLVM_VERSION << " " << ref.str() << std::endl;

	llvm::LLVMContext llvm_context;
	llvm::legacy::PassManager manager;		

	llvm::legacy::PassManager pm;
	//basic_aa.add(llvm::createBasicAAWrapperPass());
	//basic_aa.add(llvm::createCFLAndersAAWrapperPass());
	//basic_aa.add(llvm::createCFLSteensAAWrapperPass());
	//basic_aa.add(llvm::createScopedNoAliasAAWrapperPass());
	llvm::TargetLibraryInfoWrapperPass* tli = new llvm::TargetLibraryInfoWrapperPass();
	llvm::SCEVAAWrapperPass *scev = new llvm::SCEVAAWrapperPass();
	//llvm::BasicAAWrapperPass* basic_aa = new llvm::BasicAAWrapperPass();
	boaaa::TimePass<llvm::SCEVAAWrapperPass>* tmscev = new boaaa::TimePass<llvm::SCEVAAWrapperPass>();
	llvm::SCEVAAEvalWrapperPass* sceveval = new llvm::SCEVAAEvalWrapperPass();
	
	boaaa::CountPass *cp = new boaaa::CountPass(); 
	pm.add(tli);
	pm.add(cp);
	//pm.add(tmscev);
	pm.add(sceveval);
	//pm.add(tmscev);
	//pm.add(scev);
	//pm.add(basic_aa);

	//pm.add(new llvm::ScalarEvolutionWrapperPass());
	//pm.add(scev);
	//pm.add(llvm::createAAEvalPass());
	pm.run(*context.loaded_module);

	//tmscev->printResult(*context.basic_ostream);
	sceveval->printResult(*context.basic_ostream);
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
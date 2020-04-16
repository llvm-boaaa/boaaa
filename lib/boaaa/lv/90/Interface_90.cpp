#include "boaaa/lv/90/Interface_90.h"

#include "llvm/Support/CommandLine.h"
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


bool loadModuleHelper(llvm::StringRef ref, boaaa::version_context& context)
{
	context.context_to_module.reset(new LLVMLLVMContext());
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
	//dont delete pass, get deleted in PM
	//delete cp;

	//scan pointers for evaluation
	context.relevant_pointers = std::map<uint64_t, std::unique_ptr<EvaluationContainer>>();
	EvaluationPassImpl::scanPointers(*context.loaded_module, context.relevant_pointers);

	return true;
}

bool DLInterface90::loadModule(uint64_t module_file_hash)
{
	_raw_type_inst(context.string_ref_vp)::store_t storeBC = context.string_ref_vp->generateStorage();
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(module_file_hash, storeBC);

	return loadModuleHelper(bc_ref, context);
}

bool DLInterface90::loadModule(uint64_t module_file_prefix, uint64_t module_file_hash)
{
	using store = typename _raw_type_inst(context.string_ref_vp)::store_t;
	store storePrefix = context.string_ref_vp->generateStorage();
	store storeBC = context.string_ref_vp->generateStorage();

	llvm::StringRef prefix = context.string_ref_vp->parseRegistered(module_file_prefix, storePrefix);
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(module_file_hash, storeBC);

	std::string filename = prefix.str() + "80" + bc_ref.str();

	return loadModuleHelper(filename, context);
}

void DLInterface90::unloadModule()
{
	context.alias_sets.clear();
	context.no_alias_sets.clear();

	context.relevant_pointers.clear();

	context.loaded_module.reset(nullptr);
	context.context_to_module.reset(nullptr);
}

template<typename F>
bool runAnalysisHelp(F& run, boaaa::aa_id analysis)
{
	using LLV = boaaa::LLVM_90_AA;

	if ((analysis & version_mask) != LLVM_VERSIONS::LLVM_80) return false;
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

bool DLInterface90::runAnalysis(boaaa::aa_id analysis)
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

bool DLInterface90::runAnalysis(boaaa::aa_id analysis, EvaluationResult& er)
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
	llvm::initializeCore(registry);
	llvm::initializeCoroutines(registry);
	llvm::initializeScalarOpts(registry);
	llvm::initializeObjCARCOpts(registry);
	llvm::initializeVectorization(registry);
	llvm::initializeIPO(registry);
	llvm::initializeAnalysis(registry);
	llvm::initializeTransformUtils(registry);
	llvm::initializeInstCombine(registry);
	llvm::initializeAggressiveInstCombine(registry);
	llvm::initializeInstrumentation(registry);
	llvm::initializeTarget(registry);

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
//debug flags
#define DEBUG 1

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#endif

#ifdef DEBUG
#define DEBUG_COMMAND_LINE
#define DEBUG_DLL_TEST

#endif

#define LLVM_VERSION_90 90


#include "llvm/Analysis/CallGraph.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Analysis/LoopPass.h"
#include "llvm/Analysis/RegionPass.h"
#include "llvm/Analysis/TargetLibraryInfo.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/IR/DataLayout.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/LegacyPassNameParser.h"
#include "llvm/IR/RemarkStreamer.h"
#include "llvm/IR/Verifier.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/InitializePasses.h"
#include "llvm/LinkAllPasses.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/InitLLVM.h"
#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Target/TargetMachine.h"
//boaaa
#include "boaaa/stdafx.h"

#include "llvm/Support/CommandLine.h"

#define NO_EXPORT //defines __export as nothing, to fix import problem of LLVMVersionManager
#include "boaaa/dynamic_interface.h"
#undef NO_EXPORT
#include "boaaa/lvm/LLVMVersionManager.h"
#include "boaaa/support/LLVMErrorOr.h"
#include "ModuleReader.h"

#include <algorithm>
#include <memory>
#include <iostream>

//using namespace llvm;


//command line arg variables

static cl::OptionCategory BoaaaCat("BOAAA options:");


static cl::opt<std::string> 
InputFolder("f", cl::desc("Specify the input foldername of the *.bc files"), 
	cl::value_desc("folder"), cl::cat(BoaaaCat));

static cl::opt<std::string> 
OutputFilename("out", cl::desc("Specify output filename"), 
	cl::value_desc("filename"), cl::cat(BoaaaCat));


//copyied from opt

static cl::opt<std::string>
InputFilename(cl::Positional, cl::desc("<input bitcode file>"),
	cl::init("-"), cl::value_desc("filename"), cl::cat(BoaaaCat));

//static cl::list<const PassInfo*, bool, PassNameParser>
//PassList(cl::desc("Optimizations available:"), cl::cat(BoaaaCat));

void registerPasses(PassRegistry& Registry);

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/AliasAnalysisEvaluator.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/Analysis/CFLAndersAliasAnalysis.h"
#include "llvm/Analysis/CFLSteensAliasAnalysis.h"
#include "llvm/Analysis/ScalarEvolutionAliasAnalysis.h"
#include "llvm/Analysis/ScopedNoAliasAA.h"
#include "llvm/IR/LegacyPassManager.h"

#include "boaaa/lv/EvaluationPass.h"
#include "boaaa/lv/EvaluationPassDefinitions.h"

int main(int argc, char** argv) {
	//init llvm and register needed passes

#ifdef DEBUG_COMMAND_LINE
	const int _argc = 4;
	const char* _argv[] = { "boaaa", "../../../../bc_sources/libbmi160.a.bc", "--debug-pass=Structure", "--evaluate-aa-metadata" }; //"--print-all-alias-modref-info" };
	//const char* _argv[] = { "boaaa", "-help" };
	cl::ParseCommandLineOptions(_argc, _argv, "");	
#else
	//parse command line
	cl::ParseCommandLineOptions(argc, argv);
#endif
	// maybe use to reset commandline args 
	//cl::getRegisteredOptions().begin()->getValue()->reset();

	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmPrinters();
	InitializeAllAsmParsers();

	PassRegistry &Registry = *PassRegistry::getPassRegistry();
	registerPasses(Registry);

	LLVMContext context;	
	std::unique_ptr<Module> M = boaaa::parseIRFile(InputFilename, context);

	
	boaaa::LLVMVersionManager man = boaaa::LLVMVersionManager();
	boaaa::StringRefVPM* srvpm = new boaaa::StringRefVPM();
	man.registerStringRefVPM(srvpm);
	
	//std::shared_ptr<boaaa::DLInterface> llvm40 = man.loadDL("boaaa.lv_40");
	//std::shared_ptr<boaaa::DLInterface> llvm50 = man.loadDL("boaaa.lv_50");
	std::shared_ptr<boaaa::DLInterface> llvm90 = man.loadDL("boaaa.lv_90");
	
	//llvm40->setBasicOStream(std::cout);
	//llvm50->setBasicOStream(std::cout);
	llvm90->setBasicOStream(std::cout);

	//llvm40->registerStringRefVPM(srvpm);
	//llvm50->registerStringRefVPM(srvpm);
	llvm90->registerStringRefVPM(srvpm);

	StringRef ref = "this is a test string";

	uint64_t str_test_hash = man.registerData(ref);
	uint64_t str_test_bc = man.registerData(InputFilename);

	uint64_t* test_args = new uint64_t[2]{ str_test_hash, str_test_bc };

	//FreeConsole();

	//llvm40->test(test_args, 2);
	//llvm50->test(test_args, 2);
	llvm90->test(test_args, 2);

	//llvm::AAManager aaman;
	//llvm::legacy::PassManager basic_aa;
	//basic_aa.add(new llvm::BasicAAWrapperPass());
	//basic_aa.add(new boaaa::CountPass());
	//basic_aa.run(*M.get());

	return 0;
}

//tea dsa branch of sea dsa
//svf

void registerPasses(PassRegistry& Registry) 
{
	
	initializeCore(Registry);
	initializeCoroutines(Registry);
	initializeScalarOpts(Registry);
	initializeObjCARCOpts(Registry);
	initializeVectorization(Registry);
	initializeIPO(Registry);
	initializeAnalysis(Registry);
	initializeTransformUtils(Registry);
	initializeInstCombine(Registry);
	initializeAggressiveInstCombine(Registry);
	initializeInstrumentation(Registry);
	initializeTarget(Registry);
	// For codegen passes, only passes that do IR to IR transformation are
	// supported.
	initializeExpandMemCmpPassPass(Registry);
	initializeScalarizeMaskedMemIntrinPass(Registry);
	initializeCodeGenPreparePass(Registry);
	initializeAtomicExpandPass(Registry);
	initializeRewriteSymbolsLegacyPassPass(Registry);
	initializeWinEHPreparePass(Registry);
	initializeDwarfEHPreparePass(Registry);
	initializeSafeStackLegacyPassPass(Registry);
	initializeSjLjEHPreparePass(Registry);
	initializeStackProtectorPass(Registry);
	initializePreISelIntrinsicLoweringLegacyPassPass(Registry);
	initializeGlobalMergePass(Registry);
	initializeIndirectBrExpandPassPass(Registry);
	initializeInterleavedLoadCombinePass(Registry);
	initializeInterleavedAccessPass(Registry);
	initializeEntryExitInstrumenterPass(Registry);
	initializePostInlineEntryExitInstrumenterPass(Registry);
	initializeUnreachableBlockElimLegacyPassPass(Registry);
	initializeExpandReductionsPass(Registry);
	initializeWasmEHPreparePass(Registry);
	initializeWriteBitcodePassPass(Registry);
	initializeHardwareLoopsPass(Registry);
}
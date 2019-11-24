#include "boaaa/stdafx.h"
#include "ModuleReader.h"

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
//#include "llvm/Support/PluginLoader.h"
#include "llvm/Support/SystemUtils.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/YAMLTraits.h"
#include "llvm/Target/TargetMachine.h"
//boaaa
#include "boaaa/lvm/LLVMVersionManager.h"
#include <algorithm>
#include <memory>
#include <iostream>

using namespace llvm;
#define DEBUG_COMMAND_LINE

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

static cl::list<const PassInfo*, bool, PassNameParser>
PassList(cl::desc("Optimizations available:"), cl::cat(BoaaaCat));

void registerPasses(PassRegistry& Registry);

int main(int argc, char** argv) {
	
	//init llvm and register needed passes
	InitLLVM X(argc, argv);

	LLVMContext Context;

	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmPrinters();
	InitializeAllAsmParsers();

	// Initialize passes
	PassRegistry& Registry = *PassRegistry::getPassRegistry();
	registerPasses(Registry);

#ifdef DEBUG_COMMAND_LINE
	int _argc = 2;
	const char* _argv[] = { "boaaa", "../../../../bc_sources/libbmi160.a.bc" };
	//const char* _argv[] = { "boaaa", "-help" };
	cl::ParseCommandLineOptions(_argc, _argv, "");	
#else
	//parse command line
	cl::ParseCommandLineOptions(argc, argv);
#endif
	// maybe use to reset commandline args 
	//cl::getRegisteredOptions().begin()->getValue()->reset();

	
	std::unique_ptr<Module> M = boaaa::parseIRFile(InputFilename, Context);

	boaaa::LLVMVersionManager man = boaaa::LLVMVersionManager();
	std::unique_ptr<boaaa::DLInterface> inst = man.loadDL("boaaa.lv_90");

	if (!inst) {
		return 1;
	}

	inst->onLoad();

	int i;
	std::cin >> i;

	return 0;
}

void registerPasses(PassRegistry& Registry) 
{
	initializeCore(Registry);
	initializeCoroutines(Registry);
	//initializeScalarOpts(Registry);
	//initializeObjCARCOpts(Registry);
	//initializeVectorization(Registry);
	//initializeIPO(Registry);
	initializeAnalysis(Registry);
	//initializeTransformUtils(Registry);
	//initializeInstCombine(Registry);
	//initializeAggressiveInstCombine(Registry);
	//initializeInstrumentation(Registry);
	initializeTarget(Registry);
	// For codegen passes, only passes that do IR to IR transformation are
	// supported.
	/*
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
	*/
}
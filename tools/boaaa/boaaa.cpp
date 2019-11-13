#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/"

void main() {
	
	LLVMContext Context;

	InitializeAllTargets();
	InitializeAllTargetMCs();
	InitializeAllAsmPrinters();
	InitializeAllAsmParsers();

	// Initialize passes
	PassRegistry& Registry = *PassRegistry::getPassRegistry();
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

	SMDiagnostic Err;
}
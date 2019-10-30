#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"

void main() {
	
	llvm::SMDiagnostic smd;
	llvm::LLVMContext& context = new llvm::LLVMContext();
	std::unique_ptr<llvm::Module> module = llvm::getLazyIRFileModule(llvm::StringRef::StringRef("test.bc"), smd, context, false);
	llvm::AAManager manager;
	manager.registerFunctionAnalysis<llvm::BasicAA>();
	llvm::FunctionAnalysisManager fam = new llvm::FunctionAnalysisManager(true);
	llvm::AAResults res = manager.run(module->getFunctionList().front(), fam);
}
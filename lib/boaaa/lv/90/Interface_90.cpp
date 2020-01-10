#include "Interface_90.h"

#include "llvm/Support/CommandLine.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/SourceMgr.h"

#include "boaaa/support/dump_ostream.h"

using namespace boaaa;

DLInterface90::DLInterface90()
{

}

DLInterface90::~DLInterface90()
{
	
}

void DLInterface90::onLoad()
{
	context.string_ref_vp = new StringRefVP90();
	//cl passing no working or not printing
	//const int _argc = 1;
	//const char* _argv[] = { "--debug-pass=Structure" };
	//llvm::cl::ParseCommandLineOptions(_argc, _argv);
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

#include "llvm/Analysis/AliasAnalysis.h"
#include "llvm/Analysis/BasicAliasAnalysis.h"
#include "llvm/IR/LegacyPassManager.h"


void DLInterface90::test(uint64_t* hash, uint8_t num)
{
	_raw_type_inst(context.string_ref_vp)::store_t storeSR = context.string_ref_vp->generateStorage();
	llvm::StringRef ref = context.string_ref_vp->parseRegistered(hash[0], storeSR);
	*(context.basic_ostream) << LLVM_VERSION << " " << ref.str() << std::endl;

	llvm::LLVMContext llvm_context;
	llvm::legacy::PassManager manager;
	llvm::SMDiagnostic Err;

	_raw_type_inst(context.string_ref_vp)::store_t storeBC = context.string_ref_vp->generateStorage();
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(hash[1], storeBC);


	std::unique_ptr<llvm::Module> module = llvm::parseIRFile(bc_ref, Err, llvm_context);

	llvm::AAManager aaman;
	llvm::legacy::PassManager basic_aa;
	basic_aa.add(new llvm::BasicAAWrapperPass());
	basic_aa.run(*module.get());
}
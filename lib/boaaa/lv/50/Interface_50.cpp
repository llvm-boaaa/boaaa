#include "boaaa/lv/50/Interface_50.h"

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/SourceMgr.h"

#include "boaaa/support/dump_ostream.h"

using namespace boaaa;

DLInterface50::DLInterface50()
{

}

DLInterface50::~DLInterface50()
{

}

void DLInterface50::onLoad()
{
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

/*
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IRReader/IRReader.h"
#include "llvm/Support/ErrorOr.h"
#include "llvm/Support/SourceMgr.h"
*/


void DLInterface50::test(uint64_t* hash, uint8_t num)
{
	_raw_type_inst(context.string_ref_vp)::store_t storeSR = context.string_ref_vp->generateStorage();
	llvm::StringRef ref = context.string_ref_vp->parseRegistered(hash[0], storeSR);
	*(context.basic_ostream) << LLVM_VERSION << " " << ref.str() << std::endl;

	_raw_type_inst(context.string_ref_vp)::store_t storeBC = context.string_ref_vp->generateStorage();
	llvm::StringRef bc_ref = context.string_ref_vp->parseRegistered(hash[1], storeBC);

	//llvm::LLVMContext context;
	//llvm::legacy::PassManager manager;
	//llvm::SMDiagnostic Err;

	//std::unique_ptr<llvm::Module> module = llvm::parseIRFile(bc_ref, Err, context);
}
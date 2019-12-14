#include "Interface_50.h"

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

void DLInterface50::registerStringRefVPM(std::shared_ptr<StringRefVPM> manager)
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


void DLInterface50::test(uint64_t* hash, uint8_t num)
{
	llvm::StringRef ref = context.string_ref_vp->parseRegistered(*hash);
	*(context.basic_ostream) << LLVM_VERSION << " " << ref.str() << std::endl;
}
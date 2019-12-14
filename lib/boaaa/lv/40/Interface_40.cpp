#include "Interface_40.h"

#include "boaaa/support/dump_ostream.h"

using namespace boaaa;

DLInterface40::DLInterface40()
{

}

DLInterface40::~DLInterface40()
{

}

void DLInterface40::onLoad()
{
	context.string_ref_vp = new StringRefVP40();
}

void DLInterface40::onUnload()
{
	delete context.string_ref_vp;
}

void DLInterface40::registerStringRefVPM(StringRefVPM* manager)
{
	context.string_ref_vp->registerVPM(manager);
}

void DLInterface40::setBasicOStream(std::ostream& ostream, bool del)
{
	if (context.del_strm_after_use)
		delete context.basic_ostream;
	context.del_strm_after_use = del;
	context.basic_ostream = &ostream;
}


void DLInterface40::test(uint64_t* hash, uint8_t num)
{
	llvm::StringRef ref = context.string_ref_vp->parseRegistered(*hash);
	*(context.basic_ostream) << LLVM_VERSION << " " << ref.str() << std::endl;
}

#include "Interface_90.h"

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


void DLInterface90::test(uint64_t* hash, uint8_t num)
{
	llvm::StringRef ref = context.string_ref_vp->parseRegistered(*hash);
	*(context.basic_ostream) << LLVM_VERSION << " " << ref.str() << std::endl;
}
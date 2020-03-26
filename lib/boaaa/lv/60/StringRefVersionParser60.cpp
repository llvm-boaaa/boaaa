#include "boaaa/lv/60/StringRefVersionParser60.h"

using namespace boaaa;

StringRefVPM::container StringRefVP60::parse(llvm::StringRef& data)
{
	return container(data.str());
}

llvm::StringRef StringRefVP60::generate(StringRefVPM::container&& data, store_t& store)
{
	ErrorOr<std::string> StringOrErr = data.get<0>();
	if (!StringOrErr)
		return ""; //error
	store.reset();
	store = std::make_unique<std::string>(StringOrErr.get());
	return llvm::StringRef(store->c_str(), store->size());
}
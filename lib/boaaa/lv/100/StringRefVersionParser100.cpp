#include "boaaa/lv/100/StringRefVersionParser100.h"

using namespace boaaa;

StringRefVPM::container StringRefVP100::parse(llvm::StringRef& data)
{
	return container(data.str());
}

llvm::StringRef StringRefVP100::generate(StringRefVPM::container&& data, store_t& store)
{
	ErrorOr<std::string> StringOrErr = data.get<0>();
	if (!StringOrErr)
		return ""; //error
	store.reset();
	store = std::make_unique<std::string>(StringOrErr.get());
	return llvm::StringRef(store->c_str(), store->size());
}
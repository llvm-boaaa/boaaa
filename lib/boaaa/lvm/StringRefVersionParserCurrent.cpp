#include "boaaa/lvm/StringRefVersionParserCurrent.h"

using namespace boaaa;

StringRefVPM::container StringRefVPCur::parse(llvm::StringRef& data)
{
	return container(data.str());
}

llvm::StringRef StringRefVPCur::generate(StringRefVPM::container& data)
{
	ErrorOr<std::string> StringOrError = data.get<0>();
	if (!StringOrError)
		return ""; //error;
	return llvm::StringRef(StringOrError.get());
}
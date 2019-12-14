#include "StringRefVersionParser90.h"

using namespace boaaa;

StringRefVPM::container StringRefVP90::parse(llvm::StringRef& data)
{
	return container(data.str());
}

llvm::StringRef StringRefVP90::generate(StringRefVPM::container& data)
{
	ErrorOr<std::string> StringOrError = data.get<0>();
	if (!StringOrError)
		return ""; //error
	return llvm::StringRef(StringOrError.get());
}
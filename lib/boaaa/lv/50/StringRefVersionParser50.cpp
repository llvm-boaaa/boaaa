#include "StringRefVersionParser50.h"

using namespace boaaa;

StringRefVPM::container StringRefVP50::parse(llvm::StringRef& data)
{
	return container(data.str());
}

llvm::StringRef StringRefVP50::generate(StringRefVPM::container& data)
{
	ErrorOr<std::string> StringOrError = data.get<0>();
	if (!StringOrError)
		return ""; //error
	return llvm::StringRef(StringOrError.get());
}
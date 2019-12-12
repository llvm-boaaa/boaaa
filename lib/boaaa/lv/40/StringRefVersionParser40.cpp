#include "StringRefVersionParser40.h"

using namespace boaaa;

StringRefVPM::container StringRefVP40::parse(llvm::StringRef& data)
{
	return container(data.str());
}

llvm::StringRef StringRefVP40::generate(StringRefVPM::container& data)
{
	ErrorOr<std::string> StringOrError = data.get<0, std::string>();
	if (!StringOrError)
		return ""; //error;
	return llvm::StringRef(StringOrError.get());
}

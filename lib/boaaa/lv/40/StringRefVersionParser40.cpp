#include "StringRefVersionParser40.h"

#include "boaaa/lv/StringRefHold.h"

using namespace boaaa;

StringRefVPM::container StringRefVP40::parse(llvm::StringRef& data)
{
	return container(data.str());
}

llvm::StringRef StringRefVP40::generate(StringRefVPM::container& data)
{
	ErrorOr<std::string> StringOrErr = data.get<0>();
	if (!StringOrErr)
		return ""; //error;
	return StringRefHold(StringOrErr.get());
}

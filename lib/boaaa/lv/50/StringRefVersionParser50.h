#ifndef STRING_REF_VERSION_PARSER_50_H
#define STRING_REF_VERSION_PARSER_50_H

#include "llvm/ADT/StringRef.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa
{
	class StringRefVP50 : public StringRefVP<llvm::StringRef>
	{
		StringRefVP50() = default;

		virtual container parse(llvm::StringRef& data) override;
		virtual llvm::StringRef generate(container& data) override;
	};

}

#endif //!STRING_REF_VERSION_PARSER_50_H
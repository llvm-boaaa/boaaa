#ifndef BOAAA_STRING_REF_VERSION_PARSER_CURRENT_H
#define BOAAA_STRING_REF_VERSION_PARSER_CURRENT_H

#include "llvm/ADT/StringRef.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa
{
	class StringRefVPCur : public StringRefVP<llvm::StringRef>
	{
	public:
		StringRefVPCur() = default;

		virtual container parse(llvm::StringRef& data) override;
		virtual llvm::StringRef generate(container& data) override;
	};

}

#endif // !BOAAA_STRING_REF_VERSION_PARSER_CURRENT_H

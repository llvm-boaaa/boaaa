#ifndef STRING_REF_VERSION_PARSER_50_H
#define STRING_REF_VERSION_PARSER_50_H

#include "llvm/ADT/StringRef.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa
{
	class StringRefVP50 : public StringRefVP<llvm::StringRef>
	{
	public:
		StringRefVP50() = default;

		virtual container parse(llvm::StringRef& data);
		virtual llvm::StringRef generate(container& data, store_t& store);
	};

}

#endif //!STRING_REF_VERSION_PARSER_50_H
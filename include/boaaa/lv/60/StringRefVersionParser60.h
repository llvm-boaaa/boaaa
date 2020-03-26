#ifndef STRING_REF_VERSION_PARSER_60_H
#define STRING_REF_VERSION_PARSER_60_H

#include "llvm/ADT/StringRef.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa
{
	class StringRefVP60 : public StringRefVP<llvm::StringRef>
	{
	public:
		StringRefVP60() { };
		virtual ~StringRefVP60() { };

		virtual container parse(llvm::StringRef& data);
		virtual llvm::StringRef generate(container&& data, store_t& store);
	};

}

#endif //!STRING_REF_VERSION_PARSER_60_H
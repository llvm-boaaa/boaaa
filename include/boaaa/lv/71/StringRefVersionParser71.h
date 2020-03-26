#ifndef STRING_REF_VERSION_PARSER_71_H
#define STRING_REF_VERSION_PARSER_71_H

#include "llvm/ADT/StringRef.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa
{
	class StringRefVP71 : public StringRefVP<llvm::StringRef>
	{
	public:
		StringRefVP71() { };
		virtual ~StringRefVP71() { };

		virtual container parse(llvm::StringRef& data);
		virtual llvm::StringRef generate(container&& data, store_t& store);
	};

}

#endif //!STRING_REF_VERSION_PARSER_60_H
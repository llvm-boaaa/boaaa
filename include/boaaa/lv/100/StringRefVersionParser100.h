#ifndef STRING_REF_VERSION_PARSER_100_H
#define STRING_REF_VERSION_PARSER_100_H

#include "llvm/ADT/StringRef.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa
{
	class StringRefVP100 : public StringRefVP<llvm::StringRef>
	{
	public:
		StringRefVP100() { };
		virtual ~StringRefVP100() { };

		container parse(llvm::StringRef& data);
		llvm::StringRef generate(container&& data, store_t& store);
	};

}

#endif //!STRING_REF_VERSION_PARSER_100_H
#ifndef STRING_REF_VERSION_PARSER_90_H
#define STRING_REF_VERSION_PARSER_90_H

#include "llvm/ADT/StringRef.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa
{
	class StringRefVP90 : public StringRefVP<llvm::StringRef>
	{
	public:
		StringRefVP90() = default;
		~StringRefVP90() = default;

		container parse(llvm::StringRef& data);
		llvm::StringRef generate(container&& data, store_t& store);
	};

}

#endif //!STRING_REF_VERSION_PARSER_90_H
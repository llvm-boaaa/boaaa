#ifndef STRING_REF_VERSION_PARSER_40_H
#define STRING_REF_VERSION_PARSER_40_H

#include "llvm/ADT/StringRef.h"
#include "boaaa/vp/StringRefVersionParser.h"

namespace boaaa
{	
	class StringRefVP40 : public StringRefVP<llvm::StringRef>
	{
	public:
		StringRefVP40() = default;

		virtual container parse(llvm::StringRef& data);
		virtual llvm::StringRef generate(container& data, store_t& store);
	};

}



#endif //!STRING_REF_VERSION_PARSER_40_H
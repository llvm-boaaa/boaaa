#ifndef BOAAA_VP_STRING_REF_VERSION_PARSER_H
#define BOAAA_VP_STRING_REF_VERSION_PARSER_H

#include "VersionParser.h"

namespace boaaa
{
	enum StringRefParameterIndex
	{
		STRING_PARAM = 0
	};

	typedef VersionParseManager<std::string> StringRefVPM;
	template<typename type>
	using StringRefVP = StoreVersionParser<type, std::string, std::string>;
}

#endif //!BOAAA_VP_STRING_REF_VERSION_PARSER_H
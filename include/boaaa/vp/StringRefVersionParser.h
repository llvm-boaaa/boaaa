#ifndef BOAAA_VP_STRING_REF_VERSION_PARSER_H
#define BOAAA_VP_STRING_REF_VERSION_PARSER_H

#include "VersionParser.h"

namespace boaaa
{
	enum StringRefParameterIndex
	{
		STRING_PARAM = 0
	};

	typedef VersionParseManager<1, std::string> StringRefVPM;
	template<typename type>
	using StringRefVP = VersionParser<type, 1, std::string>;
}

#endif //!BOAAA_VP_STRING_REF_VERSION_PARSER_H
#ifndef STRING_REF_VERSION_PARSER_H
#define STRING_REF_VERSION_PARSER_H

#include "VersionParser.h"

namespace boaaa
{
	enum StringRefParameterIndex
	{
		STRING_PARAM = 0
	};

	typedef VersionParseManager<1, std::string> StringRefVPM;
}

#endif //!STRING_REF_VERSION_PARSER_H
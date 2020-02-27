#ifndef BOAAA_COMMAND_LINE_SEPERATOR_H
#define BOAAA_COMMAND_LINE_SEPERATOR_H

#include "llvm/ADT/StringRef.h"

#include <string>

namespace boaaa {

	char** parse(char* in, int* argc);
	char** parse(char* programm_name, char* in, int* argc);

}

#endif //!BOAAA_COMMAND_LINE_SEPERATOR_H
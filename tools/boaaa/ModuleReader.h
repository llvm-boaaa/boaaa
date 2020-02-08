#ifndef MODULEREADER_H
#define MODULEREADER_H

#include "boaaa/stdafx.h"
//#include <filesystem>
#include <experimental/filesystem>

using namespace llvm;

namespace boaaa {

	std::unique_ptr<Module> parseIRFile(StringRef filename, LLVMContext &context);
	std::unique_ptr<std::vector<StringRef>> getFilesOfFolder(StringRef fouldername);
	
	std::experimental::filesystem::path resolveAbsoluteOrRelativePath(StringRef filename);
}

#endif
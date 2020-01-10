#include "ModuleReader.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace boaaa {

	std::unique_ptr<Module> parseIRFile(StringRef filename, LLVMContext& context)
	{
		SMDiagnostic Err;
		std::unique_ptr<Module> M = llvm::parseIRFile(filename, Err, context);
		if (!M) {
			Err.print("boaaa", errs());
		}
		return M;
	}

	std::unique_ptr<std::vector<StringRef>> getFilesOfFolder(StringRef fouldername)
	{
		fs::path foulder = resolveAbsoluteOrRelativePath(fouldername);
		if (!fs::is_directory(foulder))
			; //add error handling

		std::unique_ptr<std::vector<StringRef>> files(new std::vector<StringRef>());
		for (auto& f : fs::directory_iterator(foulder))
		{
			if (f.path().has_extension())
				if (f.path().extension() == "bc")
					files->push_back(f.path().string());
		}
		return files;
	}

	fs::path resolveAbsoluteOrRelativePath(StringRef path)
	{
		fs::path p = path.str();
		if (p.is_relative())
			p = fs::current_path() / p;		
		return p;
	}


}
#ifndef BOAAA_UNIX_DL_HANDLER_H
#define BOAAA_UNIX_DL_HANDLER_H

#include "DLHandle.h"
#include "llvm/ADT/StringRef.h"

namespace boaaa {

	class UNIXDLHandle : public DLHandle 
	{
	private:
		bool loaded;
		bool loadable;
		llvm::StringRef filename;
		llvm::StringRef foldername;
		DLError err;

		void* inst;

		void load();
	public:
		UNIXDLHandle()
			: filename(""), foldername(""), loaded(false),
			loadable(false), err(DLError("")) { };
		UNIXDLHandle(llvm::StringRef filename, llvm::StringRef foldername);
		~UNIXDLHandle();

		DLError getERR() override;
		void* getFunction(llvm::StringRef funcname) override;
		llvm::StringRef getFilename() override { return filename; };
		llvm::StringRef getFoldername() override { return foldername; };
		void unload() override;
	};
}

#endif // !BOAAA_UNIX_DL_HANDLER_H
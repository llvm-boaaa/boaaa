#ifndef BOAAA_WINDOWS_DL_HANDLE_H
#define BOAAA_WINDOWS_DL_HANDLE_H

#include "boaaa/lvm/DLHandle.h"
//include <windows.h> dont include again for compilespeed
#include "boaaa/dynamic_interface.h"
#include "llvm/ADT/StringRef.h"

namespace boaaa {

	class WindowsDLHandle : public DLHandle 
	{
	private:
		bool loaded;
		bool loadable;
		llvm::StringRef filename;
		llvm::StringRef foldername;
		DLError err;

		HINSTANCE inst;
		
		void load();
	public:
		WindowsDLHandle() 
			: filename(""), foldername(""), loaded(false), 
			loadable(false), err(DLError("")), inst(nullptr) { };
		WindowsDLHandle(llvm::StringRef filename, llvm::StringRef foldername);
		~WindowsDLHandle();

		DLError getERR() override ;
		void* getFunction(llvm::StringRef funcname) override ;
		llvm::StringRef getFilename() override { return filename; };
		llvm::StringRef getFoldername() override { return foldername; };
		void unload() override;
	};
}

#endif // !BOAAA_WINDOWS_DL_HANDLE_H

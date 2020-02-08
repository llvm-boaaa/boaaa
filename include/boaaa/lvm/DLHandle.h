#ifndef BOAAA_LVM_DL_HANDLE_H
#define BOAAA_LVM_DL_HANDLE_H

#include "llvm/ADT/StringRef.h"

namespace boaaa {

	class DLError;

	class DLHandle 
	{
	public:	

		virtual ~DLHandle() = default;	
		virtual DLError getERR() = 0;
		virtual void* getFunction(llvm::StringRef funcname) = 0;
		virtual llvm::StringRef getFilename() = 0;
		virtual llvm::StringRef getFoldername() = 0;
		virtual void unload() = 0;


	};

	class DLError 
	{
	private:
		llvm::StringRef error_string;
		bool error_flag;
	public:
		DLError(llvm::StringRef error) {
			error_string = error;
			error_flag = !error.str().empty();
		}

		void setError(llvm::StringRef error) {
			if (error.empty())
				return;
			error_string = error;
			error_flag = true;
		}

		llvm::StringRef readError() {
			error_flag = false;
			auto tmp = error_string;
			error_string = "";
			return tmp;
		}
	};
}

#endif // !BOAAA_LVM_DL_HANDLE_H
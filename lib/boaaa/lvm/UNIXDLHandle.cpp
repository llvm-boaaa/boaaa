#include "UNIXDLHandle.h"
#include <dlfcn.h>

void* dlopen(const char* filename, int flag);
char* dlerror();
void* dlsym(void* handle, const char* symbol);
int dlclose(void* handle);
extern int RTLD_LAZY;

using namespace boaaa;

UNIXDLHandle::UNIXDLHandle(llvm::StringRef filename, llvm::StringRef foldername) 
	: filename(filename), foldername(foldername), err(DLError(""))
{
	//check file
	loaded = false;
	loadable = true;
}

UNIXDLHandle::~UNIXDLHandle()
{
	if (loaded)
	{
		unload();
	}
}

DLError UNIXDLHandle::getERR()
{
	return err;
}

void* UNIXDLHandle::getFunction(llvm::StringRef funcname)
{
	if (!loaded) {
		load();
		if (!loaded) {
			return nullptr;
		}
	}
	void* func = dlsym(inst, funcname.str.c_str());
	err.setError(err.readError().str() + "\n" + dlerror());
	return func;
}

void UNIXDLHandle::unload()
{
	dlclose(inst);
	err.setError(dlerror());
}

void UNIXDLHandle::load()
{
	if (loaded)
		return;
	std::string file = foldername.str() + "/" + filename.str() + ".so";
	inst = dlopen(file.c_str(), RTLD_LAZY);
	err.setError(dlerror());
}
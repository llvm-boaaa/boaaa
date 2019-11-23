#include "boaaa/lvm/WindowsDLHandle.h"
#include <Psapi.h>

using namespace boaaa;

std::wstring s2ws(const std::string& s);

WindowsDLHandle::WindowsDLHandle(llvm::StringRef filename, llvm::StringRef foldername)
	: filename(filename), foldername(foldername), err(DLError(""))
{
	//TODO: check file
	loaded = false;
	loadable = true;
	inst = nullptr;
}

WindowsDLHandle::~WindowsDLHandle()
{
	if (loaded) {
		unload();
	}
}

DLError WindowsDLHandle::getERR()
{
	return err;
}

void* WindowsDLHandle::getFunction(llvm::StringRef funcname)
{
	if (!loaded) {
		load();
		if (!loaded) {
			return nullptr;
		}
	}
	return GetProcAddress(inst, funcname.str().c_str());
}

void WindowsDLHandle::load()
{
	std::string file = foldername.str() + "/" + filename.str() + ".dll";
	inst = LoadLibrary(s2ws(file).c_str());
	if (!inst) {
		err.setError("DLL in file: " + file + ", can't be loaded!");
		loadable = false;
		return;
	}
	loaded = true;
}

void WindowsDLHandle::unload()
{
	if (!loaded)
		return;
	if (!FreeLibrary(inst)) {
		std::string file = foldername.str() + "/" + filename.str() + ".dll";
		err.setError("DLL in file:" + file + " cant be unloaded");
	}
}

//copied from : https://stackoverflow.com/questions/27220/how-to-convert-stdstring-to-lpcwstr-in-c-unicode
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}
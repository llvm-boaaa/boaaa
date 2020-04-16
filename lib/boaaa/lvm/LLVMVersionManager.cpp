#include "boaaa/lvm/LLVMVersionManager.h"

#include "boaaa/support/UNDEF__STRINGIFY.h"
#ifdef _WIN32
#include "WindowsDLHandle.h"
using _DLHandle = boaaa::WindowsDLHandle;
#else
#include "UNIXDLHandle.h"
using _DLHandle = boaaa::UNIXDLHandle;
#endif

#include "boaaa/support/__STRINGIFY.h"

using namespace boaaa;

/*
auto LLVMVersionManager::getInstance() -> LLVMVersionManager& {
	static auto instance = std::unique_ptr<LLVMVersionManager>{ nullptr };
	if (!instance)
		instance.reset(new LLVMVersionManager);
	return *instance.get();
}
*/

std::shared_ptr<DLInterface>
LLVMVersionManager::loadDL(StringRef filename, StringRef folder) {
	LLVM_Info* info;
	if (m_dl_map->count(filename)) { //count 0 or 1 so first item of it from find is the only object
		info = m_dl_map->find(filename)->getValue();
		if (!info->handle->getFoldername().equals(folder)) {
			//ERROR
			return std::unique_ptr<DLInterface>(nullptr);
		}
	} else {
		std::unique_ptr<_DLHandle> handle = std::unique_ptr<_DLHandle>(new _DLHandle(filename, folder));
		
		StringRef funcname = __STRINGIFY(GETDL_INFO);

		GETDL_INFOFUNC getInfo = (GETDL_INFOFUNC)handle->getFunction(funcname);
		if (!getInfo) {
			//ERROR
			return std::unique_ptr<DLInterface>(nullptr);
		}
		DL_Info _info = getInfo();
		info = new LLVM_Info(filename, _info.gen(), std::move(handle), _info);

		info->inst->onLoad();
		m_dl_map->insert(std::pair<std::string, LLVM_Info*>(filename.str(), info));
	}
	info->ref_counter++;
	return info->inst;
}

void LLVMVersionManager::mayUnload(std::shared_ptr<DLInterface> inst, StringRef filename) {
	if (!inst) return;
	LLVM_Info* info;
	if (!m_dl_map->count(filename)) { //count 0 or 1 so first item of it from find is the only object
		//ERROR
		return;
	}
	info = m_dl_map->find(filename)->getValue();
	if (!info) {
		//ERROR
		return;
	}
	info->info.del(std::move(inst));
	info->ref_counter--;
	if (info->ref_counter <= 0) {
		info->inst->onUnload();
		info->handle->unload();
	}
}

void LLVMVersionManager::onLoad() { }

void LLVMVersionManager::onUnload() { }

void LLVMVersionManager::registerStringRefVPM(StringRefVPM* manager)
{
	registerVPM(manager);
}

void LLVMVersionManager::setBasicOStream(std::ostream& ostream, bool del)
{
	if (context.del_strm_after_use)
		delete context.basic_ostream;
	context.del_strm_after_use = del;
	context.basic_ostream = &ostream;
}

boaaa::cl_aa_store LLVMVersionManager::getAvailableAAs()
{
	return boaaa::cl_aa_store();
}

void LLVMVersionManager::test(uint64_t* hash, uint8_t num)
{
	//never called
}

bool LLVMVersionManager::loadModule(uint64_t module_file_hash) { return false; };

bool LLVMVersionManager::loadModule(uint64_t module_file_prefix, uint64_t module_file_hash) { return false; };

void LLVMVersionManager::unloadModule() { }

boaaa::cl_aa_store getAvailableAAs() 
{
	cl_aa_store store;
	return store;
}





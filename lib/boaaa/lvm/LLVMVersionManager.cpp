#include "boaaa/lvm/LLVMVersionManager.h"

using namespace boaaa;
/*
auto LLVMVersionManager::getInstance() -> LLVMVersionManager& {
	static auto instance = std::unique_ptr<LLVMVersionManager>{ nullptr };
	if (!instance)
		instance.reset(new LLVMVersionManager);
	return *instance.get();
}
*/

std::unique_ptr<DLInterface>
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
		
#define __STR(macro) __STRINGIFY(macro)
		StringRef funcname = __STR(GETDL_INFO);
#undef __STR
		GETDL_INFOFUNC getInfo = (GETDL_INFOFUNC)handle->getFunction(funcname);
		if (!getInfo) {
			//ERROR
			return std::unique_ptr<DLInterface>(nullptr);
		}
		DL_Info _info = getInfo();
		info = new LLVM_Info(filename, std::move(_info.gen()), std::move(handle), _info);

		info->inst->onLoad();
		m_dl_map->insert(std::pair<std::string, LLVM_Info*>(filename.str(), info));
	}
	info->ref_counter++;
	return info->info.gen();
}

void LLVMVersionManager::mayUnload(std::unique_ptr<DLInterface> inst, StringRef filename) {
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




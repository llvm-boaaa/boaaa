#include "boaaa/lvm/LLVMVersionManager.h"

using namespace boaaa;

auto LLVMVersionManager::getInstance() -> LLVMVersionManager& {
	static auto instance = std::unique_ptr<LLVMVersionManager>{ nullptr };
	if (!instance)
		instance.reset(new LLVMVersionManager);
	return *instance.get();
}






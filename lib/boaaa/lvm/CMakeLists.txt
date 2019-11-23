message(STATUS "||=========llvm_versions_Manager========||")
message(STATUS "||")

file(GLOB LLVM_VERSION_MANAGER_Files
	"*.h"
	"*.cpp"
)
message(STATUS "lvm files : ${LLVM_VERSION_MANAGER_Files}")

nm_add_library(boaaa.lvm STATIC ${LLVM_VERSION_MANAGER_Files})
message(STATUS "||added lib: llvm_version_manager")
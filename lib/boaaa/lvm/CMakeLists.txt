message(STATUS "||=========llvm_versions_Manager========||")
message(STATUS "||")

file(GLOB LLVM_VERSION_MANAGER_Files
	"*.h"
	"*.cpp"
)

if(WIN32)
file(GLOB OS_SPECIFIC_MANAGER_files
	"UNIXDLHandle.h"
	"UNIXDLHandle.cpp"
)
else()
file(GLOB OS_SPECIFIC_MANAGER_files
	"WindowsDLHandle.h"
	"WindowsDLHandle.cpp"
)
endif()

foreach(item ${OS_SPECIFIC_MANAGER_files})
LIST(REMOVE_ITEM LLVM_VERSION_MANAGER_Files ${item})
endforeach()

nm_add_library(boaaa.lvm STATIC ${LLVM_VERSION_MANAGER_Files})
target_link_libraries(boaaa.lvm boaaa.support boaaa.vp)
message(STATUS "||added lib: llvm_version_manager")

add_dependencies(boaaa.lvm LLVM_${llvm_boaaa})
target_link_libraries(boaaa.lvm rapidjson LLVM_${llvm_boaaa})
message(STATUS "||add LLVM_${llvm_boaaa}")
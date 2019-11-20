message(STATUS "||================BOAAA=================||")
message(STATUS "||")

file(GLOB SourceFilesBoaaa
	"*.h"
	"*.cpp"
)

#add llvm
set(LLVM_DIR "${BOAAA_SOURCE_DIR}/libraries/llvm/900/cmake/modules/CMakeFiles/")
find_package(LLVM REQUIRED CONFIG)
message(STATUS "||================LLVM==================")
message(STATUS "||LLVM_DIR: ${LLVM_DIR}")
message(STATUS "||Found LLVM Version ${LLVM_PACKAGE_VERSION}")
message(STATUS "||LLVM libs: ${LLVM_AVAILABLE_LIBS} ")
message(STATUS "||LLVM include dirs: ${LLVM_INCLUDE_DIRS} ")
message(STATUS "||======================================")

nm_add_thirdparty_libs(NAME LLVM_CURRENT 
					   LIBS ${LLVM_AVAILABLE_LIBS} 
					   INTERFACE_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS}
					   INTERFACE_COMPILE_OPTIONS -D_DEBUG
					   )

add_definitions(${LLVM_DEFINITIONS})

nm_add_tool(boaaa ${SourceFilesBoaaa})
message(STATUS "||added tool boaaa")
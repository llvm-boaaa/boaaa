message(STATUS "||================BOAAA=================||")
message(STATUS "||")

file(GLOB SourceFilesBoaaa
	"*.h"
	"*.cpp"
)

nm_add_tool(boaaa ${SourceFilesBoaaa})
target_link_libraries(boaaa PUBLIC boaaa.support boaaa.vp boaaa.lvm)

#removed this builddependencies because of multi-definitions of the llvm::classes.
# By removing this loop every needed target has to be build seperate.
#
#foreach(dep ${dll_dependecies})
	#message(STATUS "|| boaaa dll dep: ${dep}")
	#add_dependencies(boaaa ${dep})
#endforeach()

#add llvm_version to boaaa
add_dependencies(boaaa LLVM_${llvm_boaaa})
message(STATUS "||added tool boaaa")

if(UNIX AND NOT APPLE)
message(STATUS "||Set propertie of lv_${version_} as PIC")
set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-ldl")
endif()
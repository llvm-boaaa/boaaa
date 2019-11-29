message(STATUS "||================BOAAA=================||")
message(STATUS "||")

file(GLOB SourceFilesBoaaa
	"*.h"
	"*.cpp"
)

nm_add_tool(boaaa ${SourceFilesBoaaa})
target_link_libraries(boaaa PUBLIC boaaa.lvm)
foreach(dep ${dll_dependecies})
	message(STATUS "|| boaaa dll dep: ${dep}")
	add_dependencies(boaaa ${dep})
	add_dependencies(${dep} BOAAA_LLVM_BUILD)
endforeach()
add_dependencies(boaaa BOAAA_LLVM_BUILD)
message(STATUS "||added tool boaaa")
message(STATUS "||================BOAAA=================||")
message(STATUS "||")

file(GLOB SourceFilesBoaaa
	"*.h"
	"*.cpp"
)

nm_add_tool(boaaa ${SourceFilesBoaaa})
target_link_libraries(boaaa PUBLIC boaaa.support boaaa.vp boaaa.lvm)
foreach(dep ${dll_dependecies})
	message(STATUS "|| boaaa dll dep: ${dep}")
	add_dependencies(boaaa ${dep})
endforeach()
add_dependencies(boaaa LLVM_${llvm_boaaa})
message(STATUS "||added tool boaaa")
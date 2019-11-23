message(STATUS "||================BOAAA=================||")
message(STATUS "||")

file(GLOB SourceFilesBoaaa
	"*.h"
	"*.cpp"
)

nm_add_tool(boaaa ${SourceFilesBoaaa})
target_link_libraries(boaaa PUBLIC boaaa.lvm)
foreach(dep ${dll_dependecies})
add_dependencies(boaaa dep)
endforeach()
message(STATUS "||added tool boaaa")
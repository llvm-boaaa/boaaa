message(STATUS "||================BOAAA=================||")
message(STATUS "||")

file(GLOB SourceFilesBoaaa
	"*.h"
	"*.cpp"
)

add_definitions(${LLVM_DEFINITIONS})

nm_add_tool(boaaa ${SourceFilesBoaaa})
message(STATUS "||added tool boaaa")
message(STATUS "||===================vp=================||")
message(STATUS "||")

file(GLOB VERSION_PARSER_Files
	"*.h"
	"*.cpp"
)

nm_add_library(boaaa.vp STATIC ${VERSION_PARSER_Files})
message(STATUS "||added lib: version parser")
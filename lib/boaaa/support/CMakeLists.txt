message(STATUS "||================support===============||")
message(STATUS "||")

file(GLOB Support_Files
	"*.h"
	"*.cpp"
)

nm_add_library(boaaa.support STATIC ${Support_Files})
message(STATUS "||added lib: support")
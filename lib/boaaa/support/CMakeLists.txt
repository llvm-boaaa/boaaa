message(STATUS "||================support===============||")
message(STATUS "||")

file(GLOB Support_Files
	"*.h"
	"*.cpp"
)

nm_add_library(boaaa.support STATIC ${Support_Files})
message(STATUS "||added lib: support")

if(UNIX AND NOT APPLE)
message(STATUS "||Set propertie of boaaa.support as PIC")
set_target_properties(boaaa.support PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()
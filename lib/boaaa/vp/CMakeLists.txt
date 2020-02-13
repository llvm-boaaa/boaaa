message(STATUS "||===================vp=================||")
message(STATUS "||")

file(GLOB VERSION_PARSER_Files
	"*.h"
	"*.cpp"
)

message(STATUS "||test|| NICEMAKE_LIBS: ${NM_THIRDPARTY_LIBS}")

nm_add_library(boaaa.vp STATIC ${VERSION_PARSER_Files})
message(STATUS "||added lib: version parser")
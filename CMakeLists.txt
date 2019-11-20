cmake_minimum_required(VERSION 3.6)

project(BOAAA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BOAA_SOURCE_DIR = ${CMAKE_CURRENT_SOURCE_DIR})

#descripes all release_XX version of the repo https://github.com/llvm-mirror/llvm
set(all_llvm_versions "10;16;20;21;22;23;24;25;27;28;29;30;31;32;33;34;35;36;37;38;39;40;50;60;70;80;90;master")

# include Nicemake https://github.com/fkutzner/NiceMake
include(cmake/nicemake/NiceMake.cmake)
# set include dir
#set(NM_CONF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

#add libs
	# googletests, remove googletest error of some kind, but currently not important
	#nm_add_gtest()

#add
add_subdirectory(lib)

#add tools
add_subdirectory(tools)
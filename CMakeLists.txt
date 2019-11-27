cmake_minimum_required(VERSION 3.6)

project(BOAAA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(BOAA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

#descripes all release_XX version of the repo https://github.com/llvm-mirror/llvm
set(all_llvm_versions "10;16;20;21;22;23;24;25;27;28;29;30;31;32;33;34;35;36;37;38;39;40;50;60;70;80;90;master")
set(build_llvm_version "90")

message(STATUS "||===========generating BOAAA===========||")
message(STATUS "||")
message(STATUS "|| BOAAA is generated with following")
message(STATUS "|| supported versions:")

#prints supported and build versions
set(ver_string "")
set(ver_list "")
foreach(ver ${build_llvm_version})

list(APPEND ver_list "${ver}")
if(ver_string STREQUAL "")
	set(ver_string "${ver}")
else()
	set(ver_string "${ver_string} - ${ver}")
endif()

LIST(LENGTH ver_list list_length)
if (list_length GREATER_EQUAL 4)
	message(STATUS "|| ${ver_string}")
	set(ver_string "")
	set(ver_list "")
endif()
endforeach(ver)
LIST(LENGTH ver_list list_length)
if (list_length)
	message(STATUS "|| ${ver_string}")
endif()

# include Nicemake https://github.com/fkutzner/NiceMake
include(cmake/nicemake/NiceMake.cmake)
#include cmake_git_clone https://github.com/tschuchortdev/cmake_git_clone
include(cmake/cmake_git_clone/cmake/GitUtils.cmake)


# set include dir
#set(NM_CONF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

#add llvm
set(LLVM_DIR "${BOAAA_SOURCE_DIR}/libraries/llvm/current/cmake/modules/CMakeFiles/")
find_package(LLVM REQUIRED CONFIG)
message(STATUS "||=========LLVM used for BOAAA==========")
message(STATUS "||LLVM_DIR: ${LLVM_DIR}")
message(STATUS "||Found LLVM Version ${LLVM_PACKAGE_VERSION}")
message(STATUS "||LLVM libs: ${LLVM_AVAILABLE_LIBS} ")
message(STATUS "||LLVM include dirs: ${LLVM_INCLUDE_DIRS} ")
message(STATUS "||======================================")
message(STATUS "||")

nm_add_thirdparty_libs(NAME LLVM_BOAAA_VERSION 
					   LIBS ${LLVM_AVAILABLE_LIBS} 
					   INTERFACE_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS}
					   INTERFACE_COMPILE_OPTIONS -D_DEBUG)

add_definitions(${LLVM_DEFINITIONS})

#add libs
	# googletests, remove googletest error of some kind, but currently not important
	#nm_add_gtest()

#add
add_subdirectory(lib)

#add tools
add_subdirectory(tools)

message(STATUS "||=========BOAAA generation done========||")
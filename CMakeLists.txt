cmake_minimum_required(VERSION 3.6)

project(BOAAA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#descripes all release_XX version of the repo https://github.com/llvm-mirror/llvm
set(all_llvm_versions "10;16;20;21;22;23;24;25;27;28;29;30;31;32;33;34;35;36;37;38;39;40;50;60;70;80;90;master")
set(build_llvm_version "40;50;90")

message(STATUS "||===========generating BOAAA===========||")
message(STATUS "||")
message(STATUS "|| BOAAA is generated with following")
message(STATUS "|| supported versions:")

unset(dll_dependecies CACHE)
unset(llvm_dependencies CACHE)

set(BOAAA_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

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
include(cmake/checkOrCreateFoulder.cmake)

message(STATUS "|| test: ${BOAAA_SOURCE_DIR}/libraries/llvm/build/")
# set include dir
#set(NM_CONF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
	message(STATUS "|| configure cmake for boaaa")
	if(WIN32)
		if(EXISTS "${BOAAA_SOURCE_DIR}/libraries/llvm/build/LLVM.sln") #skip generate for existing .sln
			message(STATUS "|| skip cmake generation...")
		else()
		execute_process( #clean repo
			COMMAND git clean -d -f -x
			WORKING_DIRECTORY "${BOAAA_SOURCE_DIR}/libraries/llvm/build/"
			OUTPUT_VARIABLE   o)
		message(STATUS "|| generating cmake ...")
		boaaa_checkOrCreateFoulder("${BOAAA_SOURCE_DIR}/libraries/llvm/build")
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		message(STATUS "|| using x64 generator")
		execute_process(
			COMMAND cmake "../source" -G "Visual Studio 16 2019" -Thost=x64
			WORKING_DIRECTORY "${BOAAA_SOURCE_DIR}/libraries/llvm/build/"
			OUTPUT_VARIABLE   o)
		else()
		message(STATUS "|| using x32 generator")
		execute_process(
			COMMAND cmake "../source" -G "Visual Studio 16 2019"
			WORKING_DIRECTORY ${BOAAA_SOURCE_DIR}"/libraries/llvm/build/"
			OUTPUT_VARIABLE   o)
		endif()	
		endif()
	else()
		message(STATUS "|| generating cmake ...")
		execute_process(
			COMMAND cmake ../source
			WORKING_DIRECTORY "${BOAAA_SOURCE_DIR}/libraries/llvm/build/"
			OUTPUT_VARIABLE   o)
	endif()

#add llvm
set(LLVM_DIR "${BOAAA_SOURCE_DIR}/libraries/llvm/build/cmake/modules/CMakeFiles/")
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
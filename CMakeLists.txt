cmake_minimum_required(VERSION 3.6)

project(BOAAA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "||===========generating BOAAA===========||")
message(STATUS "||")

find_package(Git)
if(NOT GIT_FOUND)
    message(FATAL_ERROR "git not found!")
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/LLVMVersionConfig.cmake")
include(LLVMVersionConfig.cmake)
else()
message(STATUS "|| WARNING: configure installed LLVM-Versions in LLVMVersionConfig.cmake")
message(STATUS "||")
include(LLVMVersionConfig-template.cmake)
endif()

include(cmake/LLVMVersionIncludeHelp.cmake)
include(cmake/replaceLast.cmake)
include(cmake/copyAndRename.cmake)
include(cmake/checkOrCreateFoulder.cmake)
include(cmake/createLLVMWrapperTarget.cmake)

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

message(STATUS "|| configure LLVM versions:")

foreach(version_ ${build_llvm_version})

	boaaa_create_llvm_wrapper(${version_})

endforeach(version_)

message(STATUS "|| configured all versions")

	set(BOAAA_LLVM_NAME "LLVM_${llvm_boaaa}")

	message(STATUS "||>>=======LLVM used for BOAAA=========")
	message(STATUS "||LLVM Version ${${BOAAA_LLVM_NAME}_VERSION}")
	message(STATUS "||LLVM libs: ${${BOAAA_LLVM_NAME}_LIBS} ")
	message(STATUS "||LLVM include dirs: ${${BOAAA_LLVM_NAME}_INCLUDE_DIRS} ")

nm_add_thirdparty_libs(NAME LLVM_BOAAA_VERSION 
					   LIBS ${${BOAAA_LLVM_NAME}_LIBS} 
					   INTERFACE_INCLUDE_DIRS ${${BOAAA_LLVM_NAME}_INCLUDE_DIRS}
					   INTERFACE_COMPILE_OPTIONS -D_DEBUG)


#add_definitions(${LLVM_DEFINITIONS})

#add libs
	# googletests, remove googletest error of some kind, but currently not important
	#nm_add_gtest()

#add
add_subdirectory(lib)

#add tools
add_subdirectory(tools)

message(STATUS "||=========BOAAA generation done========||")
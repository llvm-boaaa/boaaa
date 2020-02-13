cmake_minimum_required(VERSION 3.6)

project(BOAAA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



set(GCC_COVERAGE_COMPILE_FLAG "-lstdc++fs")

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

message(STATUS "||")
message(STATUS "|| BOAAA is generated with following supported versions:")

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

message(STATUS "||")
message(STATUS "|| configure LLVM versions:")

foreach(version_ ${build_llvm_version})

	message(STATUS "||")
	unset(LLVM_DIR)
	boaaa_create_llvm_wrapper(${version_})
	message(STATUS "||>>=======LLVM used for BOAAA=========")
	message(STATUS "||LLVM Version :      ${LLVM_${version_}_VERSION}")
	#message(STATUS "||LLVM DIR :          ${LLVM_${version_}_DIR}")
	message(STATUS "||LLVM Link Dirs :    ${LLVM_${version_}_LIBRARY_DIR}")
	message(STATUS "||LLVM libs :         ${LLVM_${version_}_LIBS} ")
	message(STATUS "||LLVM include dirs : ${LLVM_${version_}_INCLUDE_DIRS} ")


endforeach(version_)

message(STATUS "|| configured all versions")
message(STATUS "||")

	set(BOAAA_LLVM_NAME "LLVM_${llvm_boaaa}")

	message(STATUS "||>>=======LLVM used for BOAAA=========")
	message(STATUS "||LLVM Version ${${BOAAA_LLVM_NAME}_VERSION}")
	message(STATUS "||LLVM libs: ${${BOAAA_LLVM_NAME}_LIBS} ")
	message(STATUS "||LLVM include dirs: ${${BOAAA_LLVM_NAME}_INCLUDE_DIRS} ")

#add_definitions(${LLVM_DEFINITIONS})

#add libs
	# googletests, remove googletest error of some kind, but currently not important
	#nm_add_gtest()

#add
add_subdirectory(lib)

#add tools
add_subdirectory(tools)

message(STATUS "||=========BOAAA generation done========||")
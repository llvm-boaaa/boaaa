message(STATUS "||=============llvm_versions============||")
message(STATUS "||")

include(ExternalProject)
#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


#coppied from nicemake/TargetCreation.cmake::nm_add_library
#moddified, so every file in FOULDER_NAME get linked to the lib.
#all individual source files have to be added by the cmake-function target_source
function(boaa_add_llvm_library NAME FOULDER_NAME KIND)
  if((KIND MATCHES "OBJECT") AND (CMAKE_VERSION VERSION_LESS "3.12"))
    # Pre-3.12 CMake has incomplete support for object libraries, e.g.
    # no way of importing compile options and include dirs via
    # target_link_libraries:
    message(FATAL_ERROR "NiceMake supports object libraries only with CMake 3.12 and later")
  endif()

  set(nm_target_include_dir "${PROJECT_SOURCE_DIR}/${NM_CONF_INCLUDE_DIR}/${FOLDER_NAME}")
  file(GLOB LIBRARY_HEADERS "${nm_target_include_dir}/*.h"
                            "${nm_target_include_dir}/*.hh"
                            "${nm_target_include_dir}/*.hpp"
                            "${nm_target_include_dir}/*.hxx"
                            "${nm_target_include_dir}/*.H")

  if(${KIND} STREQUAL "OBJECT-STATIC" OR ${KIND} STREQUAL "OBJECT-SHARED")
    set(simple_kind "OBJECT")
  else()
    set(simple_kind ${KIND})
  endif()
  
  add_library(${NAME} ${simple_kind} ${LIBRARY_HEADERS} ${ARGN})

  LIST(APPEND dll_dependecies ${NAME})

  set_property(TARGET "${NAME}" PROPERTY FOLDER "Libraries/${FOLDER_NAME}")
  set_property(TARGET "${NAME}" PROPERTY PROJECT_LABEL "Library")

  #add only one llvm version to each dynamic lib
  #target_link_libraries(${NAME} ${NM_THIRDPARTY_LIBS})
  nm_detail_add_compile_options_to_lib(${NAME} ${KIND})

  target_include_directories(${NAME} PUBLIC ${PROJECT_SOURCE_DIR}/${NM_CONF_INCLUDE_DIR})
endfunction()

file(GLOB DL_files
	"*.h"
	"*.cpp"
	"${BOAAA_SOURCE_DIR}/include/boaaa/*.h"
)

set(tmp_FOLDER_NAME ${FOULDER_NAME})

find_package(Git)
if(NOT GIT_FOUND)
    message(FATAL_ERROR "git not found!")
endif()

#############################################################
#						start foreach						#
#															#

foreach(version_ ${build_llvm_version} )

	message(STATUS "||>>>>>---------release_${version_}---------<<<<<")

	if (${version_} STREQUAL "master")
	set(branch ${version_})
	else()
	set(branch "release_${version_}")
	endif()

	set(_path "${BOAAA_SOURCE_DIR}/llvm_version/")

	if (EXISTS "${_path}/${branch}")
		message(STATUS "|| pulling from branch ${branch} ...")
		execute_process(
			COMMAND ${GIT_EXECUTABLE} pull origin ${branch}
			WORKING_DIRECTORY "${_path}/${branch}"
			RESULT_VARIABLE   result)
	else()
		message(STATUS "|| clone from branch ${branch} ...")
		execute_process(
			COMMAND ${GIT_EXECUTABLE} clone --branch ${branch} https://github.com/llvm-mirror/llvm.git ${branch}
			WORKING_DIRECTORY ${_path}
			RESULT_VARIABLE   result)
	endif()

	if(WIN32)
		if(EXISTS "${_path}/${branch}/LLVM.sln") #skip generate for existing .sln
			message(STATUS "|| skip cmake generation...")
		else()
		execute_process( #clean repo
			COMMAND git clean -d -f -x
			WORKING_DIRECTORY "${_path}/${branch}"
			OUTPUT_VARIABLE   o)
		message(STATUS "|| generating cmake ...")
		if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		message(STATUS "|| using x64 generator")
		execute_process(
			COMMAND cmake "." -G "Visual Studio 16 2019" -Thost=x64
			WORKING_DIRECTORY "${_path}/${branch}"
			OUTPUT_VARIABLE   o)
		else()
		message(STATUS "|| using x32 generator")
		execute_process(
			COMMAND cmake "." -G "Visual Studio 16 2019" -A Win32
			WORKING_DIRECTORY "${_path}/${branch}"
			OUTPUT_VARIABLE   o)
		endif()	
		endif()
	else()
		message(STATUS "|| generating cmake ...")
		execute_process(
			COMMAND cmake .
			WORKING_DIRECTORY "${_path}/${branch}"
			OUTPUT_VARIABLE   o)
	endif()
	
	set(LLVM_NAME "LLVM_${branch}")
	set(LLVM_NAME_DIR "${LLVM_NAME}_DIR")

	set(${LLVM_NAME_DIR} "${_path}/${branch}/cmake/modules/CMakeFiles")

	if(EXISTS ${${LLVM_NAME_DIR}}/${LLVM_NAME}Config.cmake)
		message(STATUS "|| No renaming to do file: ${LLVM_NAME}.cmake already exists.")
	else()
		if(EXISTS ${${LLVM_NAME_DIR}}/LLVMConfig.cmake)
			message(STATUS "|| renaming LLVMConfig.cmake to ${LLVM_NAME}Config.cmake")
			file(RENAME ${${LLVM_NAME_DIR}}/LLVMConfig.cmake ${${LLVM_NAME_DIR}}/${LLVM_NAME}Config.cmake)
		else()
		message(STATUS "ERROR: llvm_version: ${ver} of branch: {branch} is not properly configured")
		endif()
	endif()

	find_package(${LLVM_NAME} CONFIG REQUIRED)
	message(STATUS "||>>=====LLVM used for ${branch}=======")
	message(STATUS "||LLVM_DIR: ${LLVM_DIR}")
	message(STATUS "||Found LLVM Version ${LLVM_PACKAGE_VERSION}")
	message(STATUS "||LLVM libs: ${LLVM_AVAILABLE_LIBS} ")
	message(STATUS "||LLVM include dirs: ${LLVM_INCLUDE_DIRS} ")
	message(STATUS "||>>====================================")
	message(STATUS "||")

	add_custom_target(${LLVM_NAME})

	LIST(APPEND LLVM_VERSION_TARGETS "${branch}")

	#add_subdirectory(${_path}/${branch} ${_path}/${branch} EXCLUDE_FROM_ALL)

	file(GLOB _dl_files
		"${version_}/*.h"
		"${version_}/*.cpp"
	)
	boaa_add_llvm_library(boaaa.lv_${version_} "boaaa/lv" MODULE ${DL_files} ${_dl_files})
	target_include_directories(boaaa.lv_${version_} PRIVATE ${LLVM_INCLUDE_DIRS})
	target_link_libraries(boaaa.lv_${version_} ${LLVM_AVAILABLE_LIBS})

	target_compile_definitions(boaaa.lv_${version_} 
	PRIVATE 
		LLVM_VERSION=${version_}
		LLVM_VERSION_${version_}=${version_} 
		BUILD_SHARED_LIB)
	add_dependencies(boaaa.lv_${version_} ${LLVM_NAME})
	message(STATUS "||added dynamic lib with llvm_version ${version_}: lv_${version_}")
endforeach(version_)

#															#
#						end foreach							#
#############################################################


set(FOLDER_NAME ${tmp_FOLDER_NAME})


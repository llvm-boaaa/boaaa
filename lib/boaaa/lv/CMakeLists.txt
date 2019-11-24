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

foreach(version_ ${build_llvm_version} )
	if (${version_} STREQUAL "master")
	set(branche ${version_})
	else()
	set(branche "release_${version_}")
	endif()

	set(_path "${BOAAA_SOURCE_DIR}/llvm_version/${branche}")

	ExternalProject_Add(${branche}
		PREFIX			= "${_path}"
		SOURCE_DIR		= "${_path}"
		STAMP_DIR		= "${_path}"
		BINARY_DIR		= "${_path}"
		GIT_REPOSITORY	= "https://github.com/llvm-mirror/llvm.git"
		GIT_TAG			= "origin/${branche}"
		)

	add_subdirectory(${_path} ${_path} EXCLUDE_FROM_ALL)

	set(LLVM_DIR "${_path}/cmake/modules/CMakeFiles/")
	find_package(LLVM REQUIRED CONFIG)
	message(STATUS "||========LLVM used for ${branche}=========")
	message(STATUS "||LLVM_DIR: ${LLVM_DIR}")
	message(STATUS "||Found LLVM Version ${LLVM_PACKAGE_VERSION}")
	message(STATUS "||LLVM libs: ${LLVM_AVAILABLE_LIBS} ")
	message(STATUS "||LLVM include dirs: ${LLVM_INCLUDE_DIRS} ")
	message(STATUS "||======================================")
	message(STATUS "||")

	file(GLOB _dl_files
		"${version_}/*.h"
		"${version_}/*.cpp"
	)
	boaa_add_llvm_library(boaaa.lv_${version_} "boaaa/lv" MODULE ${DL_files} ${_dl_files})
	target_include_directories(${LLVM_INCLUDE_DIRS})
	target_link_libraries(${LLVM_AVAILABLE_LIBS})

	target_compile_definitions(boaaa.lv_${version_} 
	PRIVATE 
		LLVM_VERSION=${version_}
		LLVM_VERSION_${version_}=${version_} 
		BUILD_SHARED_LIB)
	message(STATUS "||added dynamic lib with llvm_version ${version_}: lv_${version_}")
endforeach(version_)

set(FOLDER_NAME ${tmp_FOLDER_NAME})



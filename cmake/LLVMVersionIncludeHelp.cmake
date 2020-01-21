function(cmake_global_list LIST ADD IDENT)
set(${LIST} "${${LIST}};${ADD}" CACHE STRING ${IDENT} FORCE)
endfunction()

macro(boaaa_add_llvm_version)
	cmake_parse_arguments(
    P_ARGS
    ""
    "NAME"
    "LIBS;INTERFACE_INCLUDE_DIRS;INTERFACE_LINK_DIRS;INTERFACE_COMPILE_OPTIONS"
    ${ARGN}
  )
  if(NOT P_ARGS_NAME)
    message(FATAL "boaaa_add_llvm_version: NAME must be specified")
  endif()
  if(NOT P_ARGS_LIBS)
    message(FATAL "boaaa_add_llvm_version: LIBS must be specified")
  endif()

  set(nm_dummy_target_name ${P_ARGS_NAME})
  add_library(${nm_dummy_target_name} INTERFACE)
  #target_link_directories(${nm_dummy_target_name} BEFORE INTERFACE ${P_ARGS_INTERFACE_LINK_DIRS})
  #target_link_directories(${nm_dummy_target_name} INTERFACE ${P_ARGS_INTERFACE_LINK_DIRS})
  target_link_libraries(${nm_dummy_target_name} INTERFACE ${P_ARGS_LIBS})
  target_include_directories(${nm_dummy_target_name} INTERFACE ${P_ARGS_INTERFACE_INCLUDE_DIRS})
  target_compile_options(${nm_dummy_target_name} INTERFACE ${P_ARGS_INTERFACE_COMPILE_OPTIONS})
  #target_link_options(${nm_dummy_target_name} INTERFACE "/FORCE:MULTIPLE")

  if("LLVM_${llvm_boaaa}" STREQUAL ${P_ARGS_NAME})
  list(APPEND NM_THIRDPARTY_LIBS ${nm_dummy_target_name})
  message(STATUS "|| linked LLVM BOAAA Version")
  endif()

endmacro()

#copied from nicemake/TargetCreation.cmake::nm_add_library
#moddified, so every file in FOULDER_NAME get linked to the lib.
#all individual source files have to be added by the cmake-function target_source
function(boaaa_add_llvm_library REGISTERED NAME FOULDER_NAME BRANCH KIND)
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

  cmake_global_list(dll_dependecies ${NAME} "VAR0")
  if (NOT ${REGISTERED})
  cmake_global_list(llvm_dependencies ${BRANCH} "VAR1")
  endif()

  set_property(TARGET "${NAME}" PROPERTY FOLDER "Libraries/${FOLDER_NAME}")
  set_property(TARGET "${NAME}" PROPERTY PROJECT_LABEL "Library")

  #add only one llvm version to each dynamic lib
  #target_link_libraries(${NAME} ${NM_THIRDPARTY_LIBS})
  nm_detail_add_compile_options_to_lib(${NAME} ${KIND})

  target_include_directories(${NAME} PUBLIC ${PROJECT_SOURCE_DIR}/${NM_CONF_INCLUDE_DIR})
endfunction()


#from github: https://github.com/toeb/cmakepp/blob/master/cmake/string/string_starts_with.cmake
function(string_starts_with str search)
  string(FIND "${str}" "${search}" out)
  if("${out}" EQUAL 0)
    return(true)
  endif()
  return(false)
endfunction()
cmake_minimum_required(VERSION 3.6)

project(BOAAA)

# include Nicemake https://github.com/fkutzner/NiceMake
include(cmake/nicemake/NiceMake.cmake)

# include llvm

set(NM_CONF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(LLVM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libraries/llvm/current/cmake/modules/)


find_package(LLVM REQUIRED CONFIG)

message(STATUS "================LLVM==================")
message(STATUS "Found LLVM Version ${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM libs: ${LLVM_AVAILABLE_LIBS} ")
message(STATUS "LLVM include dirs: ${LLVM_AVAILABLE_LIBS} ")
message(STATUS "======================================")

nm_add_thirdparty_libs(NAME LLVM_CURRENT 
					   LIBS ${LLVM_AVAILABLE_LIBS} 
					   INTERFACE_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS}
					   INTERFACE_COMPILE_OPTIONS -D_DEBUG
					   -D__STDC_CONSTANT_MACROS
					   -D__STDC_FORMAT_MACROS
					   -D__STDC_LIMIT_MACROS
					   )

add_definitions(${LLVM_DEFINITIONS})
# googletests

nm_add_gtest()

#add submodules
add_subdirectory(tools)
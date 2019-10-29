cmake_minimum_required(VERSION 3.6)

project(BOAAA)

# include Nicemake https://github.com/fkutzner/NiceMake
include(cmake/nicemake/NiceMake.cmake)

# include llvm

set(NM_CONF_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

set(LLVM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libraries/llvm/current/cmake/modules/)



find_package(LLVM REQUIRED CONFIG)

nm_add_thirdparty_libs(NAME LLVM_CURRENT 
					   LIBS ${LLVM_AVAILABLE_LIBS} 
					   INTERFACE_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS}
					   INTERFACE_COMPILE_OPTIONS -D_DEBUG
					   -D__STDC_CONSTANT_MACROS
					   -D__STDC_FORMAT_MACROS
					   -D__STDC_LIMIT_MACROS
					   )
# googletests

nm_add_gtest()
function(boaaa_add_analysis)
    cmake_parse_arguments(
    P_ARGS
    ""
    "NAME;VERSION;MAKRO"
    "INCLUDE_DIRS;LINK_LIBS"
    ${ARGN}
  )

set(ANALYSIS_LIST "LLVM_${P_ARGS_VERSION}_ANALYSISES")
set(ANALYSIS_LIST_VAR "LLVM_${P_ARGS_VERSION}_ANA")
set(INCLUDE_LIST "LLVM_${P_ARGS_VERSION}_ADDITIONAL_INCLUDE")
set(INCLUDE_LIST_VAR "LLVM_${P_ARGS_VERSION}_INCLUDE")
set(MAKRO_LIST "LLVM_${P_ARGS_VERSION}_MAKRO")
set(MAKRO_LIST_VAR "LLVM_${P_ARGS_VERSION}_MAKRO_LIST")

message(STATUS "||test|| ${P_ARGS_NAME} ${P_ARGS_VERSION}")

cmake_global_list(${ANALYSIS_LIST} ${P_ARGS_NAME} ${ANALYSIS_LIST_VAR})

foreach(inc_dir ${P_ARGS_INCLUDE_DIRS})

message(STATUS "||test|| ${inc_dir}")
cmake_global_list(${INCLUDE_LIST} "${inc_dir}" ${INCLUDE_LIST_VAR})

endforeach()

cmake_global_list(${MAKRO_LIST} ${P_ARGS_MAKRO} ${MAKRO_LIST_VAR})

#link dependecies including specifyed llvm_version
target_link_libraries(${P_ARGS_NAME} PRIVATE "LLVM_${P_ARGS_VERSION}" ${P_ARGS_LINK_LIBS})

set_property(TARGET ${P_ARGS_NAME} PROPERTY CXX_STANDARD 14)

message(STATUS "|| added Analysis ${P_ARGS_NAME} to lv.${P_ARGS_VERSION}")

endfunction()
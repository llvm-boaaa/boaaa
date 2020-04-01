function(boaaa_add_analysis VERSION NAME)

set(LIST "LLVM_${VERSION}_ANALYSISES")
set(LIST_VAR "LLVM_${VERSION}_ANA")

cmake_global_list(${LIST} ${NAME} ${LIST_VAR})

#link dependecies including specifyed llvm_version
target_link_libraries(${NAME} PRIVATE "LLVM_${VERSION}" ${ARGN})

set_property(TARGET ${NAME} PROPERTY CXX_STANDARD 14)

message(STATUS "|| added Analysis ${NAME} to lv.${VERSION}")

endfunction()
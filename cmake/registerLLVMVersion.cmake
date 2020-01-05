function(boaaa_registerLLVMVersion)
    cmake_parse_arguments(
    P_ARGS
    ""
    "VERSION;PATH"
    ""
    ${ARGN}
  )
    if(NOT P_ARGS_VERSION "[0-9]+[0-9]")
    message(FATAL_ERROR "|| ${P_ARGS_VERSION} dont match regex [0-9]+[0-9]")
    endif()
    
    set(LLVM_${P_ARGS_VERSION}_REGISTERED TRUE PARENT_SCOPE)
    set(LLVM_${P_ARGS_VERSION}_PATH ${P_ARGS_PATH} PARENT_SCOPE)
endfunction()
function(boaaa_registerLLVMVersion VERSION_ PATH_)
    if(NOT "${VERSION_}" MATCHES "[0-9]+[0-9]")
    message(FATAL_ERROR "|| ${VERSION_} dont match regex [0-9]+[0-9]")
    else()
    message(STATUS "|| Version ${VERSION_} registered")
    endif()
    
    set(LLVM_release_${VERSION_}_REGISTERED TRUE PARENT_SCOPE)
    set(LLVM_release_${VERSION_}_PATH ${PATH_} PARENT_SCOPE)
endfunction()
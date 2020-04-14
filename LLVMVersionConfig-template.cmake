# boaaa uses different Versions of LLVM, cmake is configured to download and build all
# required LLVM Versions if needed. However this file allowes you to register installed 
# LLVM versions in order to save diskspace.  
#
# In cmake context of BOAAA LLVM_Version is allways defined as XY,
# where the normale LLVM Version (in CMAKE format) is defined as X.Y.Z, 
#
# BOAAA don't uses patch versions, because the LLVM Version only contain bugfixes and no huge changes in sourcecode.
#
# in the following two lines, all valid versions of LLVM are listed(at the moment).
# The Standart configuration for BOAAA is defined in build_llvm_version, this versions are implemented and tested, 
# but of course you could add a other versions aswell, by implementing the dll.
set(all_llvm_versions "30;31;32;33;34;35;36;37;38;39;40;50;60;70;71;80;90")
set(build_llvm_version "40;50;60;71;80;90")

set(llvm_boaaa "90")

include(cmake/registerLLVMVersion.cmake)

# LLVM_40
#boaaa_registerLLVMVersion("40" "absolute/or/relative/path")

# LLVM_50
#boaaa_registerLLVMVersion("50" "absolute/or/relative/path")

#enable Sea-Dsa
set(ENABLE_SEA_DSA false)

#boost_1.55 or graiter path for Sea-Dsa, if used (on windows testet boost 1.65.1, below wouldn't compile)
#set(CUSTOM_BOOST_1_55_ROOT "/path/to/boost/root")

# LLVM_60
#boaaa_registerLLVMVersion("60" "absolute/or/relative/path")

# LLVM_71
#boaaa_registerLLVMVersion("71" "absolute/or/relative/path")

# LLVM_80
#boaaa_registerLLVMVersion("80" "absolute/or/relative/path")

# LLVM_90
#boaaa_registerLLVMVersion("90" "absolute/or/relative/path")

#Windows
#set(BOAAA_LIB_PREFIX )
#set(BOAAA_LIB_EXTENSION ".lib")
#set(BOAAA_DYNLIB_PREFIX )
#set(BOAAA_DYNLIB_EXTENSION ".dll")
#set(BOAAA_LLVM_DYNLIB_LIST "")

#Mac
#set(BOAAA_LIB_PREFIX "lib")
#set(BOAAA_LIB_EXTENSION ".a")
#set(BOAAA_DYNLIB_PREFIX "lib")
#set(BOAAA_DYNLIB_EXTENSION ".dylib")
#set(BOAAA_LLVM_DYNLIB_LIST "LTO;Remarks;LLVM")


#Linux
#set(BOAAA_LIB_PREFIX "lib")
#set(BOAAA_LIB_EXTENSION ".a")
#set(BOAAA_DYNLIB_PREFIX "lib")
#set(BOAAA_DYNLIB_EXTENSION ".so")
#set(BOAAA_LLVM_DYNLIB_LIST "LTO;Remarks")
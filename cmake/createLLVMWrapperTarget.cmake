function(boaaa_create_llvm_wrapper version_)

message(STATUS "||>>>>>---------release_${version_}---------<<<<<")

	set(branch "release_${version_}")
	set(LLVM_NAME "LLVM_${branch}")
	set(LLVM_NAME_DIR "${LLVM_NAME}_DIR")

	if(${LLVM_NAME}_REGISTERED)
		#>> llvm installed
		set(_path ${${LLVM_NAME}_PATH})
		set(${LLVM_NAME_DIR} "${_path}/lib/cmake/llvm")
	else()
		#>> llvm need to get build
		set(_path "${BOAAA_SOURCE_DIR}/llvm_version")

		boaaa_checkOrCreateFoulder("${_path}/${branch}")

		#>> pull or clone git
		if (EXISTS "${_path}/${branch}/source")
			message(STATUS "|| pulling from branch ${branch} ...")
			execute_process(
				COMMAND git pull origin ${branch}
				WORKING_DIRECTORY "${_path}/${branch}/source"
				RESULT_VARIABLE   result)
		else()
			message(STATUS "|| clone from branch ${branch} ...")
			execute_process(
				COMMAND git clone https://github.com/llvm-mirror/llvm.git --branch ${branch} --single-branch source
				WORKING_DIRECTORY "${_path}/${branch}"
				RESULT_VARIABLE   result)
		endif()

		boaaa_checkOrCreateFoulder("${_path}/${branch}/build")

		#>> generate cmake for llvm_version
		if(WIN32)
			if(EXISTS "${_path}/${branch}/build/LLVM.sln") #skip generate for existing .sln
				message(STATUS "|| skip cmake generation...")
			else()
			execute_process( #clean repo
				COMMAND git clean -d -f -x
				WORKING_DIRECTORY "${_path}/${branch}/build"
				OUTPUT_VARIABLE   o)
			message(STATUS "|| generating cmake ...")

			if((${version_} STREQUAL "40") OR (${version_} STREQUAL "50"))
			file(COPY ${BOAAA_SOURCE_DIR}/replace/STLExtras.h DESTINATION "${_path}/${branch}/source/include/llvm/ADT")
			endif()

			if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			message(STATUS "|| using x64 generator")
			execute_process(
				COMMAND ${CMAKE_COMMAND} "../source" -G "Visual Studio 16 2019" -Thost=x64
				WORKING_DIRECTORY "${_path}/${branch}/build"
				OUTPUT_VARIABLE   o)
			else()
			message(STATUS "|| using x32 generator")
			execute_process(
				COMMAND ${CMAKE_COMMAND} "../source" -G "Visual Studio 16 2019" -A Win32
				WORKING_DIRECTORY "${_path}/${branch}/build"
				OUTPUT_VARIABLE   o)
			endif()	
			endif()
		else()
			message(STATUS "|| generating cmake ...")
			message(STATUS "|| use CC=${CMAKE_C_COMPILER}, CXX=${CMAKE_CXX_COMPILER}")
			message(STATUS "|| generator: -G\"${CMAKE_GENERATOR}\"")
			execute_process(
				COMMAND ${CMAKE_COMMAND} "../source" -G ${CMAKE_GENERATOR} -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER} -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
 				WORKING_DIRECTORY "${_path}/${branch}/build"
				OUTPUT_VARIABLE   o)
		endif()
		
		set(${LLVM_NAME_DIR} "${_path}/${branch}/build/cmake/modules/CMakeFiles")
	endif()

	#>> find package 
	if(EXISTS ${${LLVM_NAME_DIR}}/${LLVM_NAME}Config.cmake)
		message(STATUS "|| No renaming to do file: ${LLVM_NAME}.cmake already exists.")
	else()
		if(EXISTS ${${LLVM_NAME_DIR}}/LLVMConfig.cmake)
			message(STATUS "|| renaming LLVMConfig.cmake to ${LLVM_NAME}Config.cmake")
			#file(READ ${${LLVM_NAME_DIR}}/LLVMConfig.cmake __tmp_content)
			#file(WRITE ${${LLVM_NAME_DIR}}/${LLVM_NAME}Config.cmake ${__tmp_content})
			#removed rename, because of lost file LLVMConfig in installed LLVM Versions
			#file(RENAME ${${LLVM_NAME_DIR}}/LLVMConfig.cmake ${${LLVM_NAME_DIR}}/${LLVM_NAME}Config.cmake)
			boaaa_copy_and_rename(${${LLVM_NAME_DIR}} LLVMConfig.cmake ${${LLVM_NAME_DIR}} ${LLVM_NAME}Config.cmake)
			#file(COPY ${${LLVM_NAME_DIR}}/LLVMConfig.cmake ${${LLVM_NAME_DIR}}/${LLVM_NAME}Config.cmake)
		else()
		message(FATAL_ERROR "ERROR: llvm_version: ${ver} of branch: ${branch} is not properly configured")
		endif()
	endif()

	find_package(${LLVM_NAME} CONFIG REQUIRED)

	#probably wrong configuration of cmake in release_40 and release_50, not needed, so dependencies get removed
	if(${version_} STREQUAL "40")
	set(REMOVE_LIST "LLVMAArch64AsmPrinter;LLVMAMDGPUAsmPrinter;LLVMARMAsmPrinter;LLVMBPFAsmPrinter;LLVMLanaiAsmPrinter;LLVMMipsAsmPrinter;LLVMMSP430AsmPrinter;LLVMNVPTXAsmPrinter;LLVMPowerPCAsmPrinter;LLVMSparcAsmPrinter;LLVMSystemZAsmPrinter;LLVMX86AsmPrinter;LLVMXCoreAsmPrinter;LLVMVectorize")
	foreach(rem ${REMOVE_LIST})
	list(REMOVE_ITEM LLVM_AVAILABLE_LIBS ${rem})
	endforeach()
	endif()

	if(${version_} STREQUAL "50")
	set(REMOVE_LIST "LLVMAArch64AsmPrinter;LLVMAMDGPUAsmPrinter;LLVMARMAsmPrinter;LLVMBPFAsmPrinter;LLVMLanaiAsmPrinter;LLVMMipsAsmPrinter;LLVMMSP430AsmPrinter;LLVMNVPTXAsmPrinter;LLVMPowerPCAsmPrinter;LLVMSparcAsmPrinter;LLVMSystemZAsmPrinter;LLVMX86AsmPrinter;LLVMXCoreAsmPrinter;LLVMLanaiInstPrinter")
	foreach(rem ${REMOVE_LIST})
	list(REMOVE_ITEM LLVM_AVAILABLE_LIBS ${rem})
	endforeach()
	endif()

	message(STATUS "||>>=====LLVM used for ${branch}=======")
	message(STATUS "||LLVM_DIR: ${LLVM_DIR}")
	message(STATUS "||Found LLVM Version ${LLVM_PACKAGE_VERSION}")
	message(STATUS "||LLVM libs: ${LLVM_AVAILABLE_LIBS} ")
	message(STATUS "||LLVM include dirs: ${LLVM_INCLUDE_DIRS} ")
	message(STATUS "||>>====================================")
	message(STATUS "||")

	#>> check installed version
	if(${LLVM_NAME}_REGISTERED)
		string(REPLACE "." "" __tmp_version "${LLVM_PACKAGE_VERSION}")
		if(NOT "${__tmp_version}" MATCHES "^${version_}")
			message(FATAL_ERROR "|| error version of LLVM ${LLVM_PACKAGE_VERSION} is incompatible with ${version_}")
		endif()
		#TODO check INCLUDES
	endif()

	set(LLVM_${version_}_LIBS ${LLVM_AVAILABLE_LIBS})
	set(LLVM_${version_}_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS})

	#>> if not installed fix include dirs
	if(NOT ${LLVM_NAME}_REGISTERED)
	boaaa_replace_last("build" "source" LLVM_${version_}_INCLUDE_DIRS ${LLVM_INCLUDE_DIRS})
	set(LLVM_${version_}_INCLUDE_DIRS "${LLVM_${version_}_INCLUDE_DIRS};${LLVM_INCLUDE_DIRS}")
	message(STATUS "||fixed include dir path for generated llvm version")
	message(STATUS "|| path: ${LLVM_${version_}_INCLUDE_DIRS}")
	endif()

	boaaa_add_llvm_version(NAME LLVM_${version_}
					   LIBS ${LLVM_${version_}_LIBS}
					   INTERFACE_INCLUDE_DIRS ${LLVM_${version_}_INCLUDE_DIRS}
					   INTERFACE_COMPILE_OPTIONS -D_DEBUG)
	
	#return definitions
	set(LLVM_${version_}_VERSION "${LLVM_PACKAGE_VERSION}" PARENT_SCOPE)
	set(LLVM_${version_}_LIBS "${LLVM_${version_}_LIBS}" PARENT_SCOPE)
	set(LLVM_${version_}_INCLUDE_DIRS "${LLVM_${version_}_INCLUDE_DIRS}" PARENT_SCOPE)
endfunction(boaaa_create_llvm_wrapper)
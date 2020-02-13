macro(boaaa_create_llvm_wrapper version_)

message(STATUS "||>>>>>---------release_${version_}---------<<<<<")

	set(branch "release_${version_}")
	set(LLVM_NAME "LLVM_${branch}")
	set(LLVM_NAME_DIR "${LLVM_NAME}_DIR")
	set(LLVM_LINK_DIR "${LLVM_NAME}_LINK_DIR")

	if(${LLVM_NAME}_REGISTERED)
		#>> llvm installed
		set(_path ${${LLVM_NAME}_PATH})
		set(${LLVM_NAME_DIR} "${_path}/lib/cmake/llvm")
		set(${LLVM_LINK_DIR} "${_path}/lib")
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

			#build lto static and dynamic on windows in version 50
			if((${version_} STREQUAL "40") OR (${version_} STREQUAL "50"))
			file(COPY ${BOAAA_SOURCE_DIR}/replace/CMakeLists.txt DESTINATION "${_path}/${branch}/source/tools/lto")
			endif()

			if (${version_} STREQUAL "40") 
			if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			message(STATUS "|| using x64 generator")
			execute_process(
				COMMAND ${CMAKE_COMMAND} "../source" -G "Visual Studio 15 2017" -Thost=x64
				WORKING_DIRECTORY "${_path}/${branch}/build"
				OUTPUT_VARIABLE   o)
			else()
			message(STATUS "|| using x32 generator")
			execute_process(
				COMMAND ${CMAKE_COMMAND} "../source" -G "Visual Studio 15 2019"
				WORKING_DIRECTORY "${_path}/${branch}/build"
				OUTPUT_VARIABLE   o)
			endif()	
			else()
			if(CMAKE_SIZEOF_VOID_P EQUAL 8)
			message(STATUS "|| using x64 generator")
			execute_process(
				COMMAND ${CMAKE_COMMAND} "../source" -G "Visual Studio 16 2019" -Thost=x64
				WORKING_DIRECTORY "${_path}/${branch}/build"
				OUTPUT_VARIABLE   o)
			else()
			message(STATUS "|| using x32 generator")
			execute_process(
				COMMAND ${CMAKE_COMMAND} "../source" -G "Visual Studio 16 2019"
				WORKING_DIRECTORY "${_path}/${branch}/build"
				OUTPUT_VARIABLE   o)
			endif()	
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
		set(${LLVM_LINK_DIR} "${_path}/${branch}/build/Debug/lib")
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

	if(${version_} STREQUAL "40")
	#set(REMOVE_LIST "LLVMFuzzMutate")
	#set(REMOVE_LIST "LLVMFuzzMutate;LLVMLanaiAsmPrinter;LLVMBinaryFormat;LLVMBitstreamReader;LLVMAggressiveInstCombine;LLVMMCA;LLVMRemarks;LLVMDebugInfoGSYM;LLVMJITLink;LLVMARMUtils;LLVMBPFAsmParser;LLVMMSP430AsmParser;LLVMMSP430Disassembler;LLVMRISCVAsmParser;LLVMRISCVDisassembler;LLVMRISCVUtils;LLVMWebAssemblyCodeGen;LLVMWebAssemblyAsmParser;LLVMWebAssemblyDisassembler;LLVMWebAssemblyDesc;LLVMWebAssemblyInfo;LLVMTextAPI;LLVMDlltoolDriver;LLVMWindowsManifest;LLVM-C;Remarks;LLVMRISCVCodeGen;LLVMRISCVDesc;LLVMRISCVInfo")
	foreach(rem ${REMOVE_LIST})
	list(REMOVE_ITEM LLVM_AVAILABLE_LIBS ${rem})
	endforeach()
	endif()

	if(${version_} STREQUAL "50")
	#set(REMOVE_LIST "LLVMFuzzMutate;LLVMBitstreamReader")
	#set(REMOVE_LIST "LLVMFuzzMutate;LLVMLanaiAsmPrinter;LLVMBinaryFormat;LLVMBitstreamReader;LLVMAggressiveInstCombine;LLVMMCA;LLVMRemarks;LLVMDebugInfoGSYM;LLVMJITLink;LLVMARMUtils;LLVMBPFAsmParser;LLVMMSP430AsmParser;LLVMMSP430Disassembler;LLVMRISCVAsmParser;LLVMRISCVDisassembler;LLVMRISCVUtils;LLVMWebAssemblyCodeGen;LLVMWebAssemblyAsmParser;LLVMWebAssemblyDisassembler;LLVMWebAssemblyDesc;LLVMWebAssemblyInfo;LLVMTextAPI;LLVMDlltoolDriver;LLVMWindowsManifest;LLVM-C;Remarks;LLVMRISCVCodeGen;LLVMRISCVDesc;LLVMRISCVInfo")
	foreach(rem ${REMOVE_LIST})
	list(REMOVE_ITEM LLVM_AVAILABLE_LIBS ${rem})
	endforeach()
	endif()

	if(${version_} STREQUAL "90")
	set(REMOVE_LIST "")
	#set(REMOVE_LIST "LLVMFuzzMutate;LLVMLanaiAsmPrinter;LLVMBinaryFormat;LLVMBitstreamReader;LLVMAggressiveInstCombine;LLVMMCA;LLVMRemarks;LLVMDebugInfoGSYM;LLVMJITLink;LLVMARMUtils;LLVMBPFAsmParser;LLVMMSP430AsmParser;LLVMMSP430Disassembler;LLVMRISCVAsmParser;LLVMRISCVDisassembler;LLVMRISCVUtils;LLVMWebAssemblyCodeGen;LLVMWebAssemblyAsmParser;LLVMWebAssemblyDisassembler;LLVMWebAssemblyDesc;LLVMWebAssemblyInfo;LLVMTextAPI;LLVMDlltoolDriver;LLVMWindowsManifest;LLVM-C;Remarks;LLVMRISCVCodeGen;LLVMRISCVDesc;LLVMRISCVInfo")
	foreach(rem ${REMOVE_LIST})
	list(REMOVE_ITEM LLVM_AVAILABLE_LIBS ${rem})
	endforeach()
	endif()

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

	#create full path to libs
	set(LIBS_TMP "")
	foreach(lib ${LLVM_${version_}_LIBS})
	if(${lib} IN_LIST BOAAA_LLVM_DYNLIB_LIST)
	list(APPEND LIBS_TMP "${${LLVM_LINK_DIR}}/${BOAAA_DYNLIB_PREFIX}${lib}${BOAAA_DYNLIB_EXTENSION}")
	else()
	list(APPEND LIBS_TMP "${${LLVM_LINK_DIR}}/${BOAAA_LIB_PREFIX}${lib}${BOAAA_LIB_EXTENSION}")
	endif()
	endforeach(lib)

	set(LLVM_${version_}_LIBS ${LIBS_TMP})

	#return definitions
	set(LLVM_${version_}_DIR "${${LLVM_NAME}_DIR}")
	set(LLVM_${version_}_LIBRARY_DIR "${LLVM_LIBRARY_DIR}")
	set(LLVM_${version_}_VERSION "${LLVM_PACKAGE_VERSION}")
	set(LLVM_${version_}_LIBS "${LLVM_${version_}_LIBS}")
	set(LLVM_${version_}_INCLUDE_DIRS "${LLVM_${version_}_INCLUDE_DIRS}")
	#set(${LLVM_LINK_DIR} ${${LLVM_LINK_DIR}} PARENT_SCOPE)
	set(${LLVM_NAME}_DIR "${${LLVM_NAME}_DIR}")
	set(${LLVM_NAME}_LIBRARY_DIR "${LLVM_LIBRARY_DIR}")
	set(${LLVM_NAME}_VERSION "${LLVM_PACKAGE_VERSION}")
	set(${LLVM_NAME}_LIBS "${LLVM_${version_}_LIBS}")
	set(${LLVM_NAME}_INCLUDE_DIRS "${LLVM_INCLUDE_DIRS}")

	boaaa_add_llvm_version(NAME LLVM_${version_}
					   LIBS ${LLVM_${version_}_LIBS}
					   INTERFACE_INCLUDE_DIRS ${LLVM_${version_}_INCLUDE_DIRS}
					   INTERFACE_LINK_DIRS ${${LLVM_NAME}_LIBRARY_DIR}
					   INTERFACE_COMPILE_OPTIONS -D_DEBUG)

endmacro(boaaa_create_llvm_wrapper)
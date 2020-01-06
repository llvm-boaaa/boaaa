message(STATUS "||=============llvm_versions============||")
message(STATUS "||")

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB DL_files
	"*.h"
	"*.cpp"
	"${BOAAA_SOURCE_DIR}/include/boaaa/*.h"
)

set(tmp_FOLDER_NAME ${FOULDER_NAME})

find_package(Git)
if(NOT GIT_FOUND)
    message(FATAL_ERROR "git not found!")
endif()

#############################################################
#						start foreach						#
#															#

foreach(version_ ${build_llvm_version} )

	message(STATUS "||>>>>>---------release_${version_}---------<<<<<")

	set(branch "release_${version_}")
	set(LLVM_NAME "LLVM_${branch}")
	set(LLVM_NAME_DIR "${LLVM_NAME}_DIR")

	if(${LLVM_NAME}_REGISTERED)
		#>> llvm installed
		set(_path ${${LLVM_NAME}_PATH})
		set(${LLVM_NAME_DIR} "${_path}/cmake/modules/CMakeFiles")
	else()
		#>> llvm need to get build
		set(_path "${BOAAA_SOURCE_DIR}/llvm_version")

		#>> pull or clone git
		if (EXISTS "${_path}/${branch}/source")
			message(STATUS "|| pulling from branch ${branch} ...")
			execute_process(
				COMMAND ${GIT_EXECUTABLE} pull origin ${branch}
				WORKING_DIRECTORY "${_path}/${branch}/source"
				RESULT_VARIABLE   result)
		else()
			message(STATUS "|| clone from branch ${branch} ...")
			execute_process(
				COMMAND ${GIT_EXECUTABLE} clone https://github.com/llvm-mirror/llvm.git --branch ${branch} --single-branch "source"
				WORKING_DIRECTORY "${_path}/${branch}"
				RESULT_VARIABLE   result)
		endif()

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

			boaaa_checkOrCreateFoulder("${_path}/${branch}/build")

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
			execute_process(
				COMMAND ${CMAKE_COMMAND} "../source"
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
			file(READ ${${LLVM_NAME_DIR}}/LLVMConfig.cmake __tmp_content)
			file(WRITE ${${LLVM_NAME_DIR}}/${LLVM_NAME}Config.cmake ${__tmp_content})
			#removed rename, because of lost file LLVMConfig in installed LLVM Versions
			#file(RENAME ${${LLVM_NAME_DIR}}/LLVMConfig.cmake ${${LLVM_NAME_DIR}}/${LLVM_NAME}Config.cmake)
		else()
		message(FATAL_ERROR "ERROR: llvm_version: ${ver} of branch: {branch} is not properly configured")
		endif()
	endif()

	find_package(${LLVM_NAME} CONFIG REQUIRED)

	#wrong configuration of cmake in release_40 and release_50, not needed, so dependencie get removed
	if((${version_} STREQUAL "40") OR (${version_} STREQUAL "50"))
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

	file(GLOB _dl_files
		"${version_}/*.h"
		"${version_}/*.cpp"
	)

	#>> check installed version
	if(${LLVM_NAME}_REGISTERED)
		string(REPLACE "." "" __tmp_version "${LLVM_PACKAGE_VERSION}")
		if(NOT string_starts_with("${__tmp_version}" "${version_}"))
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

	boaaa_add_llvm_library(boaaa.lv_${version_} "boaaa/lv" ${branch} MODULE ${DL_files} ${_dl_files})
	#target_include_directories(boaaa.lv_${version_} PRIVATE ${LLVM_INCLUDE_DIRS})
	target_link_libraries(boaaa.lv_${version_}
							PRIVATE LLVM_${version_} boaaa.support boaaa.vp boaaa.lvm)

	# on windows define LLVM_ON_WIN32 because sometimes undefined
	#if((${version_} STREQUAL "50"))
	#if(WIN32)
	#set(sys_def "LLVM_ON_WIN32=1")
	#endif()
	#endif()

	target_compile_definitions(boaaa.lv_${version_} 
	PRIVATE 
		LLVM_VERSION=${version_}
		LLVM_VERSION_${version_}=${version_} 
		BUILD_SHARED_LIB
		${sys_def})

	#add_custom_target(${LLVM_NAME})

	add_dependencies(boaaa.lv_${version_} LLVM_${version_})
	message(STATUS "||added dynamic lib with llvm_version ${version_}: lv_${version_}")
endforeach(version_)

#															#
#						end foreach							#
#############################################################

set(FOLDER_NAME ${tmp_FOLDER_NAME})



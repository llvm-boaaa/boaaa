message(STATUS "||=============llvm_versions============||")
message(STATUS "||")

#set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

file(GLOB DL_files
	"*.h"
	"*.cpp"
	"${BOAAA_SOURCE_DIR}/include/boaaa/*.h"
	"${BOAAA_SOURCE_DIR}/include/boaaa/lv/*.h"
	"${BOAAA_SOURCE_DIR}/include/boaaa/lv/include_versions/*.inc"
)

set(tmp_FOLDER_NAME ${FOULDER_NAME})

#############################################################
#						start foreach						#
#															#

foreach(version_ ${build_llvm_version})


file(GLOB DL_VERSION_files
	"${BOAAA_SOURCE_DIR}/include/boaaa/lv/${version_}/*.h"
)

	set(branch "release_${version_}")
	set(LLVM_NAME "LLVM_${branch}")
	set(LLVM_LINK_DIR "${LLVM_NAME}_LINK_DIR")
	#set(LLVM_NAME_DIR "${LLVM_NAME}_DIR")

	if(${LLVM_NAME}_REGISTERED)
		#>> llvm installed
		set(REGISTERED TRUE)
	else()
		#>> llvm need to get build
		set(REGISTERED FALSE)
	endif()

	file(GLOB _dl_files
		"${version_}/*.h"
		"${version_}/*.cpp"
	)

	boaaa_add_llvm_library(${REGISTERED} boaaa.lv_${version_} "boaaa/lv" ${branch} MODULE ${DL_files} ${DL_VERSION_files} ${_dl_files})
	#target_include_directories(boaaa.lv_${version_} PRIVATE ${LLVM_INCLUDE_DIRS})
	target_link_libraries(boaaa.lv_${version_}
							PRIVATE "LLVM_${version_}" boaaa.support boaaa.vp ${LLVM_${version_}_ANALYSISES})

	set_property(TARGET boaaa.lv_${version_} PROPERTY CXX_STANDARD 14)

	if(UNIX AND NOT APPLE)
	message(STATUS "||Set propertie of lv_${version_} as PIC")
	set_target_properties(boaaa.lv_${version_} PROPERTIES POSITION_INDEPENDENT_CODE ON)
	endif()
	#target_link_directories(boaaa.lv_${version_} BEFORE INTERFACE ${${LLVM_LINK_DIR}})

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



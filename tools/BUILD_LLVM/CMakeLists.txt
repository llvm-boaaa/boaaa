message(STATUS "||==========BOAAA_LLVM_BUILD============||")
message(STATUS "||")

message(STATUS "|| LLVM_VERSION_TARGETS: ${llvm_dependencies}")

foreach(dep ${llvm_dependencies})
	set(output_file "do_nothing_${dep}.h")
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${output_file}")
	file(REMOVE "${CMAKE_CURRENT_SOURCE_DIR}/${output_file}")
	endif()

	if(WIN32)
	add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${output_file}"
		PRE_BUILD
		COMMAND "start" cmd /c "cmake --build ."
		#COMMAND "start" cmd /c "${CMAKE_COMMAND} --build . --target install"
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/do_nothing.h ${CMAKE_CURRENT_SOURCE_DIR}/${output_file}
		WORKING_DIRECTORY "${BOAAA_SOURCE_DIR}/llvm_version/${dep}/build"
		COMMENT "BUILD LLVM version: ${dep}")
	else()
	add_custom_command(OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/${output_file}"
		PRE_BUILD
		COMMAND ${CMAKE_COMMAND} --build .
		#COMMAND "${CMAKE_COMMAND} --build . --target install"
		COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/do_nothing.h ${CMAKE_CURRENT_SOURCE_DIR}/${output_file}
		WORKING_DIRECTORY "${BOAAA_SOURCE_DIR}/llvm_version/${dep}/build"
		COMMENT "BUILD LLVM version: ${dep}")
	endif()
	LIST(APPEND ALL_DEPS "${CMAKE_CURRENT_SOURCE_DIR}/${output_file}")
endforeach()

message(STATUS "|| deps: ${ALL_DEPS}")

add_executable(BOAAA_LLVM_BUILD "do_nothing.cpp" ${ALL_DEPS})
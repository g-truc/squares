function(glCreateSampleGTC NAME)
	set(SAMPLE_NAME ${GL_PROFILE_GTC}-${GL_VERSION_GTC}-${NAME})
	set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../data)

	foreach(FILE ${GL_SHADER_GTC})
		set(SHADER_PATH ${SHADER_PATH} ${SHADER_DIR}/${FILE})
	endforeach(FILE)

	source_group("Shader Files" FILES ${SHADER_PATH})
	
	add_executable(${SAMPLE_NAME} ${NAME}.cpp ${SHADER_PATH})
	add_test(NAME ${SAMPLE_NAME} COMMAND $<TARGET_FILE:${SAMPLE_NAME}>)

	target_link_libraries(${SAMPLE_NAME} ${FRAMEWORK_NAME} ${BINARY_FILES})
	add_dependencies(${SAMPLE_NAME} glfw ${FRAMEWORK_NAME} ${COPY_BINARY})

	install(TARGETS ${SAMPLE_NAME} DESTINATION .)
endfunction(glCreateSampleGTC)

set(GL_SHADER_GTC texture-float.vert texture-float.frag)
glCreateSampleGTC(squares)


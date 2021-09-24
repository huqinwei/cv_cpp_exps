#
set(TARGET_NAME testCFG)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Include/SampleDLL/	)


set(DEPENDS_LIBS ${CV_CPP_EXPS_BOOST_LIB})
############################### GRAB HEADERS #####################################
FILE(GLOB HEADER_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
									${CMAKE_CURRENT_SOURCE_DIR}/*.h
										
									)
		

########################################### BEGIN: GRAB SOURCE FILES ##########################################
FILE(GLOB SRC_LIST RELATIVE_PATH	"${CMAKE_CURRENT_SOURCE_DIR}"
										"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"																		
								)


set(SRC_FILES ${SRC_LIST} ${HEADER_LIST} )

CV_CPP_EXPS_USING_BOOST()
set(DEPENDS_LIBS "")

add_executable(${TARGET_NAME}  ${SRC_FILES} )

target_link_libraries(${TARGET_NAME}  PUBLIC ${DEPENDS_LIBS}	
				)




set_target_properties(${TARGET_NAME} PROPERTIES  FOLDER "Test")

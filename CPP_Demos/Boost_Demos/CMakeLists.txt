
set(TARGET_NAME Boost_Demos)

############################### GRAB HEADERS #####################################
FILE(GLOB HEADER_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
									${CMAKE_CURRENT_SOURCE_DIR}/*.h
										
									)
		

########################################### BEGIN: GRAB SOURCE FILES ##########################################
FILE(GLOB SRC_LIST RELATIVE_PATH	"${CMAKE_CURRENT_SOURCE_DIR}"
										"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"																		
								)
set(SRC_FILES ${SRC_LIST} ${HEADER_LIST} )


CV_CPP_EXPS_USING_OPENCV()

CV_CPP_EXPS_USING_SIMPLYGON()
CV_CPP_EXPS_USING_OPENMESH()
#CV_CPP_EXPS_USING_OSG_340()
CV_CPP_EXPS_USING_BOOST()
set(CMAKE_CXX_STANDARD 17)




set(DEPENDS_LIBS ${CV_CPP_EXPS_BOOST_LIB} ${CV_CPP_EXPS_OPENMESH_LIB}    )

add_executable(${TARGET_NAME}  ${SRC_FILES} )
target_link_libraries(${TARGET_NAME}  PUBLIC 	${DEPENDS_LIBS}	)
set_target_properties(${TARGET_NAME} PROPERTIES  FOLDER "CPP_Demos")

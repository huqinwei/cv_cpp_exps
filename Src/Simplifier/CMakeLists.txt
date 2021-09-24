#
set(TARGET_NAME SimplifierLib)


############################### GRAB HEADERS #####################################
FILE(GLOB HEADER_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
									${CMAKE_CURRENT_SOURCE_DIR}/*.h
									"${CMAKE_CURRENT_SOURCE_DIR}/../../Include/Simplifier/*.h"	
									"${CMAKE_CURRENT_SOURCE_DIR}/../../Include/MeshBase/*.h"	
									)
		

########################################### BEGIN: GRAB SOURCE FILES ##########################################
FILE(GLOB SRC_LIST RELATIVE_PATH	"${CMAKE_CURRENT_SOURCE_DIR}"
										"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"			
										"MeshBase/*.cpp"																		
								)



ADD_DEFINITIONS("-DCV_CPP_EXPS_MAKE_DLL")
CV_CPP_EXPS_USING_SIMPLYGON()
CV_CPP_EXPS_USING_OPENMESH()
CV_CPP_EXPS_USING_OSG_365()
#CV_CPP_EXPS_USING_OSG_340()
CV_CPP_EXPS_USING_BOOST()
set(CMAKE_CXX_STANDARD 17)

set(SRC_FILES ${SRC_LIST} ${HEADER_LIST} )

set(DEPENDS_LIBS  	${CV_CPP_EXPS_OPENMESH_LIB} 	${CV_CPP_EXPS_OSG_LIB_365}  MeshBaseLib)

#add_executable(${TARGET_NAME}  ${SRC_FILES} )


add_library(${TARGET_NAME} SHARED ${SRC_FILES} ) #STATIC  SHARED
target_link_libraries(${TARGET_NAME} 	${DEPENDS_LIBS}	)
set_target_properties(${TARGET_NAME} PROPERTIES  FOLDER "Libs")

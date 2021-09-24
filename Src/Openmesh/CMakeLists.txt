
set(TARGET_NAME MyMesh) #chy 待构建生成的名字


############################### GRAB HEADERS #####################################
FILE(GLOB HEADER_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
				${CMAKE_CURRENT_SOURCE_DIR}/../../include/OpenMesh/*.h  #chy 头文件的路径
									
									)
		

########################################### BEGIN: GRAB SOURCE FILES ##########################################
FILE(GLOB SRC_LIST RELATIVE_PATH	"${CMAKE_CURRENT_SOURCE_DIR}"
										"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"																		
								)


CV_CPP_EXPS_USING_OPENMESH()  #chy 用到的库

set(SRC_FILES ${SRC_LIST} ${HEADER_LIST} )
set(DEPENDS_LIBS ${CV_CPP_EXPS_OPENMESH_LIB})   #chy 用到的库

add_executable(${TARGET_NAME} ${SRC_FILES} ) 

target_link_libraries(${TARGET_NAME} 	${DEPENDS_LIBS}	)
set_target_properties(${TARGET_NAME} PROPERTIES  FOLDER "Apps")   #chy 设置你希望vs展示的文件抬头


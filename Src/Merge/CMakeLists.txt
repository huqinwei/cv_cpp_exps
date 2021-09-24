
set(TARGET_NAME Merge)#chy 设定目标名



############################### GRAB HEADERS #####################################
FILE(GLOB HEADER_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
									${CMAKE_CURRENT_SOURCE_DIR}/*.h
									"${CMAKE_CURRENT_SOURCE_DIR}/../../include/Merge/*.h"	 #chy 设定头文件路径
									)
		

########################################### BEGIN: GRAB SOURCE FILES ##########################################
FILE(GLOB SRC_LIST RELATIVE_PATH	"${CMAKE_CURRENT_SOURCE_DIR}"
										"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"																		
								)







#chy以下为分组显示
#FILE(GLOB VIEWER_HEADERS RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}"
#								 ${CMAKE_CURRENT_SOURCE_DIR}/../../Include/viewer/*.h
#								)
#FILE(GLOB VIEWER_SRCS RELATIVE	"${CMAKE_CURRENT_SOURCE_DIR}"
#								"${CMAKE_CURRENT_SOURCE_DIR}/../viewer/*.cpp"
#								"${CMAKE_CURRENT_SOURCE_DIR}/../viewer/*.cc"															
#								)
#SOURCE_GROUP(Viewer\\Headeres FILES ${VIEWER_HEADERS})
#SOURCE_GROUP(Viewer\\Sources FILES ${VIEWER_SRCS})
#chy分组显示end


ADD_DEFINITIONS("-DCV_CPP_EXPS_MAKE_DLL") #chy 如果生成动态库则需要
CV_CPP_EXPS_USING_BOOST()  #chy 添加需要的库
CV_CPP_EXPS_USING_OPENMESH() 

set(SRC_FILES ${SRC_LIST} ${HEADER_LIST} )
set(DEPENDS_LIBS ${CV_CPP_EXPS_BOOST_LIB} ${CV_CPP_EXPS_OPENMESH_LIB} MeshBaseLib)    #chy 添加需要的库的名字，统一为${CV_CPP_EXPS_***_LIB},如boost为${CV_CPP_EXPS_BOOST_LIB},但osg例外，因为同一工程涉及到不同版本

#add_library(${TARGET_NAME} SHARED ${SRC_FILES} ) #STATIC  SHARED
add_executable(${TARGET_NAME}  ${SRC_FILES} )

target_link_libraries(${TARGET_NAME} 	${DEPENDS_LIBS}	)

set_target_properties(${TARGET_NAME} PROPERTIES  FOLDER "Apps")
#chy 拷贝dll因每个工程不同需可单独
#if (WIN32)
#
#add_custom_command(TARGET ${TARGET_NAME}
#    POST_BUILD
#    COMMAND SET postfix=\n
#            IF /I $(Configuration)== Debug ( \n
#                SET postfix=-d\n
#            ) \n
#            ${CMAKE_COMMAND} -E copy ${CURL_ROOT_DIR}/dll/$(Platform)/$(Configuration)/libcurl%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#
#)
#
#
#
#add_custom_command(TARGET ${TARGET_NAME}
#    POST_BUILD
#  COMMAND SET postfix=\n
#           IF /I $(Configuration)== Debug ( \n
#               SET postfix=d\n
#           ) \n
#            ${CMAKE_COMMAND} -E copy ${ZLIB_ROOT_DIR}/dll/$(Platform)/$(Configuration)/zlib%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#			${CMAKE_COMMAND} -E copy ${EXIV2_ROOT_DIR}/dll/$(Platform)/$(Configuration)/exiv2%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#			${CMAKE_COMMAND} -E copy ${GLOG_ROOT_DIR}/dll/$(Platform)/$(Configuration)/glog%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#			${CMAKE_COMMAND} -E copy ${EXPAT_ROOT_DIR}/dll/$(Platform)/$(Configuration)/libexpat%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#			#    ${CMAKE_COMMAND} -E copy ${QTDIR}/bin/Qt5Xml%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#			
#)
#
#
#
#add_custom_command(TARGET ${TARGET_NAME}
#    POST_BUILD
#    COMMAND ${CMAKE_COMMAND} -E copy ${EVENT_ROOT_DIR}/dll/$(Platform)/$(Configuration)/event.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#    COMMAND ${CMAKE_COMMAND} -E copy ${EVENT_ROOT_DIR}/dll/$(Platform)/$(Configuration)/event_core.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#    COMMAND ${CMAKE_COMMAND} -E copy ${EVENT_ROOT_DIR}/dll/$(Platform)/$(Configuration)/event_extra.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#    COMMAND ${CMAKE_COMMAND} -E copy ${EVENT_ROOT_DIR}/dll/$(Platform)/$(Configuration)/event_openssl.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#)
#
##qt
#if(USE_QT)
#add_custom_command(TARGET ${TARGET_NAME}
#    POST_BUILD
#    COMMAND SET postfix=\n
#            IF /I $(Configuration)== Debug ( \n
#                SET postfix=d\n
#            ) \n
#            ${CMAKE_COMMAND} -E copy $ENV{QTDIR}/bin/Qt5Core%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#			${CMAKE_COMMAND} -E copy $ENV{QTDIR}/bin/Qt5Gui%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#			${CMAKE_COMMAND} -E copy $ENV{QTDIR}/bin/Qt5OpenGL%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#			${CMAKE_COMMAND} -E copy $ENV{QTDIR}/bin/Qt5Widgets%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#			${CMAKE_COMMAND} -E copy $ENV{QTDIR}/bin/Qt5Xml%postfix%.dll $<TARGET_FILE_DIR:${TARGET_NAME}>\n
#)
#endif(USE_QT)
#
#endif(WIN32)

#set_target_properties(${TARGET_NAME} PROPERTIES LINKER_LANGUAGE CXX)
#target_compile_definitions(${TARGET_NAME} PRIVATE  CV_CPP_EXPS_MAKE_DLL)
#set_target_properties(${TARGET_NAME} PROPERTIES OUTPUT_NAME_DEBUG ${TARGET_NAME}d)
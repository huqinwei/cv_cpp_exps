#
set(TARGET_NAME SimpifyLib)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../Include/SimpifyDLL/	)


############################### GRAB HEADERS #####################################
FILE(GLOB HEADER_LIST RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} 
									${CMAKE_CURRENT_SOURCE_DIR}/*.h
									"${CMAKE_CURRENT_SOURCE_DIR}/../../include/SimpifyDLL/*.h"	
									)
		

########################################### BEGIN: GRAB SOURCE FILES ##########################################
FILE(GLOB SRC_LIST RELATIVE_PATH	"${CMAKE_CURRENT_SOURCE_DIR}"
										"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"																		
								)



ADD_DEFINITIONS("-DCV_CPP_EXPS_MAKE_DLL")
CV_CPP_EXPS_USING_BOOST()

set(SRC_FILES ${SRC_LIST} ${HEADER_LIST} )
set(DEPENDS_LIBS "")
if(USE_QT)
	file( GLOB ui_list RELATIVE
	                             "${CMAKE_CURRENT_SOURCE_DIR}"
									"${CMAKE_CURRENT_SOURCE_DIR}/viewer/*.ui"
									
									)
	file( GLOB qrc_list RELATIVE
	                             "${CMAKE_CURRENT_SOURCE_DIR}"
									"${CMAKE_CURRENT_SOURCE_DIR}/*.qrc"								
									)
	
	
	qt5_wrap_ui( generated_ui_list ${ui_list} )
	qt5_add_resources( generated_qrc_list ${qrc_list} )
	set(DEPENDS_LIBS ${DEPENDS_LIBS} Qt5::Core Qt5::Gui Qt5::Widgets Qt5::OpenGL Qt5::Sql Qt5::Xml)
	set(SRC_FILES ${SRC_FILES} ${generated_ui_list} ${generated_qrc_list} )
endif(USE_QT)

add_library(${TARGET_NAME} SHARED ${SRC_FILES} ) #STATIC  SHARED

target_link_libraries(${TARGET_NAME} 	${DEPENDS_LIBS}	)
set_target_properties(${TARGET_NAME} PROPERTIES  FOLDER "Libs")

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
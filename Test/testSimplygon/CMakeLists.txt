set(TARGET_NAME testSimplygon)

# all files exclude Test* files
file(GLOB_RECURSE SRC_FILES *.cpp *.c *.h)
CV_CPP_EXPS_USING_SIMPLYGON()

set(DEPENDS_LIBS ${CV_CPP_EXPS_SIMPLYGON_LIB})

add_executable(${TARGET_NAME} ${SRC_FILES} )



target_link_libraries(${TARGET_NAME}  ${DEPENDS_LIBS})

set_target_properties(${TARGET_NAME} PROPERTIES  FOLDER "Test")























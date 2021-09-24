if(EXISTS  ${CMAKE_CURRENT_SOURCE_DIR}/testCFG)
	add_subdirectory(testCFG) #chy增加文件夹
endif()
if(EXISTS  ${CMAKE_CURRENT_SOURCE_DIR}/TileNodeSegmentation)
	add_subdirectory(TileNodeSegmentation)
endif()
if(EXISTS  ${CMAKE_CURRENT_SOURCE_DIR}/testManyFun)
	add_subdirectory(testManyFun)
endif()
if(EXISTS  ${CMAKE_CURRENT_SOURCE_DIR}/testSimplygon)
	add_subdirectory(testSimplygon)
endif()






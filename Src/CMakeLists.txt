if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Simplifier)
	add_subdirectory(Simplifier) #chy增加文件夹
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Openmesh)
	add_subdirectory(Openmesh)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/MeshBase)
	add_subdirectory(MeshBase)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/SampleDLL)
	add_subdirectory(SampleDLL)
endif()

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Merge)
	add_subdirectory(Merge)
endif()




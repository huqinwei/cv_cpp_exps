
set(PROJECT_NAME CV_CPP_EXPS) #chy  sln工程名
project(${PROJECT_NAME})


set(CV_CPP_EXPS_DIR           ${PROJECT_SOURCE_DIR}/)
set(CV_CPP_EXPS_TEST_DIR      ${CV_CPP_EXPS_DIR}/Test)
set(CV_CPP_EXPS_CPP_DEMOS_DIR      ${CV_CPP_EXPS_DIR}/CPP_Demos)
set(CV_CPP_EXPS_CV_DEMOS_DIR      ${CV_CPP_EXPS_DIR}/CV_Demos)
set(CV_CPP_EXPS_MODULES_DIR      ${CV_CPP_EXPS_DIR}/Src)
set(CV_CPP_EXPS_THIRDPARTY_DIR ${CV_CPP_EXPS_DIR}/Externals)
set(CV_CPP_EXPS_TEST_TARGETDIR ${PROJECT_NAME})
set(CV_CPP_EXPS_BUILD_DIR  ${CV_CPP_EXPS_DIR}CMakeModules) 

set(CMAKE_MODULE_PATH	${CMAKE_MODULE_PATH}    ${CV_CPP_EXPS_BUILD_DIR}/CMake)

set(USE_QT ON) #chy 如果用到qt则开启，否则设置为OFF
set(USE_OPENGL OFF) #chy 如果用到此库则开启，否则设置为OFF
set(USE_OPENMP OFF) #chy 如果用到此库则开启，否则设置为OFF
set(USE_TBB OFF)    #chy 如果用到此库则开启，否则设置为OFF 

option(USE_CUDA "Whether to USE CUDA, if available" OFF) #chy 如果用到此库则开启，否则设置为OFF 

include(Prequisite)  
include(CMakeExternalLibs)
include(CMakePushCheckState) 
include(CheckCXXCompilerFlag) 

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include	)
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/Test)
 add_subdirectory(Test)  
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CV_Demos)
 add_subdirectory(CV_Demos)  
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/CPP_Demos)
 add_subdirectory(CPP_Demos)  
endif()
add_subdirectory(Src) 


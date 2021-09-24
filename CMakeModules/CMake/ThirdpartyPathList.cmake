#chy 此文件为添加第三方库的全局文件
#DEPS_DIR_VC142是系统变量指定的路径，工作机目前是   D:\alk_workspace\third_party\Windows\vc142
#TODO:
set(TP_ROOT $ENV{DEPS_DIR_VC142})
#

set(COMPILER_USED vc142)
set(CV_CPP_EXPS_ARCHITECTURE ${CV_CPP_EXPS_PLATFORM_NAME})


#chy 宏目前linux只有release版本用于控制 是否只使用release版本
if(UNIX)
	set(CV_CPP_EXPS_USE_DEPS_FAST ON CACHE BOOL "Enable use third libraries's release version .")
endif(UNIX)

if(WIN32)
	set(CV_CPP_EXPS_USE_DEPS_FAST OFF CACHE BOOL "Enable use third libraries's release version .")
endif(WIN32)


#chy 因不同平台的库的后缀名不一样，所以在此设置
set(LIB_POSTFIX "")
set(BIN_POSTFIX "")
if(WIN32)
	set(LIB_POSTFIX .lib)
	set(BIN_POSTFIX .dll)
elseif(UNIX)
	set(LIB_POSTFIX .a)
endif(WIN32)









#chy 以下为各个第三方库的存放路径， 每增加一个库，可往后添加
#本地库
	#ALKCORE
SET(USE_CV_CPP_EXPSCORE ON)	
IF(USE_CV_CPP_EXPSCORE)
	set(CV_CPP_EXPSCORE_VERSION 2.1)
	set(CV_CPP_EXPSCORE_ROOT_DIR     ${CV_CPP_EXPS_THIRDPARTY_DIR}/alkcore/${CV_CPP_EXPSCORE_VERSION})
	set(CV_CPP_EXPS_CORE_INCLUDE_PATH   ${CV_CPP_EXPSCORE_ROOT_DIR}/include)
	if(WIN32)
		
		set(CV_CPP_EXPS_CORE_LIBRARY_PATH   ${CV_CPP_EXPSCORE_ROOT_DIR}/lib/$(Platform))
	
	set(CV_CPP_EXPS_CORE_LIB_DEBUG S3DReconstructiond.lib S3DCommond.lib S3DSfMd.lib S3DSimdd.lib)
	set(CV_CPP_EXPS_CORE_LIB_RELEASE S3DReconstruction.lib S3DCommon.lib S3DSfM.lib S3DSimd.lib)
	
			set(CV_CPP_EXPS_CORE_LIB 
			${CV_CPP_EXPS_CORE_LIB_RELEASE}#S3DReconstruction.lib S3DCommon.lib S3DSfM.lib S3DSimd.lib
				)
				
	
	endif(WIN32)
	function(CV_CPP_EXPS_USING_CORE)
			include_directories( ${CV_CPP_EXPS_CORE_INCLUDE_PATH} )
			link_directories(    ${CV_CPP_EXPS_CORE_LIBRARY_PATH} )
			message("--1--${CV_CPP_EXPS_CORE_LIBRARY_PATH}")
	endfunction()
ENDIF(USE_CV_CPP_EXPSCORE)

	#SIMPLYGON

#SIMPLYGON

SET(USE_SIMPLYGON ON)	
IF(USE_SIMPLYGON)
		set(SIMPLYGON_VERSION 8)
		set(SIMPLYGON_ROOT_DIR     ${CV_CPP_EXPS_THIRDPARTY_DIR}/Simplygon/${SIMPLYGON_VERSION})
		
		set(CV_CPP_EXPS_SIMPLYGON_INCLUDE_PATH   ${SIMPLYGON_ROOT_DIR}/include)
		if(WIN32)
			
			set(CV_CPP_EXPS_SIMPLYGON_LIBRARY_PATH   ${SIMPLYGON_ROOT_DIR}/lib/$(Platform))
		
				set(CV_CPP_EXPS_SIMPLYGON_LIB )
		
		endif(WIN32)
		function(CV_CPP_EXPS_USING_SIMPLYGON)
		
			include_directories( ${CV_CPP_EXPS_SIMPLYGON_INCLUDE_PATH} )
			
			link_directories(    ${CV_CPP_EXPS_SIMPLYGON_LIBRARY_PATH} )
		endfunction()
ENDIF(USE_SIMPLYGON)

#MESH操作相关
     #OPENMESH
SET(USE_OPENMESH ON)
IF(USE_OPENMESH)
	set(OPENMESH_VERSION 8.1)
	set(OPENMESH_ROOT_DIR           ${CV_CPP_EXPS_THIRDPARTY_DIR}/OpenMesh/${OPENMESH_VERSION})
	set(CV_CPP_EXPS_OPENMESH_INCLUDE_PATH   ${OPENMESH_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_OPENMESH_LIBRARY_PATH   ${OPENMESH_ROOT_DIR}/lib/$(Platform))
		set(OPENMESH_FIND_COMPONENTS    Tools  Core		) 
		foreach(COMPONENT ${OPENMESH_FIND_COMPONENTS})
			set(CV_CPP_EXPS_OPENMESH_LIB ${CV_CPP_EXPS_OPENMESH_LIB} 
				debug OpenMesh${COMPONENT}d${LIB_POSTFIX}
				optimized OpenMesh${COMPONENT}${LIB_POSTFIX}
				)
	
		endforeach()

#		set(CV_CPP_EXPS_OPENMESH_LIB
#				debug ${OPENMESH_ROOT_DIR}/lib/$(Platform)/debug/OpenMeshToolsd.lib  ${OPENMESH_ROOT_DIR}/lib/$(Platform)/debug/OpenMeshCored.lib  
#				optimized ${OPENMESH_ROOT_DIR}/lib/$(Platform)/release/OpenMeshTools.lib  ${OPENMESH_ROOT_DIR}/lib/$(Platform)/release/OpenMeshCore.lib
#		)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_OPENMESH)
		include_directories( ${CV_CPP_EXPS_OPENMESH_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_OPENMESH_LIBRARY_PATH} )
	endfunction()
ENDIF(USE_OPENMESH)


#基本库
SET(USE_GLOG ON)
IF(USE_GLOG)
	set(GLOG_VERSION 0.4.0)
	set(GLOG_ROOT_DIR           ${TP_ROOT}/glog/${GLOG_VERSION})
	
	set(CV_CPP_EXPS_GLOG_INCLUDE_PATH   ${GLOG_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_GLOG_LIBRARY_PATH   ${GLOG_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_GLOG_LIB
				debug glogd.lib
				optimized glog.lib )
				
	elseif(UNIX)  #此处未完还未考虑debug版本//根据需求添加
		if(CV_CPP_EXPS_USE_DEPS_FAST)
			set(CV_CPP_EXPS_GLOG_LIBRARY_PATH   ${GLOG_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
			
			set(CV_CPP_EXPS_GLOG_LIB glog
	#${LIB_POSTFIX}
	)
		endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	
	function(CV_CPP_EXPS_USING_GLOG)
		include_directories( ${CV_CPP_EXPS_GLOG_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_GLOG_LIBRARY_PATH} )
		add_definitions(-DGOOGLE_GLOG_DLL_DECL=)
		ADD_DEFINITIONS(-DGLOG_NO_ABBREVIATED_SEVERITIES)
	endfunction()
ENDIF(USE_GLOG)

#GFLAGS
SET(USE_GFLAGS ON)
IF(USE_GFLAGS)

	set(GFLAGS_VERSION 2.2.0)
	set(GFLAGS_ROOT_DIR          ${TP_ROOT}/gflags/${GFLAGS_VERSION})
set(CV_CPP_EXPS_GFLAGS_INCLUDE_PATH   ${GFLAGS_ROOT_DIR}/include)
#${LIB_POSTFIX}
	if(WIN32)
		set(CV_CPP_EXPS_GFLAGS_LIBRARY_PATH   ${GFLAGS_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_GFLAGS_LIB
				debug gflags_static.lib
				optimized gflags_static.lib )
	elseif(UNIX)  #此处未完还未考虑debug版本//根据需求添加
		if(CV_CPP_EXPS_USE_DEPS_FAST)
			set(CV_CPP_EXPS_GFLAGS_LIBRARY_PATH   ${GFLAGS_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
			
			set(CV_CPP_EXPS_GFLAGS_LIB gflags_static${LIB_POSTFIX} ) #_nothreadslibgflags
		
		endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	
	function(CV_CPP_EXPS_USING_GFLAGS)
		include_directories( ${CV_CPP_EXPS_GFLAGS_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_GFLAGS_LIBRARY_PATH} )
		add_definitions(-DCV_CPP_EXPS_GFLAGS_NAMESPACE=${GFLGAS_NAMESPACE})
  add_definitions(-DCV_CPP_EXPS_GFLAGS_NAMESPACE=${GFLAGS_NAMESPACE})
	endfunction()
ENDIF(USE_GFLAGS)
    # BOOST	
SET(USE_BOOST ON)
IF(USE_BOOST)
set(BOOST_VERSION_STRING 1_71)
	set(BOOST_VERSION 1.71.0)
	set(BOOST_ROOT_DIR          ${TP_ROOT}/boost/${BOOST_VERSION})

set(CV_CPP_EXPS_BOOST_INCLUDE_PATH	${BOOST_ROOT_DIR}/include)
    
if(WIN32)
	
  set(BOOST_FIND_COMPONENTS    iostreams  program_options  
				filesystem  system       regex
				) #chrono
		#serialization		
  
	set(CV_CPP_EXPS_BOOST_LIBRARY_PATH   ${BOOST_ROOT_DIR}/lib/$(Platform))
	set(BOOST_USE_STATIC ON)
	set(BOOST_LIB_PREFIX "")
  if(BOOST_USE_STATIC)
  	set(BOOST_LIB_PREFIX lib)
  endif(BOOST_USE_STATIC)
		
	set(_BOOST_COMPILER ${COMPILER_USED})#${MSVC_PLATFORM}
	set(_BOOST_MULTITHREADED "-mt")
	 set(Boost_USE_MULTITHREADED ON)
  if( NOT Boost_USE_MULTITHREADED )
    set (_BOOST_MULTITHREADED "")
  endif()
 
 	set(CV_CPP_EXPS_ARCHITECTURE ${CV_CPP_EXPS_PLATFORM_NAME})#$(Platform) 需测试
  set(_BOOST_DEBUG_TAG "-gd")

	set(CV_CPP_EXPS_BOOST_LIB "")
 	foreach(COMPONENT ${BOOST_FIND_COMPONENTS})
      set(CV_CPP_EXPS_BOOST_LIB ${CV_CPP_EXPS_BOOST_LIB} 
      debug ${BOOST_LIB_PREFIX}boost_${COMPONENT}-${_BOOST_COMPILER}${_BOOST_MULTITHREADED}${_BOOST_DEBUG_TAG}-${CV_CPP_EXPS_PLATFORM_NAME}-${BOOST_VERSION_STRING}${LIB_POSTFIX}
      optimized ${BOOST_LIB_PREFIX}boost_${COMPONENT}-${_BOOST_COMPILER}${_BOOST_MULTITHREADED}-${CV_CPP_EXPS_PLATFORM_NAME}-${BOOST_VERSION_STRING}${LIB_POSTFIX}
      )
	  message("==${BOOST_LIB_PREFIX}boost_${COMPONENT}-${_BOOST_COMPILER}${_BOOST_MULTITHREADED}${_BOOST_DEBUG_TAG}-${BOOST_VERSION_STRING}${LIB_POSTFIX}")
  endforeach()
	
elseif(UNIX)  #此处未完还未考虑debug版本//根据需求添加
	#
	if(CV_CPP_EXPS_USE_DEPS_FAST)
		set(CV_CPP_EXPS_BOOST_LIBRARY_PATH   ${BOOST_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
		set(BOOST_USE_STATIC ON)
	
	set(BOOST_LIB_PREFIX "")
  if(BOOST_USE_STATIC)
  	set(BOOST_LIB_PREFIX lib)
  endif(BOOST_USE_STATIC)
	set(BOOST_FIND_COMPONENTS   iostreams  program_options  
				filesystem  system  serialization     regex
				)

	set(CV_CPP_EXPS_BOOST_LIB "")
 	foreach(COMPONENT ${BOOST_FIND_COMPONENTS})
      set(CV_CPP_EXPS_BOOST_LIB ${CV_CPP_EXPS_BOOST_LIB} 
     	${BOOST_LIB_PREFIX}boost_${COMPONENT}${LIB_POSTFIX}
      )
  endforeach()
	endif(CV_CPP_EXPS_USE_DEPS_FAST)
endif(WIN32)

function(CV_CPP_EXPS_USING_BOOST)
	include_directories( ${CV_CPP_EXPS_BOOST_INCLUDE_PATH} )
	MESSAGE("- CV_CPP_EXPS_BOOST_INCLUDE_PATH- ${CV_CPP_EXPS_BOOST_LIB}---")
	link_directories(    ${CV_CPP_EXPS_BOOST_LIBRARY_PATH} )
	ADD_DEFINITIONS(${Boost_DEFINITIONS} -D_USE_BOOST)
	SET(_USE_BOOST TRUE)

endfunction()
ENDIF(USE_BOOST)
 
#expat
	
	 SET(USE_EXPAT ON) 
	IF(USE_EXPAT)
	set(EXPAT_VERSION 2.4.1)
	set(EXPAT_ROOT_DIR          ${TP_ROOT}/expat/${EXPAT_VERSION})
	set(CV_CPP_EXPS_EXPAT_INCLUDE_PATH   ${EXPAT_ROOT_DIR}/include)
	if(WIN32)
	
		set(CV_CPP_EXPS_EXPAT_LIBRARY_PATH   ${EXPAT_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_EXPAT_LIB
				debug libexpatd.lib    
				optimized libexpat.lib 
	)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_EXPAT)
		include_directories( ${CV_CPP_EXPS_EXPAT_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_EXPAT_LIBRARY_PATH} )		
	endfunction()
	
	ENDIF(USE_EXPAT)



#网络通信相关

	#CURL
SET(USE_CURL ON)
IF(USE_CURL)
	set(CURL_VERSION 7.77.0)
	set(CURL_ROOT_DIR           ${TP_ROOT}/curl/${CURL_VERSION})
	set(CV_CPP_EXPS_CURL_INCLUDE_PATH   ${CURL_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_CURL_LIBRARY_PATH   ${CURL_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_CURL_LIB
				debug libcurl-d_imp.lib    
				optimized libcurl_imp.lib 
		)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_CURL)
		include_directories( ${CV_CPP_EXPS_CURL_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_CURL_LIBRARY_PATH} )
	endfunction()
	ENDIF(USE_CURL)
	
	#EVENT
	SET(USE_EVENT ON)
IF(USE_EVENT)
	set(EVENT_VERSION 2.1.12)
	set(EVENT_ROOT_DIR   ${TP_ROOT}/libevent/${EVENT_VERSION})	
	set(CV_CPP_EXPS_EVENT_INCLUDE_PATH   ${EVENT_ROOT_DIR}/include)
	if(WIN32)
	
		set(CV_CPP_EXPS_EVENT_LIBRARY_PATH   ${EVENT_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_EVENT_LIB event_core.lib  event_extra.lib	event_openssl.lib)
			#  debug event_core.lib  event_extra.lib	event_openssl.lib	
			#  optimized event_core.lib  event_extra.lib	event_openssl.lib)
		
	endif(WIN32)
	function(CV_CPP_EXPS_USING_EVENT)
		include_directories( ${CV_CPP_EXPS_EVENT_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_EVENT_LIBRARY_PATH} )
	endfunction()
ENDIF(USE_EVENT)

	#OPENSSL
	SET(USE_OPENSSL ON)
IF(USE_OPENSSL)
	set(OPENSSL_VERSION 1.1.1k)
	set(OPENSSL_ROOT_DIR   ${TP_ROOT}/openssl/${OPENSSL_VERSION})
	set(CV_CPP_EXPS_OPENSSL_INCLUDE_PATH   ${OPENSSL_ROOT_DIR}/include)
	if(WIN32)
		
		set(CV_CPP_EXPS_OPENSSL_LIBRARY_PATH   ${OPENSSL_ROOT_DIR}/lib/$(Platform))
	
			set(CV_CPP_EXPS_OPENSSL_LIB 
			debug libcrypto.lib  libssl.lib
			optimized libcrypto.lib  libssl.lib)
	
	endif(WIN32)
	function(CV_CPP_EXPS_USING_OPENSSL)
		include_directories( ${CV_CPP_EXPS_OPENSSL_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_OPENSSL_LIBRARY_PATH} )
	endfunction()
ENDIF(USE_OPENSSL)

#显示
    #OSG
SET(USE_OSG ON)
IF(USE_OSG)

	set(OSG_VERSION_STRING 340)
	
	set(OSG_VERSION_340 3.4.0)
	
	set(OSG_ROOT_DIR_340            ${TP_ROOT}/OSG/${OSG_VERSION_340})
	
	# OSG
	set(CV_CPP_EXPS_OSG_INCLUDE_PATH_340	${OSG_ROOT_DIR_340}/include)   
	if(WIN32)

		set(OSG_FIND_COMPONENTS osg	OpenThreads	osgDB  osgUtil osgGA	osgText osgManipulator
	osgParticle osgShadow	osgSim	osgTerrain                   )
	#osgViewer
			
		#serialization	
		

		set(CV_CPP_EXPS_OSG_LIBRARY_PATH_340   ${OSG_ROOT_DIR_340}/lib/$(Platform))

		set(CV_CPP_EXPS_OSG_LIB_340 "")
 		foreach(COMPONENT ${OSG_FIND_COMPONENTS})
			set(CV_CPP_EXPS_OSG_LIB_340 ${CV_CPP_EXPS_OSG_LIB_340} 
			debug ${COMPONENT}d${LIB_POSTFIX}
			optimized ${COMPONENT}${LIB_POSTFIX}
		)
		endforeach()
	
	endif(WIN32)

	function(CV_CPP_EXPS_USING_OSG_${OSG_VERSION_STRING})
	
		include_directories( ${CV_CPP_EXPS_OSG_INCLUDE_PATH_340} )
		link_directories(    ${CV_CPP_EXPS_OSG_LIBRARY_PATH_340} )
	endfunction()

	set(OSG_VERSION_STRING 365)
	set(OSG_VERSION_365 3.6.5_png)
	set(OSG_ROOT_DIR_365            ${TP_ROOT}/OSG/${OSG_VERSION_365})
	# OSG
	set(OSG_ROOT_DIR ${OSG_ROOT_DIR_365})
	set(CV_CPP_EXPS_OSG_INCLUDE_PATH_365	${OSG_ROOT_DIR_365}/include)   
	if(WIN32)
		set(CV_CPP_EXPS_OSG_LIBRARY_PATH_365   ${OSG_ROOT_DIR_365}/lib/$(Platform))#/release

		set(CV_CPP_EXPS_OSG_LIB_365 "")
 	foreach(COMPONENT ${OSG_FIND_COMPONENTS})
		set(CV_CPP_EXPS_OSG_LIB_365 ${CV_CPP_EXPS_OSG_LIB_365} 
		debug ${COMPONENT}d${LIB_POSTFIX}
		optimized ${COMPONENT}${LIB_POSTFIX}
#		MESSAGE("!!!!!!CV_CPP_EXPS_OSG_LIB_365 COMPONENT:debug- ${COMPONENT}d${LIB_POSTFIX}---")
#		MESSAGE("!!!!!!CV_CPP_EXPS_OSG_LIB_365 COMPONENT:optimized- ${COMPONENT}${LIB_POSTFIX}---")
		)
	endforeach()
	
	endif(WIN32)

	function(CV_CPP_EXPS_USING_OSG_365)
		include_directories( ${CV_CPP_EXPS_OSG_INCLUDE_PATH_365} )
		link_directories(    ${CV_CPP_EXPS_OSG_LIBRARY_PATH_365} )
	endfunction()

ENDIF(USE_OSG)
	


	
	


	#GLEW
	SET(USE_GLEW ON)
IF(USE_GLEW)
	set(GLEW_VERSION 2.1.0)
	set(GLEW_ROOT_DIR          ${TP_ROOT}/glew/${GLEW_VERSION})
	set(CV_CPP_EXPS_GLEW_INCLUDE_PATH   ${GLEW_ROOT_DIR}/include)
	if(WIN32)
	
		set(CV_CPP_EXPS_GLEW_LIBRARY_PATH   ${GLEW_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_GLEW_LIB
				debug glew32d${LIB_POSTFIX}    
				optimized glew32${LIB_POSTFIX} 
	)
	elseif(UNIX)  #此处未完还未考虑debug版本//根据需求添加
		
		set(CV_CPP_EXPS_GLEW_LIB GLEW )
		
	#	endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	
	function(CV_CPP_EXPS_USING_GLEW)
		include_directories( ${CV_CPP_EXPS_GLEW_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_GLEW_LIBRARY_PATH} )		
	endfunction()
ENDIF(USE_GLEW)



#数学库
SET(USE_CERES ON)
IF(USE_CERES)
	set(CERES_VERSION 1.14.x)#1.14
	set(CERES_ROOT_DIR          ${TP_ROOT}/ceres/${CERES_VERSION})
	#CERES
	set(CV_CPP_EXPS_CERES_INCLUDE_PATH   ${CERES_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_CERES_LIBRARY_PATH   ${CERES_ROOT_DIR}/lib/$(Platform))
	set(CV_CPP_EXPS_CERES_LIB
				debug ceres-debug.lib     optimized ceres.lib)
	elseif(UNIX)  #此处未完还未考虑debug版本//根据需求添加
		if(CV_CPP_EXPS_USE_DEPS_FAST)
			set(CV_CPP_EXPS_CERES_LIBRARY_PATH   ${CERES_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
			
		set(CV_CPP_EXPS_CERES_LIB libceres${LIB_POSTFIX} )
		
		endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_CERES)
		include_directories( ${CV_CPP_EXPS_CERES_INCLUDE_PATH} )
		
		link_directories(    ${CV_CPP_EXPS_CERES_LIBRARY_PATH} )
	
	endfunction()
ENDIF(USE_CERES)
	
	
	#EIGEN
	SET(USE_EIGEN ON)
IF(USE_EIGEN)
	set(EIGEN_VERSION 3.3.4)
	set(EIGEN_ROOT_DIR          ${TP_ROOT}/Eigen/${EIGEN_VERSION})
	set(CV_CPP_EXPS_EIGEN_INCLUDE_PATH ${EIGEN_ROOT_DIR}/include)
	function(CV_CPP_EXPS_USING_EIGEN)
		include_directories( ${CV_CPP_EXPS_EIGEN_INCLUDE_PATH} )
		ADD_DEFINITIONS(${EIGEN_DEFINITIONS} -D_USE_EIGEN)
		SET(_USE_EIGEN TRUE)
	endfunction()
ENDIF(USE_EIGEN)
#GIS

#geographiclib

SET(USE_GEOGRAPHICLIB ON)
IF(USE_GEOGRAPHICLIB)
	set(GEOGRAPHICLIB_VERSION 1.50.1)#
	set(GEOGRAPHICLIB_ROOT_DIR          ${TP_ROOT}/geographiclib/${GEOGRAPHICLIB_VERSION})
	
	set(CV_CPP_EXPS_GEOGRAPHICLIB_INCLUDE_PATH   ${GEOGRAPHICLIB_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_GEOGRAPHICLIB_LIBRARY_PATH   ${GEOGRAPHICLIB_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_GEOGRAPHICLIB_LIB
				debug Geographic_d.lib   
				optimized Geographic.lib
	)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_GEOGRAPHICLIB)
		include_directories( ${CV_CPP_EXPS_GEOGRAPHICLIB_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_GEOGRAPHICLIB_LIBRARY_PATH} )
			
	endfunction()
ENDIF(USE_GEOGRAPHICLIB)

#gdal
SET(USE_GDAL ON)
IF(USE_GDAL)

 set(GDAL_VERSION 3.3.0)
 set(GDAL_ROOT_DIR          ${TP_ROOT}/gdal/${GDAL_VERSION})


 set(CV_CPP_EXPS_GDAL_INCLUDE_PATH   ${GDAL_ROOT_DIR}/include)
 if(WIN32)
  set(CV_CPP_EXPS_GDAL_LIBRARY_PATH   ${GDAL_ROOT_DIR}/lib/$(Platform))
  set(CV_CPP_EXPS_GDAL_LIB gdal_i.lib 
  )
 
endif(WIN32)
 function(CV_CPP_EXPS_USING_GDAL)
  include_directories( ${CV_CPP_EXPS_GDAL_INCLUDE_PATH} )
  link_directories(    ${CV_CPP_EXPS_GDAL_LIBRARY_PATH} )
  
 endfunction()
ENDIF(USE_GDAL)
#PROJ
SET(USE_PROJ ON)
IF(USE_PROJ)

	set(PROJ_VERSION 6.3.2)
	set(PROJ_ROOT_DIR          ${TP_ROOT}/proj/${PROJ_VERSION})


	set(CV_CPP_EXPS_PROJ_INCLUDE_PATH   ${PROJ_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_PROJ_LIBRARY_PATH   ${PROJ_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_PROJ_LIB
			debug proj_d.lib
			optimized proj.lib 
		)
	elseif(UNIX)
	
		if(CV_CPP_EXPS_USE_DEPS_FAST)
		set(CV_CPP_EXPS_PROJ_LIBRARY_PATH   ${PROJ_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
		
		set(CV_CPP_EXPS_PROJ_LIB libproj${LIB_POSTFIX})	
		endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)

	function(CV_CPP_EXPS_USING_PROJ)
		include_directories( ${CV_CPP_EXPS_PROJ_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_PROJ_LIBRARY_PATH} )
		ADD_DEFINITIONS(-DACCEPT_USE_OF_DEPRECATED_PROJ_API_H)
	endfunction()
ENDIF(USE_PROJ)

#图像处理相关


SET(USE_EXIV2 ON)
IF(USE_EXIV2)
	set(EXIV2_VERSION 0.26.0)
	set(EXIV2_ROOT_DIR          ${TP_ROOT}/exiv2/${EXIV2_VERSION})

	set(CV_CPP_EXPS_EXIV2_INCLUDE_PATH   ${EXIV2_ROOT_DIR}/include)
	if(WIN32)
  
  		set(CV_CPP_EXPS_EXIV2_LIBRARY_PATH   ${EXIV2_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_EXIV2_LIB
			debug exiv2d.lib    
			optimized exiv2.lib 
		)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_EXIV2)
	include_directories( ${CV_CPP_EXPS_EXIV2_INCLUDE_PATH} )
	link_directories(    ${CV_CPP_EXPS_EXIV2_LIBRARY_PATH} )
		
	endfunction()

ENDIF(USE_EXIV2)


#FREEIMAGE
SET(USE_FREEIMAGE ON)
IF(USE_FREEIMAGE)
	set(FREEIMAGE_VERSION 3.18.0)
	set(FREEIMAGE_ROOT_DIR   ${TP_ROOT}/Freeimage/${FREEIMAGE_VERSION})

	set(CV_CPP_EXPS_FREEIMAGE_INCLUDE_PATH   ${FREEIMAGE_ROOT_DIR}/include)
	if(WIN32)
  
  		set(CV_CPP_EXPS_FREEIMAGE_LIBRARY_PATH   ${FREEIMAGE_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_FREEIMAGE_LIB
			debug FreeImaged.lib    
			optimized FreeImage.lib 
		)
	elseif(UNIX)  #此处未完还未考虑debug版本//根据需求添加
		if(CV_CPP_EXPS_USE_DEPS_FAST)
			set(CV_CPP_EXPS_FREEIMAGE_LIBRARY_PATH   ${FREEIMAGE_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
		
			set(CV_CPP_EXPS_FREEIMAGE_LIB libfreeimage${LIB_POSTFIX} )
	
		endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_FREEIMAGE)
		include_directories( ${CV_CPP_EXPS_FREEIMAGE_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_FREEIMAGE_LIBRARY_PATH} )
		
	endfunction()
ENDIF(USE_FREEIMAGE)
	#PNG
	SET(USE_PNG ON)
	IF(USE_PNG)
	SET(PNG_VERSION  1.6.37)
	set(PNG_ROOT_DIR          ${TP_ROOT}/libpng/${PNG_VERSION})
	set(CV_CPP_EXPS_PNG_INCLUDE_PATH   ${PNG_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_PNG_LIBRARY_PATH   ${PNG_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_PNG_LIB	debug  libpng16d.lib optimized  libpng16.lib) #libpng.lib  libpngd.lib
	elseif(UNIX)
		
		if(CV_CPP_EXPS_USE_DEPS_FAST)
			set(CV_CPP_EXPS_PNG_LIBRARY_PATH   ${PNG_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
			
			set(CV_CPP_EXPS_PNG_LIB libpng16${LIB_POSTFIX})	
		endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	
	function(CV_CPP_EXPS_USING_PNG)
		include_directories( ${CV_CPP_EXPS_PNG_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_PNG_LIBRARY_PATH} )
			ADD_DEFINITIONS(${PNG_DEFINITIONS} -D_USE_PNG)
		SET(_USE_PNG TRUE CACHE INTERNAL "")
	endfunction()
	ENDIF(USE_PNG)


	#JPEG	
	SET(USE_JPEG ON)
	IF(USE_JPEG)
	set(JPEG_VERSION 9.0.4) 
	set(JPEG_ROOT_DIR          ${TP_ROOT}/jpeg/${JPEG_VERSION})
	set(CV_CPP_EXPS_JPEG_INCLUDE_PATH   ${JPEG_ROOT_DIR}/include)
	if(WIN32)
	
		set(CV_CPP_EXPS_JPEG_LIBRARY_PATH   ${JPEG_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_JPEG_LIB
				debug jpegd${LIB_POSTFIX}
				optimized jpeg${LIB_POSTFIX} 
	)	
	elseif(UNIX)  #此处未完还未考虑debug版本//根据需求添加
		if(CV_CPP_EXPS_USE_DEPS_FAST)
			set(CV_CPP_EXPS_JPEG_LIBRARY_PATH   ${JPEG_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
			
		set(CV_CPP_EXPS_JPEG_LIB libjpeg${LIB_POSTFIX} )
		
		endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	
	
	function(CV_CPP_EXPS_USING_JPEG)
		include_directories( ${CV_CPP_EXPS_JPEG_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_JPEG_LIBRARY_PATH} )
			ADD_DEFINITIONS(${JPEG_DEFINITIONS} -D_USE_JPG)
			SET(_USE_JPG TRUE CACHE INTERNAL "")
	endfunction()
	ENDIF(USE_JPEG)


	#TIFF
	SET(USE_TIFF ON)

IF(USE_TIFF)
	set(TIFF_VERSION 4.0.9)
	set(TIFF_ROOT_DIR          ${TP_ROOT}/tiff/${TIFF_VERSION})
	
	set(CV_CPP_EXPS_TIFF_INCLUDE_PATH   ${TIFF_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_TIFF_LIBRARY_PATH   ${TIFF_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_TIFF_LIB
				debug tiffd.lib
				optimized tiff.lib 
		)
	
	elseif(UNIX)
	
		if(CV_CPP_EXPS_USE_DEPS_FAST)
			set(CV_CPP_EXPS_TIFF_LIBRARY_PATH   ${TIFF_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
			set(CV_CPP_EXPS_TIFF_LIB
							libtiff${LIB_POSTFIX})
							
		endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	
	function(CV_CPP_EXPS_USING_TIFF)
		include_directories( ${CV_CPP_EXPS_TIFF_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_TIFF_LIBRARY_PATH} )
		ADD_DEFINITIONS(${TIFF_DEFINITIONS} -D_USE_TIFF)
		SET(_USE_TIFF TRUE CACHE INTERNAL "")
	endfunction()
ENDIF(USE_TIFF)
	
#zlib
	SET(USE_ZLIB ON)
IF(USE_ZLIB)
	set(ZLIB_VERSION 1.2.11)
	set(ZLIB_ROOT_DIR          ${TP_ROOT}/zlib/${ZLIB_VERSION})

	set(CV_CPP_EXPS_ZLIB_INCLUDE_PATH   ${ZLIB_ROOT_DIR}/include)
	if(WIN32)
	set(CV_CPP_EXPS_ZLIB_LIBRARY_PATH   ${ZLIB_ROOT_DIR}/lib/$(Platform))
	set(CV_CPP_EXPS_ZLIB_LIB
				debug zlibd.lib          optimized zlib.lib
	#			debug zlibstaticd.lib    optimized zlibstatic.lib 
	)
	elseif(UNIX)  #此处未完还未考虑debug版本//根据需求添加
	
		if(CV_CPP_EXPS_USE_DEPS_FAST)
	
			set(CV_CPP_EXPS_ZLIB_LIBRARY_PATH   ${ZLIB_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
	
			set(CV_CPP_EXPS_ZLIB_LIB libz${LIB_POSTFIX} ) 
		endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_ZLIB)
		include_directories( ${CV_CPP_EXPS_ZLIB_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_ZLIB_LIBRARY_PATH} )
	endfunction()

ENDIF(USE_ZLIB)

#OPENCV
SET(USE_OPENCV ON)
IF(USE_OPENCV)
	set(OPENCV_VERSION 347)
	set(OPENCV_ROOT_DIR   ${TP_ROOT}/opencv/${OPENCV_VERSION})
	set(CV_CPP_EXPS_OPENCV_INCLUDE_PATH	${OPENCV_ROOT_DIR}/include)
	#set(OPENCV_FIND_COMPONENTS  features2d dnn ml  calib3d  stitching video videoio
	#			flann  highgui  imgcodecs     imgproc core objdetect  photo
	#			)#xfeatures2d
	#
	if(WIN32)
	
		set(CV_CPP_EXPS_OPENCV_LIBRARY_PATH   ${OPENCV_ROOT_DIR}/lib/$(Platform))
		 set(CV_CPP_EXPS_OPENCV_LIB debug opencv_world347d${LIB_POSTFIX}   
				optimized opencv_world347${LIB_POSTFIX}) 
	#
	#	set(CV_CPP_EXPS_OPENCV_LIB "")
 	#	foreach(COMPONENT ${OPENCV_FIND_COMPONENTS})
	#	  set(CV_CPP_EXPS_OPENCV_LIB ${CV_CPP_EXPS_OPENCV_LIB} 
	#	  debug opencv_${COMPONENT}${OPENCV_VERSION}d${LIB_POSTFIX}
	#	  optimized opencv_${COMPONENT}${OPENCV_VERSION}${LIB_POSTFIX}
	#	  )
	#	endforeach()
	#		
	#	elseif(UNIX)  #此处未完还未考虑debug版本//根据需求添加
	#
	#	if(CV_CPP_EXPS_USE_DEPS_FAST)
	#	set(CV_CPP_EXPS_OPENCV_LIBRARY_PATH   ${OPENCV_ROOT_DIR}/lib/${CV_CPP_EXPS_PLATFORM_NAME}/Release)
	#	set(OPENCV_USE_STATIC ON)
	#	
	#	
 	#	foreach(COMPONENT ${OPENCV_FIND_COMPONENTS})
	#	  set(CV_CPP_EXPS_OPENCV_LIB ${CV_CPP_EXPS_OPENCV_LIB} 
    # 	opencv_${COMPONENT}
	#	  )
	#	  endforeach()
	#	endif(CV_CPP_EXPS_USE_DEPS_FAST)
	endif(WIN32)
	
	function(CV_CPP_EXPS_USING_OPENCV)
	include_directories( ${CV_CPP_EXPS_OPENCV_INCLUDE_PATH} )
	link_directories(    ${CV_CPP_EXPS_OPENCV_LIBRARY_PATH} )
	ADD_DEFINITIONS(${OpenCV_DEFINITIONS})
	SET(_USE_OPENCV TRUE)
	MESSAGE(STATUS "OpenCV ${OpenCV_VERSION} found (include: ${OpenCV_INCLUDE_DIRS})")
	endfunction()

ENDIF(USE_OPENCV)

SET(USE_LIBRAW ON)
IF(USE_LIBRAW)

 set(LIBRAW_VERSION 0.20.2)
 set(LIBRAW_ROOT_DIR          ${TP_ROOT}/libraw/${LIBRAW_VERSION})


 set(CV_CPP_EXPS_LIBRAW_INCLUDE_PATH   ${LIBRAW_ROOT_DIR}/include)
 if(WIN32)
  set(CV_CPP_EXPS_LIBRAW_LIBRARY_PATH   ${LIBRAW_ROOT_DIR}/lib/$(Platform))
  set(CV_CPP_EXPS_LIBRAW_LIB libraw.lib 
  )
 
endif(WIN32)
 function(CV_CPP_EXPS_USING_LIBRAW)
  include_directories( ${CV_CPP_EXPS_LIBRAW_INCLUDE_PATH} )
  link_directories(    ${CV_CPP_EXPS_LIBRAW_LIBRARY_PATH} )
  
 endfunction()
ENDIF(USE_LIBRAW)


#数据库相关
SET(USE_SQLITE ON)
IF(USE_SQLITE)
	set(SQLITE_VERSION 3.36)
	set(SQLITE_ROOT_DIR          ${TP_ROOT}/sqlite3/${SQLITE_VERSION})
	
	set(CV_CPP_EXPS_SQLITE_INCLUDE_PATH   ${SQLITE_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_SQLITE_LIBRARY_PATH   ${SQLITE_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_SQLITE_LIB
				debug sqlite3.lib   
				optimized sqlite3.lib
	)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_SQLITE)
		include_directories( ${CV_CPP_EXPS_SQLITE_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_SQLITE_LIBRARY_PATH} )
			
	endfunction()
ENDIF(USE_SQLITE)


#点云相关
#点云相关
SET(USE_LIBLAS ON)
IF(USE_LIBLAS)
	set(LIBLAS_VERSION 1.16)#3.0
	set(LIBLAS_ROOT_DIR          ${TP_ROOT}/liblas/${LIBLAS_VERSION})
	
	set(CV_CPP_EXPS_LIBLAS_INCLUDE_PATH   ${LIBLAS_ROOT_DIR}/include)
	if(WIN32)
		set(CV_CPP_EXPS_LIBLAS_LIBRARY_PATH   ${LIBLAS_ROOT_DIR}/lib/$(Platform))
		set(CV_CPP_EXPS_LIBLAS_LIB
				debug liblas_c.lib    liblas.lib
				optimized liblas_c.lib liblas.lib
	)
	endif(WIN32)
	function(CV_CPP_EXPS_USING_LIBLAS)
		include_directories( ${CV_CPP_EXPS_LIBLAS_INCLUDE_PATH} )
		link_directories(    ${CV_CPP_EXPS_LIBLAS_LIBRARY_PATH} )
			
	endfunction()
ENDIF(USE_LIBLAS)

#JSON
SET(USE_RAPIDJSON ON)
IF(USE_RAPIDJSON)
	set(RAPIDJSON_VERSION )
	set(RAPIDJSON_ROOT_DIR   ${TP_ROOT}/rapidjson/${RAPIDJSON_VERSION})


	set(CV_CPP_EXPS_RAPIDJSON_INCLUDE_PATH ${RAPIDJSON_ROOT_DIR}/include)
	function(CV_CPP_EXPS_USING_RAPIDJSON)
		include_directories( ${CV_CPP_EXPS_RAPIDJSON_INCLUDE_PATH} )
	endfunction()
ENDIF(USE_RAPIDJSON)




















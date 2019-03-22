#
# 1394020320@qq.com
#

cmake_minimum_required(VERSION 3.4.3)
project(SweepingEdge)
file (TO_CMAKE_PATH "$ENV{SOUIPATH}" SOUI_ROOT)

message(STATUS "SOUI_ROOT: ${SOUI_ROOT}")
if ("${SOUI_ROOT}" STREQUAL "")
	message(FATAL_ERROR "FATAL:can't find env variable SOUIPATH, clone soui core and install wizard first, please.")
endif()

include(${SOUI_ROOT}/__cmake/cotire.cmake)
include(${SOUI_ROOT}/__cmake/internal_utils.cmake)
include(${SOUI_ROOT}/__cmake/global.cmake)

readSettingFile(UNICODE "1" USE_UNICODE)
readSettingFile(WCHAR "1" WCHAR_AS_DEFAULT)
readSettingFile(MT "0" SHARED_CRT)
readSettingFile(SUPPORT_XP "1" XP_TOOLSET)

#message(STATUS "USE_UNICODE: ${USE_UNICODE}")
#message(STATUS "WCHAR_AS_DEFAULT: ${WCHAR_AS_DEFAULT}")
#message(STATUS "SHARED_CRT: ${SHARED_CRT}")
#message(STATUS "XP_TOOLSET: ${XP_TOOLSET}")

config_compiler_and_linker()

include_directories(${SOUI_ROOT}/config)
include_directories(${SOUI_ROOT}/components)
include_directories(${SOUI_ROOT}/utilities/include)
include_directories(${SOUI_ROOT}/SOUI/include)
include_directories(${SOUI_ROOT}/controls.extend)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mblib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)

file(GLOB_RECURSE CURRENT_HEADERS  *.h *.hpp)
file(GLOB_RECURSE CURRENT_SRCS  *.cpp)
file(GLOB_RECURSE CURRENT_SOUIS  *.xml *.idx *.ico)
file(GLOB_RECURSE CURRENT_RC *.rc *.rc2)

source_group("Header Files" FILES ${CURRENT_HEADERS})
source_group("Source Files" FILES ${CURRENT_SRCS})
source_group("SoUI Resouece" FILES ${CURRENT_SOUIS})
source_group("Resource Files" FILES ${CURRENT_RC})

link_directories(${SOUI_ROOT}/bin)
add_executable(SweepingEdge WIN32 ${CURRENT_HEADERS} ${CURRENT_SRCS} ${CURRENT_SOUIS} ${CURRENT_RC})

#add_dependencies(SweepingEdge soui-sys-resource)
#target_link_libraries(SweepingEdge utilities soui ${COM_LIBS})
target_link_libraries(SweepingEdge
        debug souid optimized soui
        debug utilitiesd optimized utilities
)

set_target_properties(SweepingEdge PROPERTIES COTIRE_CXX_PREFIX_HEADER_INIT "stdafx.h")
set_target_properties(SweepingEdge PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

set_target_properties (SweepingEdge PROPERTIES
    FOLDER demos
)
cotire(SweepingEdge)

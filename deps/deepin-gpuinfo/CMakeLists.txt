
cmake_minimum_required(VERSION 3.0)
PROJECT(deepin-gpuinfo)
# LANGUAGES C　)
#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")  #调试用

set(gpuinfoVERSION_MAJOR  0)
set(gpuinfoVERSION_MINOR  1)
set(gpuinfoVERSION_PATCH  1)

set(CMAKE_CXX_STANDARD 14)  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
# ------------------------------------
 
#///////////////////////////////////////////////////////////////////#
#                        detect the plat arch                   #
#///////////////////////////////////////////////////////////////////#

message("-- CMAKE_HOST_SYSTEM_PROCESSOR: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

if( ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "x86_64" )
    set(CMAKE_SYSTEM_PROCESSOR "x86_64")
elseif( ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "aarch64" )
    set(CMAKE_SYSTEM_PROCESSOR "__aarch64__")
elseif( ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "mips64" )
    set(CMAKE_SYSTEM_PROCESSOR "__mips64__")
elseif( ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "sw_64" )
    set(CMAKE_SYSTEM_PROCESSOR "__sw_64__")
elseif( ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "i686" )
    set(CMAKE_SYSTEM_PROCESSOR "__i686__")   
elseif( ${CMAKE_HOST_SYSTEM_PROCESSOR} STREQUAL "loongarch64" )
    set(CMAKE_SYSTEM_PROCESSOR "__loongarch64__")    
endif()
# 上面mips64和i686　/usr/lib..库路径需要按上面作特殊处理， 如果上面的探测失败，　将会用下面的平台路径进行下去．
set(INSTALL_LIB_PATH   /usr/lib/${CMAKE_HOST_SYSTEM_PROCESSOR}-linux-gnu)

message("-- INSTALL_LIB_PATH: ${INSTALL_LIB_PATH}")




option(HAVE_GPU_INTEL   "HAVE_GPU_INTEL"    on)
option(HAVE_GPU_AMD     "HAVE_GPU_AMD"      on)
option(HAVE_GPU_JJW     "HAVE_GPU_JJW"      on)
option(HAVE_LIBVA       "HAVE_LIBVA"        on)
option(HAVE_LIBVDPAU    "HAVE_LIBVDPAU"     on)
option(HAVE_GPU_NVIDIA   "HAVE_GPU_NVIDIA"  on)

# 加入一个配置头文件，用于处理 CMake 对源码的设置
configure_file (
  "${PROJECT_SOURCE_DIR}/config/config.h.in"
  "${PROJECT_BINARY_DIR}/config/config.h"
  )
  configure_file(
  "${PROJECT_SOURCE_DIR}/config/version.h.in"
  "${PROJECT_BINARY_DIR}/config/version.h"  
  IMMEDIATE @ONLY)

#///////////////////////////////////////////////////////////////////#
#                             INSTALL                               #
#///////////////////////////////////////////////////////////////////#
INSTALL(FILES README.md DESTINATION share/deepin-gpuinfo)

#///////////////////////////////////////////////////////////////////#
#                        COMPILATION OPTIONS                        #
#///////////////////////////////////////////////////////////////////#

 
ADD_SUBDIRECTORY(src)







cmake_minimum_required(VERSION 3.0)
set(CMD_NAME deepin-gpuinfo)
#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g")  #调试用
#set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -g") 
#set(CMAKE_BUILD_TYPE Release)  ＃调试这句有的话需要注释掉

INCLUDE_DIRECTORIES(/usr/include/)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/config) 
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gpu_nvidia)   #./gpu_nvidia/nvml.h
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/src/gpu_glxinfo)  # glxinfo detection module



# aux_source_directory(./queryinfo  QUERYINFO_DIR_SRCS)
aux_source_directory(./libgpuinfo  LIBGPUINFO_DIR_SRCS)

if(HAVE_LIBVDPAU)
SET(VDPAU_DIR_SRCS    ./vdpauinfo/vdpauinfo.c                      
    )   
endif()

 SET(LSPCI_DIR_SRCS 
                    ./lspci/sysfs_pcidev.h
                    ./lspci/sysfs_pcidev.c
                    ./common/main.c
                    ./queryinfo/gpulib.cc
                    ./queryinfo/osutils.cc
                    ./queryinfo/osutils.h
                    ./queryinfo/pci.h
                    # ./lspci/config_lspci.h 
                    # ./lspci/header.h 
                    # ./lspci/internal.h 
                    # ./lspci/pci.h 
                    # ./lspci/pread.h
                    # ./lspci/sysdep.h
                    # ./lspci/sysfs.h
                    # ./lspci/types.h                    
                    # ./lspci/access.c
                    # ./lspci/caps.c
                    # ./lspci/generic.c
                    # ./lspci/init.c
                    # ./lspci/params.c
                    # ./lspci/sysfs.c   
                    # ./lspci/names-parse.c
                    # ./lspci/names-hash.c
                    # ./lspci/names-cache.c
                    # ./lspci/names.h
                    # ./lspci/filter.c 
                    
    )

if(HAVE_LIBVA)
SET(VAINFO_DIR_SRCS 
               ./vainfo/va_display.c
               ./vainfo/va_display_drm.c
               ./vainfo/va_display_x11.c
               ./vainfo/va_display_wayland.c
               ./vainfo/vainfo.c
    )
endif()

if(HAVE_GPU_AMD)
    aux_source_directory(./gpu_amd  GPU_AMD_DIR_SRCS)
endif()

if(HAVE_GPU_NVIDIA)
    SET(GPU_NVIDIA_DIR_SRCS    ./gpu_nvidia/nvtop.c
                                    )
endif()

if(HAVE_GPU_JJW)
SET(GPU_JJW_DIR_SRCS
               ./gpu_jjw/jm_top.c
    )
endif()

SET(GLXINFO_DIR_SRCS
               ./gpu_glxinfo/glxinfo_detect.c
    )


find_package(PkgConfig REQUIRED)

# find_package(sqlite REQUIRED)
#pkg_check_modules(SQLITE REQUIRED IMPORTED_TARGET sqlite3)
pkg_check_modules(PCIe REQUIRED IMPORTED_TARGET pciaccess)
# # PCI_USE_LIBKMOD
# pkg_check_modules(LIBKMOD_LIBS  REQUIRED IMPORTED_TARGET   libkmod)  
# pkg_check_modules(LIBPCI_LIBS  REQUIRED IMPORTED_TARGET   libpci)

if(HAVE_LIBVA)
pkg_check_modules(LIBVA_DRM  REQUIRED IMPORTED_TARGET  libva-drm)
pkg_check_modules(LIBVA  REQUIRED IMPORTED_TARGET  libva)
pkg_check_modules(LIBX11  REQUIRED IMPORTED_TARGET  x11)
pkg_check_modules(LIBVA_DRM  REQUIRED IMPORTED_TARGET  libva-drm)
pkg_check_modules(LIBVA_X11  REQUIRED IMPORTED_TARGET  libva-x11)
pkg_check_modules(LIBVA_WAYLAND  REQUIRED IMPORTED_TARGET  libva-wayland)
endif()

if(HAVE_LIBVDPAU)
pkg_check_modules(LIBVDPAU  REQUIRED IMPORTED_TARGET  vdpau)
endif()

if(HAVE_GPU_AMD)
pkg_check_modules(DRM  REQUIRED IMPORTED_TARGET libdrm)
set(GPU_AMD_LIBS  PkgConfig::DRM
                   PkgConfig::PCIe            
                  )
endif()

if(HAVE_LIBVA)
set(LIBVA_LIBS      PkgConfig::LIBVA_DRM         
                    PkgConfig::LIBVA
                    PkgConfig::LIBX11
                    PkgConfig::LIBVA_X11
                    PkgConfig::LIBVA_WAYLAND
 #                   PkgConfig::SQLITE
                    -ldl               
                    resolv
        )
endif()

if(HAVE_LIBVDPAU)
set(LIBVDPAU_LIBS  PkgConfig::LIBVDPAU )
endif()
   
ADD_EXECUTABLE(${CMD_NAME}  ${LSPCI_DIR_SRCS}
                            ${GPU_NVIDIA_DIR_SRCS}
                            ${GPU_AMD_DIR_SRCS}
                            ${GPU_JJW_DIR_SRCS}
                            ${GLXINFO_DIR_SRCS}
                            ${VDPAU_DIR_SRCS}
                            ${VAINFO_DIR_SRCS}
                            ${LIBGPUINFO_DIR_SRCS}
                            # ${QUERYINFO_DIR_SRCS}
             )

#将第三方库链接进来(注意 *_LIBRARIES)

set(EXECUTABLE_OUTPUT_PATH  ${PROJECT_SOURCE_DIR}/build)

if(HAVE_GPU_NVIDIA) 
target_link_libraries(${CMD_NAME} ${GPU_AMD_LIBS} ${GPU_JJW_LIBS} ${GPU_INTEL_LIBS} 
                ${LIBVA_LIBS} ${LIBVDPAU_LIBS})
endif()

 
#///////////////////////////////////////////////////////////////////#
#                             INSTALL                               #
#///////////////////////////////////////////////////////////////////#
# SET(CMAKE_INSTALL_PREFIX ./install) 
# SET(CMAKE_INSTALL_PREFIX / usr)  
install(TARGETS ${CMD_NAME} DESTINATION bin)
INSTALL(FILES ../config/gpuinfolib.csv DESTINATION share/deepin-gpuinfo) 



add_subdirectory(libgpuinfo)





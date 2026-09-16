project(libgpuinfo VERSION 0.1.0)

# set(CMAKE_AUTOMOC ON)
set(CMD_NAME gpuinfo)

#set(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -g  -fpic ") 
#set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -g  -fpic  ")  #调试用  -lm -ldl 

add_definitions(-D_LIBGPUINFO_)

pkg_check_modules(PCIe REQUIRED IMPORTED_TARGET pciaccess)
pkg_check_modules(LIBVA_DRM  REQUIRED IMPORTED_TARGET  libva-drm)
pkg_check_modules(LIBVA  REQUIRED IMPORTED_TARGET  libva)
pkg_check_modules(LIBX11  REQUIRED IMPORTED_TARGET  x11)
pkg_check_modules(LIBVA_DRM  REQUIRED IMPORTED_TARGET  libva-drm)
pkg_check_modules(LIBVA_X11  REQUIRED IMPORTED_TARGET  libva-x11)
pkg_check_modules(LIBVA_WAYLAND  REQUIRED IMPORTED_TARGET  libva-wayland)
pkg_check_modules(LIBVDPAU  REQUIRED IMPORTED_TARGET  vdpau)
pkg_check_modules(DRM  REQUIRED IMPORTED_TARGET libdrm)

set(GPU_AMD_LIBS  
# PkgConfig::DRM
                #    PkgConfig::PCIe            
                  )
set(LIBVA_LIBS      
PkgConfig::LIBVA_DRM         
                #   PkgConfig::LIBVA
                  PkgConfig::LIBX11
                  PkgConfig::LIBVA_X11
                  PkgConfig::LIBVA_WAYLAND
                #   resolv
      )
set(LIBVDPAU_LIBS  PkgConfig::LIBVDPAU )


include_directories(${CMAKE_INCLUDE_CURRENT_DIR})

# file(GLOB_RECURSE SRCS LIST_DIRECTORIES false *.c)
# file(GLOB_RECURSE VA_SRCS LIST_DIRECTORIES false   /src/*.cpp)
# list(APPEND SRCS play.cpp ${VA_SRCS})
SET(LIBGPUINFO_SRCS    ./pci_wrapper.c
                        ./vdpau_wrapper.c
                        ./va_wrapper.c
                        ./csv_parse.c
                        ../lspci/sysfs_pcidev.c
                        ../gpu_glxinfo/glxinfo_detect.c

                        ../lspci/sysfs_pcidev.h
                        ../lspci/pciaccess_private.h
                        ../vainfo/va_display_drm.c
                        ../vainfo/va_display_wayland.c
                        ../vainfo/va_display_x11.c
                        ../vainfo/va_display.c
                        ../vainfo/va_display.h
                        ../gpu_glxinfo/glxinfo_detect.h
            )

add_library(${CMD_NAME} SHARED ${LIBGPUINFO_SRCS})
set_target_properties(${CMD_NAME} PROPERTIES VERSION 0.1.0 SOVERSION 0.1)

target_include_directories(${CMD_NAME} PUBLIC
    ${PROJECT_SOURCE_DIR}/src/libgpuinfo    )

target_link_libraries(${CMD_NAME}  ${LIBVA_LIBS} ${LIBVDPAU_LIBS}  ${GPU_AMD_LIBS})    

include(GNUInstallDirs)

configure_file(libgpuinfo.pc.in ${PROJECT_BINARY_DIR}/libgpuinfo.pc @ONLY)

install(TARGETS ${CMD_NAME} DESTINATION ${CMAKE_INSTALL_LIBDIR})
# install(FILES    libgpuinfo_detect.h    DESTINATION include/libgpuinfo)
install(FILES    libgpuinfo_decoder.h   DESTINATION include/libgpuinfo)

install(FILES ${PROJECT_BINARY_DIR}/libgpuinfo.pc DESTINATION 
        ${CMAKE_INSTALL_LIBDIR}/pkgconfig)

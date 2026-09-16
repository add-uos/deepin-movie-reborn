# deepin-gpuinfo

## deepin-gpuinfo

This package contains the deepin-gpuinfo  tools, version 0.1.0.

Copyright (c) 2021--2040 shuaijie <shuaijie@uniontech.com>

All files in this package can be freely distributed and used according
to the terms of the GNU General Public License, either version 2 or
(at your opinion) any newer version. See http://www.gnu.org/ for details.


## 1. What's that?

The tools utilities include:  

  - detect pci: displays detailed information about GPU PCI buses and devices and name classes.

  - vainfo: allows to show the VAAPI Entry.

  - vdpauinfo:  allows to show the vdpauinfo Entry.


## 2. Compiling and (un)installing

Just run  "mkdir build" , "cd build"  ,"cmake .." , "make" ,to compile the package and then "make install" to install it.                         or   cmake .. -DCMAKE_INSTALL_PREFIX=./install.
Please note that GNU make and cmake is needed on most platforms.

## 3. get start to use

open the terminal shell , please run the command.

$: deepin-gpuinfo -h
$: deepin-gpuinfo 
$: deepin-gpuinfo -u
$: deepin-gpuinfo -a
$: deepin-gpuinfo -l
$: sudo deepin-gpuinfo -t


 atfer 2 secondds  your can see some information about the  gpu , show int the screen.

$: deepin-gpuinfo -a
 If the command outputs an error, you can try to install drivers via command:

- For NVIDIA nouveau and AMD, run command:
sudo apt install mesa-va-drivers
- For Gen 8+ Intel hardware, install via command:
sudo apt install intel-media-va-driver
And for old Intel hardware, install i965-va-driver instead via apt install command.

By setting the environment variable, you can choose which driver to use. For example, set “export LIBVA_DRIVER_NAME=i965” to use i965 driver. The value can also set to “nouveau” for NVIDIA, or “radeonsi” for AMDGPU.


$: deepin-gpuinfo -u
配置
环境变量 VDPAU_DRIVER 决定使用的驱动。可以全局设置 或 针对一个用户 设置 环境变量

正确的驱动程序名称取决于您的设置：

Intel 使用 va_gl.
$: VDPAU_DRIVER=va_gl ./deepin-gpuinfo -u
开源AMD/ATI驱动程序,您需要将它设置为适当的驱动程序版本取决于你的GPU。
Nvidia的专有版本将变量设置为“nvidia”。
$ grep -i vdpau /var/log/Xorg.0.log
(II) RADEON(0): [DRI2] VDPAU driver: r300
然后设置 VDPAU 驱动：

VDPAU_DRIVER=r300


多显卡显示
对混合显卡(NVIDIA和AMD)，需要设置下面环境变量。

$ export DRI_PRIME=1

## 4. Getting new versions

to do.......


## 5. Using the library

So far, there is only a little documentation for the library .
to do .......


## 6. Feedback

If you have any bug reports or suggestions, send them to the author.

​                                                                                                 Have fun
​                                                                                                               shuaijie@uniontech.com


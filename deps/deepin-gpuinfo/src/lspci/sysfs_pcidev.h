/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  sysfs_pcidev.h
 * Author:     shuaijie <shuaijie@uniontech.com> 
 * Maintainer: shuaijie <shuaijie@uniontech.com>
 * descrition:  This file is part of deepin-gpuinfo. 
 *
 * This program is free software: you can redistribute it and/or modify,it underthe terms  of the GNU General Public 
 *  License as published by the Free Software Foundation, either version 3 of the License, or any later version. 
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even  the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.You should have
 * received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
//sysfs_pcidev.h
#ifndef _SYSFS_PCIDEV_H
#define _SYSFS_PCIDEV_H

#ifdef __cplusplus
extern "C" {
#endif

#include "pciaccess.h"
#define MAX_NUM_MEDIA_DEVICE 4  //目前只考虑设备上的显卡不会超过4个


/**
 * Base type for tracking PCI subsystem information.
 */
struct pci_dev_driver
{

  /** Array of known devices.
     */
  //struct pci_device_private *devices;
  
  struct pci_device base;
  int  driverName_lenth;
  char drivername[256];   //kernel frivername
};


int    sysfs_populate_entries(void);         //下面的函数执行须先执行sysfs_populate_entries（）建立 device information

// void   sysfs_MediaDevice_print(void);       //打印
struct  pci_dev_driver sysfs_read_MediaDevInfo(int8_t  index);  //读取单个 (index须小于MAX_NUM_MEDIA_DEVICE)
int8_t sysfs_readMediaDevice_number(void);  //返回值为 拥有多媒体设备个数
 int sysfs_pci_device_linux_read( struct pci_device * dev, void * data,
			     pciaddr_t offset, pciaddr_t size,
			     pciaddr_t * bytes_read );

int sysfs_pci_device_linux_probe( struct pci_device * dev );

#ifdef __cplusplus
}
#endif

#endif

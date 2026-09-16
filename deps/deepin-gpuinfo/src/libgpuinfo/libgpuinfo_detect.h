/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  libgpuinfo_detect.h
 * Author:     shuaijie <shuaijie@uniontech.com> 
 * Maintainer: shuaijie <shuaijie@uniontech.com>
 * descrition:  This file is part of libgpuinfo. 
 *
 * This program is free software: you can redistribute it and/or modify,it underthe terms  of the GNU General Public 
 *  License as published by the Free Software Foundation, either version 3 of the License, or any later version. 
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even  the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.You should have
 * received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _LIBGPUINFO_DETECT_H
#define _LIBGPUINFO_DETECT_H
#include "../vdpauinfo/vdpauinfo.h"


struct _pci_st
{
 unsigned int  idclass;
 char  pcidvendordev[16];
 char  driver[100];
 char  classname[100];  
 char  description[1000];
} ;


//PCI:
unsigned int GetCurrentMeddiaCad(struct _pci_st *stp);
void gpulib_search_description(char* ss);  //10de:4411 　格式　：　VENDOR: DEVICE , 小写



#endif  //ENDOF   #define _LIBGPUINFO_DETECT_H
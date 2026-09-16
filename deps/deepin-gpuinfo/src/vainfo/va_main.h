/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  va_main.h
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

#ifndef VA_MAIN_H
#define VA_MAIN_H


#include <stdio.h>
#include "../lspci/sysfs_pcidev.h"

#ifdef __cplusplus
extern "C" {
#endif

//int va_main(int argc,  char* argv[]);
//int main_gputop(void) ;
int va_main(void);
int lspci_top_main(struct  pci_dev_driver* );
void vdpauinfo_main(void);
int jmgputop(void);  // for jjw gpu
void nvtop_man(void) ;

int va_getMaxNumProfile(void);
int vainfo_getNameProfile(char * va_support_profile);

#ifdef __cplusplus
}
#endif

#endif /* VA_MAIN_H */

/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  jm_top.c
 * Author:     shuaijie <shuaijie@uniontech.com>
 *
 * Maintainer: shuaijie <shuaijie@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>

 int jmgputop(void)  // for jjw gpu
 {
	 int fd;
	int read_ret;
	struct stat st;

	printf("----UOS JJW GPU Top START---- \n");

	fd = open("/proc/gpuinfo_0", O_RDONLY);	
	if (fd == -1) {
        return errno;
    }
	if (fstat(fd, &st) == -1)
	{
		printf("message open JJW card /proc/gpuinfo_0 fail : \n");
		//close(fp);
	} else {
		char *buf = (char *)malloc(100);
		printf("\t-----------------------\n");
		while (read(fd, buf, 100 ) != 0)
			printf("\t%s", buf);
		free(buf);
	}	
	close(fd);
	return 0;
}
/*
----UOS JJW GPU Top START---- 
Jiffies                 : 4295051756
Vendor                  : Changsha JingJia Microelectronics Co.D
Vendor ID               : 0731
Device ID               : 7200
Subsystemid             : 7201
logy              : 28 nm
Bus Type                : PCIE2.0 x8
Firmware Version        : 3.0.0
 Fillrate          : 1200 Mpixel/s
Texture Fillrate        : 2400 Mpixel/s
 DDR3y Type             :D
Memory Size             : 1024 MB
Memory Frequence        : 400 MHz
359 MB ddr0 Remain Size : D
Memory ddr1 Remain Size : 511 MB
Main Frequence          : 300 MHz
47.99 Degreeperature    : D
GPU Use Rate(2d)        : 1.60%
GPU Use Rate(3d)        : 0.00%

*/

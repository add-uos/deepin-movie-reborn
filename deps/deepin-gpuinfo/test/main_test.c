/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  main_test.c
 * Author:     shuaijie <shuaijie@uniontech.com> 
 * Maintainer: shuaijie <shuaijie@uniontech.com>
 * descrition:  This file is part of test libgpuinfo. 
 *
 * This program is free software: you can redistribute it and/or modify,it underthe terms  of the GNU General Public 
 *  License as published by the Free Software Foundation, either version 3 of the License, or any later version. 
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even  the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.You should have
 * received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// #include "./usr/include/libgpuinfo/libgpuinfo_decoder.h"
 #include "../src/libgpuinfo/libgpuinfo_decoder.h"
//  gcc main_test.c -g -o test -ldl 
#include <getopt.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <dlfcn.h>
#include <limits.h>
#include <time.h>

#include "../build/config/version.h"

#include <dirent.h>

static int countOpenFDs(void)
{
    DIR *dir = opendir("/proc/self/fd");
    int count = 0;

    if (!dir) {
        fprintf(stderr, "Couldn't open /proc/self/fd; skipping file descriptor "
                "leak test\n");
        return 0;
    }

    while (readdir(dir) != NULL) {
        count++;
    }

    closedir(dir);
    return count;
}

// gcc main_test.c -g -o test -ldl

// gcc -shared -fpic -lm -ldl -o libtest.so pci_wrapper.c  vdpau_wrapper.c

#define LUSR_LIB_PATH  "../build/src/libgpuinfo"

void main(void)
{
  struct   timeval   tpstart,tpend;
  float   timeuse;

   char path[256];
  snprintf( path, 255, "%s/libgpuinfo.so",  LUSR_LIB_PATH );
//printf("path:%s\n",path);
  int nOpenFDs = countOpenFDs();

 gettimeofday(&tpstart,NULL);
 void *driver = NULL;
 driver = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);

  if( NULL == driver)
  {
    printf("open( %s )fail\n ",path);
   //  dlclose(driver);
    snprintf( path, 255, "%s/libgpuinfo.so",  USR_LIB_PATH );
    driver = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);
    if( NULL == driver)
      goto fail;
  }
  else{
    printf("open( %s ) sucess\n ",path);
  }

  unsigned int (*vdp_Iter_decoderInfo)(decoder_profile , VDP_Decoder_t *) =NULL;
  vdp_Iter_decoderInfo =  dlsym(driver, "vdp_Iter_decoderInfo");


  VDP_Decoder_t head ;
  int i;
    i = vdp_Iter_decoderInfo( H264, &head );
  printf("--2-test: %d, %s",i, head.ret_info);
  

  i = vdp_Iter_decoderInfo( MPEG2, &head );
  printf("--3-test: %d, %s",i, head.ret_info);

  i = vdp_Iter_decoderInfo( VC1, &head );
  printf("--2-test: %d, %s",i, head.ret_info);

  gettimeofday(&tpend,NULL);

  dlclose(driver);

 timeuse=1000000*(tpend.tv_sec-tpstart.tv_sec)+tpend.tv_usec-tpstart.tv_usec;
        timeuse/=1000;
        printf( "Used   Time:%f ms\n ",timeuse);


    // Make sure no file descriptors were leaked.
    if (countOpenFDs() != nOpenFDs) {
        fprintf(stderr, "Mismatch in the number %d of open file descriptors!\n", countOpenFDs()-nOpenFDs);
        return ;
    }
    else
	 fprintf(stderr, "ok match in the number of open file descriptors!\n");

fail:
 return ;
}



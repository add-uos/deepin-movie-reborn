/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  main.c
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

#include "config.h"
#include "version.h"

#include "../vainfo/va_main.h"
#include <unistd.h>
#include <stdio.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>
#include <dlfcn.h>
#include <dirent.h>
 #include "../libgpuinfo/libgpuinfo_decoder.h"



#ifndef NONLS
#include <locale.h>
#endif

#include "pciaccess.h"
#include "../queryinfo/gpulib.h"
#include "../queryinfo/pci.h"

void dec_test(void);

 void sysfs_MediaDevice_print(void)
{
  int i;
  struct pci_dev_driver tmp;
  struct pci_dev_driver *device = &tmp;

  printf("Class_ID, VID:PID,  PCI,       kerneldriver,        vendor_name,          device_name  \n");
  printf("-------------------------------------------------------------------------------------------------------- \n");

  for(i=0;i<sysfs_readMediaDevice_number();i++)    {
      // device = &sysfs_mediaDevice_info[i];
      *device = sysfs_read_MediaDevInfo(i);
      printf("0x%04x, %04x:%04x, ", device->base.device_class>>8, device->base.vendor_id,  device->base.device_id  );   
      printf("@%04x:%02x:%02x.%1u, ",device->base.domain, device->base.bus, device->base.dev, device->base.func );
      printf("\t%s, ", device->drivername);
               
      printf("  \t%s,  \t%s; \n", pci_device_get_vendor_name(&device->base) ,
                                     pci_device_get_device_name(&device->base)
                                     );
      //  printf("\t----/sub vendor_name: %s" " / " "%s \n", pci_device_get_subvendor_name(&device->base) ,
      //                                pci_device_get_subdevice_name(&device->base)
      //                                );
    }
  printf("-------------------------------------------------------------------------------------------------------- \n");
  return;
}


// #define PCI_VENDOR_ID_ZX        0x1d17   //"zx", zx_chip_ids
// #define PCI_VENDOR_ID_LONGSOON  0x0014

void gpu_top(void)
{

struct  pci_dev_driver pci_info;
int8_t number_device;

number_device = sysfs_readMediaDevice_number();  
printf("number_device:%d \n",number_device);

  for (unsigned int i = 0; i < number_device; i++)  {
    pci_info = sysfs_read_MediaDevInfo(i); 
   
    if (PCI_BASE_CLASS_DISPLAY ==  (pci_info.base.device_class >>16 ))  
    {
      if (PCI_VENDOR_ID_AMD == pci_info.base.vendor_id) {  // printf("GPU AMD \n");       
         lspci_top_main(&pci_info);         
      }
       if (PCI_VENDOR_ID_JJW == pci_info.base.vendor_id) {  	//景嘉微	//景嘉微//printf("GPU jjw \n");        
        jmgputop();
       }
      if (PCI_VENDOR_ID_NVIDIA == pci_info.base.vendor_id) {        
#ifdef HAVE_GPU_NVIDIA
        nvtop_man();
        printf("GPU nvidia \n");       
#endif
      }
      if (PCI_VENDOR_ID_INTEL == pci_info.base.vendor_id) {
        printf("GPU intel to do  \n");       
#ifdef HAVE_GPU_INTEL
#endif
      }
    }//end of if (PCI_CLASS_DISPLAY_VGA  ...)
  }
 //  printf("GPU top over \n");
}


static const char versionString[] = "version " GPUINFO_VERSION_STRING;

void us_age(const char *progname)
{
  fprintf(stderr, "deepin: %s-%s\n", progname, versionString);
  fprintf(stderr, _("usage: %s [-format] [-options ...]\n"), progname);
  fprintf(stderr, "\n");
  fprintf(stderr, _("\t-h --help         : Print help and exit\n"));
  fprintf(stderr, _("\t-v --version      : Print the version and exit\n"));
  fprintf(stderr, _("\nformat can be\n"));
  fprintf(stderr, _("\t-u --vdpau        : Show  encode/decode vdpauinfo\n"));
  fprintf(stderr, _("\t-a --va           : Show  encode/decode  the vainfo\n"));
  // fprintf(stderr, _("\t-d --dec          : Show  encode/decode  auto form lib so\n"));
  fprintf(stderr, _("\t-t --top          : Show  GPU top state output hardware information\n"));
  fprintf(stderr, _("\t-l --list         : list support GPU libary data information\n"));
  fprintf(stderr, _("\t-q --query-gpu    : query-gpu  support information from library. please input the ID vendor:device; for JingJiaWei example: -q 0731:7200\n"));
  fprintf(stderr, "\n");
}

static const struct option long_opts[] = {
    {.name = "vdpau",   .has_arg = no_argument, .flag = NULL, .val = 'u'},
    {.name = "va",      .has_arg = no_argument, .flag = NULL, .val = 'a'},
    // {.name = "dec",     .has_arg = no_argument, .flag = NULL, .val = 'd'},
    {.name = "top",     .has_arg = no_argument, .flag = NULL, .val = 't'},
    {.name = "version", .has_arg = no_argument, .flag = NULL, .val = 'v'},
    {.name = "help",    .has_arg = no_argument, .flag = NULL, .val = 'h'},
    {.name = "list",    .has_arg = no_argument, .flag = NULL, .val = 'l'},
    {.name = "query",   .has_arg = required_argument, .flag = NULL, .val = 'q'},
    {0, 0, 0, 0},
};
static const char opts[] = "uadthvlq:";

int main(int argc, char **argv)
{
  char *msg;
  //char ptmp[128] ="";
  char *ptmp = NULL;
  int8_t  _vdpau ,_va, _dec, _top, _listlib,_qlib;  

  // (void)setlocale(LC_CTYPE, "");
   _vdpau = _va = _dec = _top  = _listlib  =_qlib  = 0; 

  sysfs_populate_entries();
  
  while (1)  {
    int optchar = getopt_long(argc, argv, opts, long_opts, NULL);
    if (optchar == -1)
      break;
    switch (optchar)    {
    case 'u': _vdpau = 1;
      break;
    case 'a': _va = 1;
      break;
    case 'd': _dec = 1;
      break;  
    case 't': _top = 1;
      break;
    case 'l': _listlib = 1;
      break;
    case 'q': 
        if (optarg){
          ptmp = optarg;
          _qlib =1;
        }
        else {          
          exit(EXIT_FAILURE);
        }        
      break;
    case 'v':
      printf("%s-%s\n", argv[0], versionString);
      exit(EXIT_SUCCESS);    
    case 'h':
      us_age(argv[0]);
      exit(EXIT_SUCCESS);  
    default:
      fprintf(stderr, "Unhandled error in getopt missing argument\n");
      us_age(argv[0]);
      exit(EXIT_FAILURE);
      break;
    }

  } //end of  while (true)
  sysfs_MediaDevice_print();  
 
  if(_vdpau){
     vdpauinfo_main(); 
  }
  if(_va){
     va_main();
  }

  if(_dec){
     dec_test();
 }

  if(_top){
    gpu_top();
  }
  if(_listlib){
    gpulib_allshow();
  }
  if(_qlib){
      //  if (optarg)
      //   ptmp = optarg;
        printf("--query-gpu  ---by uos---%s  \n", ptmp);
        if(NULL != strchr(ptmp,':'))
          gpulib_search_description(ptmp);  
        else  
          QueryCurrentGraphicsInfo(); 
  }   
   
  exit(EXIT_SUCCESS);
}

//------------for test libgpuinfo.so--------
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

 void dec_test(void)
{
  struct   timeval   tpstart, tpend;
  float   timeuse;

   char path[256];
  snprintf( path, 255, "%s/libgpuinfo.so", USR_LIB_PATH );
printf("path:%s\n",path);
  int nOpenFDs = countOpenFDs();

 gettimeofday(&tpstart,NULL);
 void *driver = NULL;
 driver = dlopen(path, RTLD_LAZY | RTLD_GLOBAL);

  if( NULL == driver)
  {
    printf("open( %s )fail\n ",path);
   //  dlclose(driver);
    goto fail;
  }
  else{
    ;//printf("open( %s ) sucess\n ",path);
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
/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  nvtop.c
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

#include <getopt.h>
//#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <locale.h>

//#include "nvtop/interface.h"
//#include "nvtop/time.h"
// #include "nvtop/version.h"
#include "extract_gpuinfo.h"
#include <dlfcn.h>
#include <limits.h>

#include "version.h"

void PRINTF_NULL(char * format, ...){;}
#define _debug_print   PRINTF_NULL 
//#define _debug_print   printf


void clean_device_info(unsigned int num_devs, struct device_info *dev_info) {
  for (unsigned int i = 0; i < num_devs; ++i) {
    free(dev_info[i].graphic_procs);
    free(dev_info[i].compute_procs);
    free(dev_info[i].process_infos);
  }
  free(dev_info);
  //clean_pid_cache();
}



void nvtop_man(void) 
{
  char name[256];
   snprintf( name, 205, "%s/libnvidia-ml.so", USR_LIB_PATH );

   if((access(name,F_OK))==-1) 
   {
     printf("no  %s  exist \n",name);  
     return;
   }

  nvmlReturn_t retval = NVML_SUCCESS;
    unsigned int num_devices;
    struct device_info tmp;
  struct device_info *dev_infos = &tmp;

  setenv("ESCDELAY", "10", 1);
    // if (!init_gpu_info_extraction())
    // return EXIT_FAILURE;
//------------------------
 void *driver = NULL;
 
  driver = dlopen(name, RTLD_LAZY | RTLD_GLOBAL);    // driver = dlopen("/usr/lib/x86_64-linux-gnu/libnvidia-ml.so.1", RTLD_LAZY | RTLD_GLOBAL); 
  if( NULL == driver)
  {
    printf("open %s  fail\n",name);     
    return;
  }
  else 
      printf("open %s ok\n",name);
// dlclose(driver);   nvmlReturn_t;

// nvmlReturn_t DECLDIR nvmlInit(void);
  nvmlReturn_t  (* nvmlInit)(void) = NULL;
  nvmlInit = dlsym(driver, "nvmlInit");

// const DECLDIR char* nvmlErrorString(nvmlReturn_t result);
  char*(* nvmlErrorString)(nvmlReturn_t ) = NULL;
  nvmlErrorString =  dlsym(driver, "nvmlErrorString");

// nvmlReturn_t DECLDIR nvmlDeviceGetHandleByIndex(unsigned int index, nvmlDevice_t *device);
  nvmlReturn_t (* nvmlDeviceGetHandleByIndex)(unsigned int, nvmlDevice_t *) = NULL;
  nvmlDeviceGetHandleByIndex =  dlsym(driver, "nvmlDeviceGetHandleByIndex");

// nvmlReturn_t DECLDIR nvmlDeviceGetName(nvmlDevice_t device, char *name, unsigned int length);
  nvmlReturn_t (* nvmlDeviceGetName)(nvmlDevice_t, char*, unsigned int ) = NULL;
  nvmlDeviceGetName =  dlsym(driver, "nvmlDeviceGetName");
  
  // nvmlReturn_t DECLDIR nvmlDeviceGetClockInfo(nvmlDevice_t device, nvmlClockType_t type, unsigned int *clock);
  nvmlReturn_t  (* nvmlDeviceGetClockInfo)(nvmlDevice_t,nvmlClockType_t, unsigned int *) = NULL;
  nvmlDeviceGetClockInfo =  dlsym(driver, "nvmlDeviceGetClockInfo");

// nvmlReturn_t DECLDIR nvmlDeviceGetMaxClockInfo(nvmlDevice_t device, nvmlClockType_t type, unsigned int *clock);
  nvmlReturn_t  (* nvmlDeviceGetMaxClockInfo)(nvmlDevice_t,nvmlClockType_t , unsigned int * ) = NULL;
  nvmlDeviceGetMaxClockInfo =  dlsym(driver, "nvmlDeviceGetMaxClockInfo");

  // nvmlReturn_t DECLDIR nvmlDeviceGetUtilizationRates(nvmlDevice_t device, nvmlUtilization_t *utilization);
  nvmlReturn_t  (* nvmlDeviceGetUtilizationRates)(nvmlDevice_t , nvmlUtilization_t *) = NULL;
  nvmlDeviceGetUtilizationRates =  dlsym(driver, "nvmlDeviceGetUtilizationRates");

  // nvmlReturn_t DECLDIR nvmlDeviceGetMemoryInfo(nvmlDevice_t device, nvmlMemory_t *memory);
  nvmlReturn_t  (* nvmlDeviceGetMemoryInfo)(nvmlDevice_t ,nvmlMemory_t *) = NULL;
  nvmlDeviceGetMemoryInfo =  dlsym(driver, "nvmlDeviceGetMemoryInfo");
  
  // nvmlReturn_t DECLDIR nvmlDeviceGetCurrPcieLinkGeneration(nvmlDevice_t device, unsigned int *currLinkGen);
  nvmlReturn_t  (* nvmlDeviceGetCurrPcieLinkGeneration)(nvmlDevice_t,unsigned int *) = NULL;
  nvmlDeviceGetCurrPcieLinkGeneration = dlsym(driver, "nvmlDeviceGetCurrPcieLinkGeneration");

// nvmlReturn_t DECLDIR nvmlDeviceGetCurrPcieLinkWidth(nvmlDevice_t device, unsigned int *currLinkWidth);
  nvmlReturn_t  (* nvmlDeviceGetCurrPcieLinkWidth)(nvmlDevice_t, unsigned int *) = NULL;
  nvmlDeviceGetCurrPcieLinkWidth = dlsym(driver, "nvmlDeviceGetCurrPcieLinkWidth");

  // nvmlReturn_t DECLDIR nvmlDeviceGetPcieThroughput(nvmlDevice_t device, nvmlPcieUtilCounter_t counter, unsigned int *value);
  nvmlReturn_t  (* nvmlDeviceGetPcieThroughput)(nvmlDevice_t, nvmlPcieUtilCounter_t, unsigned int *) = NULL;
  nvmlDeviceGetPcieThroughput = dlsym(driver, "nvmlDeviceGetPcieThroughput");

  // nvmlReturn_t DECLDIR nvmlDeviceGetFanSpeed(nvmlDevice_t device, unsigned int *speed);
  nvmlReturn_t  (* nvmlDeviceGetFanSpeed)(nvmlDevice_t,unsigned int *) = NULL;
  nvmlDeviceGetFanSpeed = dlsym(driver, "nvmlDeviceGetFanSpeed");

  // nvmlReturn_t DECLDIR nvmlDeviceGetTemperature(nvmlDevice_t device, nvmlTemperatureSensors_t sensorType, unsigned int *temp);
  nvmlReturn_t  (* nvmlDeviceGetTemperature)(nvmlDevice_t , nvmlTemperatureSensors_t , unsigned int *) = NULL;
  nvmlDeviceGetTemperature = dlsym(driver, "nvmlDeviceGetTemperature");

  // nvmlReturn_t DECLDIR nvmlDeviceGetPowerUsage(nvmlDevice_t device, unsigned int *power);
  nvmlReturn_t  (* nvmlDeviceGetPowerUsage)(nvmlDevice_t , unsigned int *) = NULL;
  nvmlDeviceGetPowerUsage = dlsym(driver, "nvmlDeviceGetPowerUsage");

// nvmlReturn_t DECLDIR nvmlDeviceGetEnforcedPowerLimit(nvmlDevice_t device, unsigned int *limit);
  nvmlReturn_t  (* nvmlDeviceGetEnforcedPowerLimit)(nvmlDevice_t , unsigned int *) = NULL;
  nvmlDeviceGetEnforcedPowerLimit = dlsym(driver, "nvmlDeviceGetEnforcedPowerLimit");

// nvmlReturn_t DECLDIR nvmlDeviceGetEncoderUtilization(nvmlDevice_t device, unsigned int *utilization, unsigned int *samplingPeriodUs);
  nvmlReturn_t  (* nvmlDeviceGetEncoderUtilization)(nvmlDevice_t , unsigned int *, unsigned int *) = NULL;
  nvmlDeviceGetEncoderUtilization = dlsym(driver, "nvmlDeviceGetEncoderUtilization");

  // nvmlReturn_t DECLDIR nvmlDeviceGetDecoderUtilization(nvmlDevice_t device, unsigned int *utilization, unsigned int *samplingPeriodUs);
  nvmlReturn_t  (* nvmlDeviceGetDecoderUtilization)(nvmlDevice_t , unsigned int *, unsigned int *) = NULL;
  nvmlDeviceGetDecoderUtilization = dlsym(driver, "nvmlDeviceGetDecoderUtilization");

  // nvmlReturn_t DECLDIR nvmlDeviceGetCount(unsigned int *deviceCount);
  nvmlReturn_t  (* nvmlDeviceGetCount)(unsigned int *) = NULL;
  nvmlDeviceGetCount = dlsym(driver, "nvmlDeviceGetCount");

  // nvmlReturn_t DECLDIR nvmlShutdown(void);
  nvmlReturn_t  (* nvmlShutdown)(void) = NULL;
  nvmlShutdown = dlsym(driver, "nvmlShutdown");
//---------------------------
  retval = nvmlInit();
  if (retval != NVML_SUCCESS) {
    fprintf(stderr, "Impossible to initialize nvidia nvml : %s\n",
            nvmlErrorString(retval));
    goto _fail; //return false;
  }

  retval = nvmlDeviceGetCount(&num_devices);
  if (retval != NVML_SUCCESS) {
    fprintf(stderr, "Impossible to get the number of devices : %s\n", nvmlErrorString(retval));
    goto _fail; //return 0;
  }
  if (num_devices == 0) {
    fprintf(stdout, "No GPU left to monitor.\n");
  
    goto _success; //return EXIT_SUCCESS;
  }
  else printf("gpu_numbers:%d\n",num_devices);

  //dev_infos = (struct device_info *)malloc(num_devices * sizeof(struct device_info *));

  retval = nvmlDeviceGetHandleByIndex(0, &dev_infos->device_handle);
  if (retval != NVML_SUCCESS) {
    printf("-debugfail; nvmlDeviceGetHandleByIndex\n");

  }
    char gpu_name[NVML_DEVICE_NAME_BUFFER_SIZE];
    char  *pgpu_name = gpu_name;
    unsigned int length = NVML_DEVICE_NAME_BUFFER_SIZE;
    retval = nvmlDeviceGetName(dev_infos->device_handle, \
                                   pgpu_name, length);
    if (retval != NVML_SUCCESS) {
      printf("-debugfail; nvmlDeviceGetName\n");
    }else {
       printf(":: gpu_name:%s\n",pgpu_name);
    }

    // GPU CLK
     retval = nvmlDeviceGetClockInfo(dev_infos->device_handle, \
                                      NVML_CLOCK_GRAPHICS,   &dev_infos->gpu_clock_speed);
    SET_VALID(gpu_clock_speed_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->gpu_clock_speed = 0;
      RESET_VALID(gpu_clock_speed_valid, dev_infos->valid);
     _debug_print("_nvmlfail  gpu_clock_speed \n");  //--
    } else {
      printf(":: gpu_clock_speed:%d\n",dev_infos->gpu_clock_speed);
    }

    // MEM CLK
    retval = nvmlDeviceGetClockInfo(dev_infos->device_handle, \
                                NVML_CLOCK_MEM,   &dev_infos->mem_clock_speed);
    SET_VALID(mem_clock_speed_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->mem_clock_speed = 0;
      RESET_VALID(mem_clock_speed_valid, dev_infos->valid);
     _debug_print("_nvmlfail  mem_clock_speed \n");  //--
    } else {
      printf(":: mem_clock_speed:%d\n",dev_infos->mem_clock_speed);
    }

    // GPU CLK MAX
    retval = nvmlDeviceGetMaxClockInfo(dev_infos->device_handle,\
                                       NVML_CLOCK_GRAPHICS,  &dev_infos->gpu_clock_speed_max);
    SET_VALID(gpu_clock_speed_max_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->gpu_clock_speed_max = 0;
      RESET_VALID(gpu_clock_speed_max_valid, dev_infos->valid);
     _debug_print("_nvmlfail   gpu_clock_speed_max \n");  //--
    } else {
      printf(":: gpu_clock_speed_max:%d\n",dev_infos->gpu_clock_speed_max);
    }

    // MEM CLK MAX
    retval = nvmlDeviceGetMaxClockInfo(dev_infos->device_handle, \
                                        NVML_CLOCK_MEM,  &dev_infos->mem_clock_speed_max);
    SET_VALID(mem_clock_speed_max_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->mem_clock_speed_max = 0;
      RESET_VALID(mem_clock_speed_max_valid, dev_infos->valid);
     _debug_print("_nvmlfail  mem_clock_speed_max \n");  //--
    } else {
      printf(":: mem_clock_speed_max:%d\n",dev_infos->mem_clock_speed_max);
    }

    // GPU / MEM UTIL RATE
    nvmlUtilization_t util_rate;
    retval = nvmlDeviceGetUtilizationRates(dev_infos->device_handle,    &util_rate);
    if (retval != NVML_SUCCESS) {
      dev_infos->gpu_util_rate = 0;
      dev_infos->mem_util_rate = 0;
      RESET_VALID(gpu_util_rate_valid, dev_infos->valid);
      RESET_VALID(mem_util_rate_valid, dev_infos->valid);
      // _debug_print("_nvmlfail   GPU / MEM UTIL RATE \n");  //--
    } else {
      dev_infos->gpu_util_rate = util_rate.gpu;
      dev_infos->mem_util_rate = util_rate.memory;
      SET_VALID(gpu_util_rate_valid, dev_infos->valid);
      SET_VALID(mem_util_rate_valid, dev_infos->valid);
      printf(":: gpu_util_rate:%d \n",dev_infos->gpu_util_rate );
      printf(":: mem_util_rate:%d \n",dev_infos->mem_util_rate );
    }

    // FREE / TOTAL / USED MEMORY
    nvmlMemory_t meminfo;
    retval = nvmlDeviceGetMemoryInfo(dev_infos->device_handle,    &meminfo);
    if (retval != NVML_SUCCESS) {
      dev_infos->free_memory = 0;
      dev_infos->total_memory = 0;
      dev_infos->used_memory = 0;
      RESET_VALID(free_memory_valid, dev_infos->valid);
      RESET_VALID(total_memory_valid, dev_infos->valid);
      RESET_VALID(used_memory_valid, dev_infos->valid);
     _debug_print("_nvmlfail  GetMemoryInfo \n");  
    } else {
      dev_infos->free_memory = meminfo.free;
      dev_infos->total_memory = meminfo.total;
      dev_infos->used_memory = meminfo.used;
      SET_VALID(free_memory_valid, dev_infos->valid);
      SET_VALID(total_memory_valid, dev_infos->valid);
      SET_VALID(used_memory_valid, dev_infos->valid);
      printf(":: free_memory:%d (m)\n",dev_infos->free_memory / (1024)/ (1024));
      printf(":: used_memory:%d (m)\n",dev_infos->used_memory / (1024)/ (1024));
      printf(":: total_memory:%d (m)\n",dev_infos->total_memory / (1024)/ (1024));
    }

    // PCIe LINK GEN
    retval = nvmlDeviceGetCurrPcieLinkGeneration(dev_infos->device_handle, \
                                                    &dev_infos->cur_pcie_link_gen);
    SET_VALID(cur_pcie_link_gen_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->cur_pcie_link_gen = 0;
      RESET_VALID(cur_pcie_link_gen_valid, dev_infos->valid);
     _debug_print("_nvmlfail  cur_pcie_link_gen \n"); 
    } else {
      printf(":: cur_pcie_link_gen:%d\n",dev_infos->cur_pcie_link_gen);
    }

    // PCIe LINK WIDTH
    retval = nvmlDeviceGetCurrPcieLinkWidth(dev_infos->device_handle, \
                                              &dev_infos->cur_pcie_link_width);
    SET_VALID(cur_pcie_link_width_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->cur_pcie_link_width = 0;
      RESET_VALID(cur_pcie_link_width_valid, dev_infos->valid);
           _debug_print("_nvmlfail   cur_pcie_link_width \n");
    } else {
      printf(":: cur_pcie_link_width:%d\n",dev_infos->cur_pcie_link_width);
    }

    // PCIe TX THROUGHPUT
    retval = nvmlDeviceGetPcieThroughput(dev_infos->device_handle,\
                                         NVML_PCIE_UTIL_TX_BYTES,    &dev_infos->pcie_tx);
    SET_VALID(pcie_tx_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->pcie_tx = 0;
      RESET_VALID(pcie_tx_valid, dev_infos->valid);
     _debug_print("_nvmlfail  pcie_tx \n");  //--
    } else {
      printf(":: pcie_tx:%d\n",dev_infos->pcie_tx);
    }

    // PCIe RX THROUGHPUT
    retval = nvmlDeviceGetPcieThroughput(dev_infos->device_handle,\
                                         NVML_PCIE_UTIL_RX_BYTES,   &dev_infos->pcie_rx);
    SET_VALID(pcie_rx_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->pcie_rx = 0;
      RESET_VALID(pcie_rx_valid, dev_infos->valid);
      _debug_print("_nvmlfail  pcie_rx \n");  //--
    } else {
      printf(":: pcie_rx:%d\n",dev_infos->pcie_rx);
    }

    // FAN SPEED
    retval = nvmlDeviceGetFanSpeed(dev_infos->device_handle,\
                                    &dev_infos->fan_speed);
    SET_VALID(fan_speed_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->fan_speed = 0;
      RESET_VALID(fan_speed_valid, dev_infos->valid);
      _debug_print("_nvmlfail  fan_speed \n");
    } else {
      printf(":: fan_speed:%d\n",dev_infos->fan_speed);
    }

    // GPU TEMP
    retval = nvmlDeviceGetTemperature(dev_infos->device_handle,\
                                      NVML_TEMPERATURE_GPU, &dev_infos->gpu_temp);
    SET_VALID(gpu_temp_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->gpu_temp = 0;
      RESET_VALID(gpu_temp_valid, dev_infos->valid);
      _debug_print("_nvmlfail  gpu_temp \n");
 } else {
      printf(":: gpu_temp:%d\n",dev_infos->gpu_temp);
    }
    // POWER DRAW
    retval = nvmlDeviceGetPowerUsage(dev_infos->device_handle,\
                                     &dev_infos->power_draw);
    SET_VALID(power_draw_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->power_draw = 0;
      RESET_VALID(power_draw_valid, dev_infos->valid);
      
 } else {
      printf(":: power_draw:%d\n",dev_infos->power_draw);
    }

    // POWER MAX
    retval = nvmlDeviceGetEnforcedPowerLimit(dev_infos->device_handle,\
                                             &dev_infos->power_draw_max);
    SET_VALID(power_draw_max_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->power_draw_max = 0;
      RESET_VALID(power_draw_max_valid, dev_infos->valid);
    } else {
      printf(":: power_draw_max:%d\n",dev_infos->power_draw_max);
    }

    // Encoder infos
    retval = nvmlDeviceGetEncoderUtilization(dev_infos->device_handle,\
                                             &dev_infos->encoder_rate,   &dev_infos->encoder_sampling);
    SET_VALID(encoder_rate_valid, dev_infos->valid);
    SET_VALID(encoder_sampling_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->encoder_rate = 0;
      dev_infos->encoder_sampling = 0;
      RESET_VALID(encoder_rate_valid, dev_infos->valid);
      RESET_VALID(encoder_sampling_valid, dev_infos->valid);
    } else {
      printf(":: encoder_rate:%d\n",dev_infos->encoder_rate);
      printf(":: encoder_sampling:%d\n",dev_infos->encoder_sampling);
    }
    // Decoder infos
    retval = nvmlDeviceGetDecoderUtilization(dev_infos->device_handle,\
                                             &dev_infos->decoder_rate,   &dev_infos->decoder_sampling);
    SET_VALID(decoder_rate_valid, dev_infos->valid);
    SET_VALID(decoder_sampling_valid, dev_infos->valid);
    if (retval != NVML_SUCCESS) {
      dev_infos->decoder_rate = 0;
      dev_infos->decoder_sampling = 0;
      RESET_VALID(decoder_rate_valid, dev_infos->valid);
      RESET_VALID(decoder_sampling_valid, dev_infos->valid);
    } else {
      printf(":: decoder_rate:%d\n",dev_infos->decoder_rate);
      printf(":: decoder_sampling:%d\n",dev_infos->decoder_sampling);
    }
  // printf("DEBUG  for seg 2fault :\n");
 _fail:
_success:

  //  free(dev_infos);    
   retval = nvmlShutdown();//shutdown_gpu_info_extraction();
  dlclose(driver); 

 return ; 

}

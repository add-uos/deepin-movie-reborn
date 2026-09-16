/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  sysfs_pcidev.c
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

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/mman.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>  //basename() 

#include "pciaccess.h"
#include "pciaccess_private.h"
#include "sysfs_pcidev.h"


#if defined(__i386__) || defined(__x86_64__) || defined(__arm__)
#include <sys/io.h>
#else
#define inb(x) -1
#define inw(x) -1
#define inl(x) -1
#define outb(x,y) do {} while (0)
#define outw(x,y) do {} while (0)
#define outl(x,y) do {} while (0)
#define iopl(x) -1
#endif

#ifdef HAVE_MTRR
#include <asm/mtrr.h>
#include <sys/ioctl.h>
#endif

#define SYS_BUS_PCI "/sys/bus/pci/devices"


static int
scan_sys_pci_filter( const struct dirent * d )
{
    return !((strcmp( d->d_name, "." ) == 0)
	     || (strcmp( d->d_name, ".." ) == 0));
}
static int
sysfs_parse_separate_files(struct pci_device * dev)
{
    static const char *attrs[] = {
      "vendor",
      "device",
      "class",
      "revision",
      "subsystem_vendor",
      "subsystem_device",
      "driver",
    };
    char name[256];
    char resource[512];
    // char kerneldriver[512];
    uint64_t data[6];
    int fd;
    int i;

    for (i = 0; i < 6; i++) {
      snprintf(name, 255, "%s/%04x:%02x:%02x.%1u/%s",
        SYS_BUS_PCI,
        dev->domain,
        dev->bus,
        dev->dev,
        dev->func,
        attrs[i]);

      fd = open(name, O_RDONLY | O_CLOEXEC);
      if (fd == -1) {
          return errno;
      }

      read(fd, resource, 512);
      resource[511] = '\0';

      close(fd);   
        data[i] = strtoull(resource, NULL, 16);
    }

    dev->vendor_id = data[0] & 0xffff;
    dev->device_id = data[1] & 0xffff;
    dev->device_class = data[2] & 0xffffff;
    dev->revision = data[3] & 0xff;
    dev->subvendor_id = data[4] & 0xffff;
    dev->subdevice_id = data[5] & 0xffff;

    return 0;
}

struct pci_dev_driver sysfs_mediaDevice_info[MAX_NUM_MEDIA_DEVICE] ;
int8_t MediaDevice_number; 
int8_t sysfs_readMediaDevice_number(void)
{
  return(MediaDevice_number);
}

int sysfs_MediaDeviceGather(struct pci_device *device)
{
  char driver_path[256];
  char driver_name[48];
  char driver_name_lenth;

  if(0x030000 == (0xff0000 & device->device_class))    {
      memcpy(&sysfs_mediaDevice_info[MediaDevice_number].base,device,sizeof(struct pci_device));

      //  printf("pci {CLASS}[vendor:device]={0x%06x}[%04x:%04x]/sub[%04x:%04x]\n",	
      //     sysfs_mediaDevice_info[MediaDevice_number].base.device_class,  
      //     sysfs_mediaDevice_info[MediaDevice_number].base.vendor_id,      sysfs_mediaDevice_info[MediaDevice_number].base.device_id,  
      //     sysfs_mediaDevice_info[MediaDevice_number].base.subvendor_id,   sysfs_mediaDevice_info[MediaDevice_number].base.subvendor_id );         
      
      snprintf(driver_path, 255, "%s/%04x:%02x:%02x.%1u/driver",
            SYS_BUS_PCI,
            device->domain,
            device->bus,
            device->dev,
            device->func );       

      //  printf("Kernel-driver: %04x:%02x:%02x.%1u",	
      //     device->domain, device->bus, device->dev, device->func );

      if ((driver_name_lenth = readlink(driver_path, driver_name, sizeof(driver_name)-1)) != -1){  
          driver_name[driver_name_lenth] = '\0';
          
          driver_name_lenth = strlen(basename(driver_name));
          sysfs_mediaDevice_info[MediaDevice_number].driverName_lenth = driver_name_lenth;
          memcpy(sysfs_mediaDevice_info[MediaDevice_number].drivername, basename(driver_name), driver_name_lenth);
          //printf("/ %d / %s\n", driver_name_lenth, sysfs_mediaDevice_info[MediaDevice_number].drivername);
      }
      else  
        driver_name_lenth = 0;

      if(MediaDevice_number < MAX_NUM_MEDIA_DEVICE)
        MediaDevice_number++;                             
  }
}


int sysfs_populate_entries(void)
{ 
  struct pci_device  *pbase;
  struct dirent **devices = NULL;
  int n;
  int i;
  int err = 0;
 
  MediaDevice_number = 0;
  for(i=0;i< MAX_NUM_MEDIA_DEVICE; i++ )
      memset(&sysfs_mediaDevice_info[MediaDevice_number],0,sizeof(struct pci_dev_driver));

  n = scandir(SYS_BUS_PCI, &devices, scan_sys_pci_filter, alphasort);
//  printf("--debug  scandir: %d\n", n);
  if (n > 0)  {    
    pbase = calloc(n, sizeof(struct pci_device));

    if (pbase != NULL)  {     
      for (i = 0; i < n; i++)  {               
        uint8_t config[48];
        int ret;
        pciaddr_t bytes;
        unsigned dom, bus, dev, func;      

        sscanf(devices[i]->d_name, "%x:%02x:%02x.%1u", &dom, &bus, &dev, &func);
             	   
       pbase->domain = dom;
        /*
		 * Applications compiled with older versions  do not expect
		 * 32-bit domain numbers. To keep them working, we keep a 16-bit
		 * version of the domain number at the previous location.
		 */
        if (dom > 0xffff)
         pbase->domain_16 = 0xffff;
        else
         pbase->domain_16 = dom;

       pbase->bus = bus;
       pbase->dev = dev;
       pbase->func = func;       

       err = sysfs_parse_separate_files(pbase);
       if (!err )       {     
          sysfs_MediaDeviceGather(pbase); 
          continue;
       }

        err = sysfs_pci_device_linux_read(pbase, config, 0, 48, &bytes);
        if ((bytes == 48) && !err)
        {
         pbase->vendor_id = (uint16_t)config[0] + ((uint16_t)config[1] << 8);
         pbase->device_id = (uint16_t)config[2] + ((uint16_t)config[3] << 8);
         pbase->device_class = (uint32_t)config[9] + ((uint32_t)config[10] << 8) + ((uint32_t)config[11] << 16);
         pbase->revision = config[8];
         pbase->subvendor_id = (uint16_t)config[44] + ((uint16_t)config[45] << 8);
         pbase->subdevice_id = (uint16_t)config[46] + ((uint16_t)config[47] << 8);
          sysfs_MediaDeviceGather(pbase); 
        }
        if (err)
        {
          printf("error sysfs_pci_device_linux_read \n");
          break;
        }
      }

    }
    else
    {
      err = ENOMEM;
    }
  }

  for (i = 0; i < n; i++)
    free(devices[i]);
  free(devices);

  if (err)
  {
    free(pbase);
    pbase = NULL;
  }
  return err;
}



 struct  pci_dev_driver sysfs_read_MediaDevInfo(int8_t index)
 {
    struct pci_dev_driver *device = NULL;
    if((index >= MediaDevice_number)|| (index >= MAX_NUM_MEDIA_DEVICE)){
      printf("read_MediaDev num more than max\n");
     }    else
      device = &sysfs_mediaDevice_info[index];
  return(*device);  
 }

//  void sysfs_MediaDevice_print(void)
// {
//   int i;
//   struct pci_dev_driver tmp;
//   struct pci_dev_driver *device = &tmp;

//   printf("Media Device {CLASS}[vendor:device]    path-of-Kernel/driver    --vendor_name ----/ device_name  \n");	
//   // printf("\t-- vendor_name\n");	
//   // printf("\t\t-- sub vendor_name\n");	
//   printf("/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -\n");	

//   for(i=0;i<MediaDevice_number;i++)    {
//       // device = &sysfs_mediaDevice_info[i];
//       *device = sysfs_read_MediaDevInfo(i);
//       printf("{0x%06x}[%04x:%04x]/sub[%04x:%04x]",	
//           device->base.device_class,  
//           device->base.vendor_id,      device->base.device_id,  
//           device->base.subvendor_id,   device->base.subvendor_id );
                          
//       //printf("[%s]",driver_path);
//        printf(" \t%04x:%02x:%02x.%1u",	
//           device->base.domain, device->base.bus, device->base.dev, device->base.func );
//        printf(" / %s\n", device->drivername);
               
//        printf("\t-- %s" " / " "%s \n", pci_device_get_vendor_name(&device->base) ,
//                                      pci_device_get_device_name(&device->base)
//                                      );
//        printf("\t----/sub vendor_name: %s" " / " "%s \n", pci_device_get_subvendor_name(&device->base) ,
//                                      pci_device_get_subdevice_name(&device->base)
//                                      );                               
//     }
//       printf("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/\n");
//     return;
// }

int sysfs_pci_device_linux_probe( struct pci_device * dev )
{
    char     name[256];
    uint8_t  config[256];
    char     resource[512];
    int fd;
    pciaddr_t bytes;
    unsigned i;
    int err;


    err = sysfs_pci_device_linux_read( dev, config, 0, 256, & bytes );
        if ( bytes >= 64 ) {
      struct pci_device_private *priv = (struct pci_device_private *) dev;

      dev->irq = config[60];
      priv->header_type = config[14];


      /* The PCI config registers can be used to obtain information
      * about the memory and I/O regions for the device.  However,
      * doing so requires some tricky parsing (to correctly handle
      * 64-bit memory regions) and requires writing to the config
      * registers.  Since we'd like to avoid having to deal with the
      * parsing issues and non-root users can write to PCI config
      * registers, we use a different file in the device's sysfs
      * directory called "resource".
      *
      * The resource file contains all of the needed information in
      * a format that is consistent across all platforms.  Each BAR
      * and the expansion ROM have a single line of data containing
      * 3, 64-bit hex values:  the first address in the region,
      * the last address in the region, and the region's flags.
      */
      snprintf( name, 255, "%s/%04x:%02x:%02x.%1u/resource",
          SYS_BUS_PCI,
          dev->domain,
          dev->bus,
          dev->dev,
          dev->func );
      fd = open( name, O_RDONLY | O_CLOEXEC);
      if ( fd != -1 ) {
        char * next;
        pciaddr_t  low_addr;
        pciaddr_t  high_addr;
        pciaddr_t  flags;


        bytes = read( fd, resource, 512 );
        resource[511] = '\0';

        close( fd );

        next = resource;
        for ( i = 0 ; i < 6 ; i++ ) {

          dev->regions[i].base_addr = strtoull( next, & next, 16 );
          high_addr = strtoull( next, & next, 16 );
          flags = strtoull( next, & next, 16 );

          if ( dev->regions[i].base_addr != 0 ) {
              dev->regions[i].size = (high_addr
                    - dev->regions[i].base_addr) + 1;

              dev->regions[i].is_IO = (flags & 0x01);
              dev->regions[i].is_64 = (flags & 0x04);
              dev->regions[i].is_prefetchable = (flags & 0x08);
          }
        }

        low_addr = strtoull( next, & next, 16 );
        high_addr = strtoull( next, & next, 16 );
        flags = strtoull( next, & next, 16 );
        if ( low_addr != 0 ) {
          priv->rom_base = low_addr;
          dev->rom_size = (high_addr - low_addr) + 1;
        }
      }
    }
    return err;
}


 int sysfs_pci_device_linux_read_rom( struct pci_device * dev, void * buffer )
{
    char name[256];
    int fd;
    struct stat  st;
    int err = 0;
    size_t rom_size;
    size_t total_bytes;


    snprintf( name, 255, "%s/%04x:%02x:%02x.%1u/rom",
	      SYS_BUS_PCI,
	      dev->domain,
	      dev->bus,
	      dev->dev,
	      dev->func );

    fd = open( name, O_RDWR | O_CLOEXEC);
    if ( fd == -1 ) {
#ifdef LINUX_ROM
	/* If reading the ROM using sysfs fails, fall back to the old
	 * /dev/mem based interface.
	 * disable this for newer kernels using configure
	 */
	return pci_device_linux_devmem_read_rom(dev, buffer);
#else
	return errno;
#endif
    }


    if ( fstat( fd, & st ) == -1 ) {
	close( fd );
	return errno;
    }

    rom_size = st.st_size;
    if ( rom_size == 0 )
	rom_size = 0x10000;

    /* This is a quirky thing on Linux.  Even though the ROM and the file
     * for the ROM in sysfs are read-only, the string "1" must be written to
     * the file to enable the ROM.  After the data has been read, "0" must be
     * written to the file to disable the ROM.
     */
    write( fd, "1", 1 );
    lseek( fd, 0, SEEK_SET );

    for ( total_bytes = 0 ; total_bytes < rom_size ; /* empty */ ) {
	const int bytes = read( fd, (char *) buffer + total_bytes,
				rom_size - total_bytes );
	if ( bytes == -1 ) {
	    err = errno;
	    break;
	}
	else if ( bytes == 0 ) {
	    break;
	}

	total_bytes += bytes;
    }


    lseek( fd, 0, SEEK_SET );
    write( fd, "0", 1 );

    close( fd );
    return err;
}


 int sysfs_pci_device_linux_read( struct pci_device * dev, void * data,
			     pciaddr_t offset, pciaddr_t size,
			     pciaddr_t * bytes_read )
{
    char name[256];
    pciaddr_t temp_size = size;
    int err = 0;
    int fd;
    char *data_bytes = data;

    if ( bytes_read != NULL ) {
	*bytes_read = 0;
    }

    /* Each device has a directory under sysfs.  Within that directory there
     * is a file named "config".  This file used to access the PCI config
     * space.  It is used here to obtain most of the information about the
     * device.
     */
    snprintf( name, 255, "%s/%04x:%02x:%02x.%1u/config",
	      SYS_BUS_PCI,
	      dev->domain,
	      dev->bus,
	      dev->dev,
	      dev->func );

    fd = open( name, O_RDONLY | O_CLOEXEC);
    if ( fd == -1 ) {
	return errno;
    }


    while ( temp_size > 0 ) {
	const ssize_t bytes = pread64( fd, data_bytes, temp_size, offset );

	/* If zero bytes were read, then we assume it's the end of the
	 * config file.
	 */
	if (bytes == 0)
	    break;
	if ( bytes < 0 ) {
	    err = errno;
	    break;
	}

	temp_size -= bytes;
	offset += bytes;
	data_bytes += bytes;
    }

    if ( bytes_read != NULL ) {
	*bytes_read = size - temp_size;
    }

    close( fd );
    return err;
}

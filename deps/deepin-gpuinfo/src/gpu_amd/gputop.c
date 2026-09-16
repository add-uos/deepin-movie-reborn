/*
    Copyright (C) 2012 Lauri Kasanen

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, version 3 of the License.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "radeontop.h"
#include <getopt.h>

#include <stdio.h>
#include <stdlib.h>
#include <pciaccess.h>  //jeff20211228 //#include "../lspci/pci.h" 
#include "config.h"
#include "version.h"
// #include "../lspci/sysfs.h"  //jeff20211228
#include "../lspci/sysfs_pcidev.h"

void die(char *msg, ...)
{
  va_list args;

  va_start(args, msg);
  //fprintf(stderr, "%s: ", program_name);
  vfprintf(stderr, msg, args);
  fputc('\n', stderr);
  exit(1);
}


int lspci_top_main(struct  pci_dev_driver* pci_info)
{
  
  printf("GPU AMD top wait 2s ... \n");
        
  struct pci_device tmp;
  struct pci_device *dev = &tmp;

 
    seteuid(getuid());		
    unsigned char bus = 0, forcemem = 0;


  #ifdef ENABLE_NLS
    setlocale(LC_ALL, "");
    bindtextdomain("radeontop", "/usr/share/locale");
    textdomain("radeontop");
  #endif

  printf("pci: %04x:%02x:%02x.%u\n",	pci_info->base.domain, pci_info->base.bus,pci_info->base.dev, pci_info->base.func);
  memcpy(dev,&pci_info->base,sizeof(struct pci_device));

  // printf("pci: %04x:%02x:%02x.%u\n",	dev->domain, dev->bus,dev->dev, dev->func);

    //init (regain privileges for bus initialization and ultimately drop them afterwards);dev->vendor_id, dev->device_id)
    seteuid(0);
    const unsigned int pciaddr = detect_init_pci(dev);
    setuid(getuid());
    

    const int family = getfamily(dev->device_id);
    if (!family)
      puts(_("Unknown Radeon card. <= R500 won't work, new cards might."));

    const char * const cardname = family_str[family];
    //printf("__debug__pci___addr:%x,%x , %s \n",pciaddr,dev->device_id, cardname);

    initbits(family);

    det_dumpdata_show();	

    printf("----UOS---gpuTop Success---- \n");
    return 0;

}





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
#include <pciaccess.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <pciaccess.h>  //jeff20211228 //#include "../lspci/pci.h" 
#include "../lspci/sysfs_pcidev.h"
/*
static struct pci_device_id mwv206_idlist[] = {
	{0x0731, 0x7200, PCI_ANY_ID, PCI_ANY_ID, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0}
};
*/

struct bits_t bits;
unsigned long long vramsize;
unsigned long long gttsize;
int drm_fd = -1;
char drm_name[10] = ""; // should be radeon or amdgpu

char busid[32];
int use_ioctl;
const void *area;

void authenticate_drm(int fd) {
	drm_magic_t magic;

	/* Obtain magic for our DRM client. */
	if (drmGetMagic(fd, &magic) < 0) {
		return;
	}

	/* Try self-authenticate (if we are somehow the master). */
	if (drmAuthMagic(fd, magic) == 0) {
		return;
	}

#ifdef ENABLE_XCB
	call_authenticate_drm_xcb(magic);
#endif
}

struct pci_device  * pci_id_match_probe(unsigned char bus, uint32_t   vendor_id)
{

	struct pci_id_match match;


	match.vendor_id = vendor_id ;   
	match.device_id = PCI_MATCH_ANY;
	match.subvendor_id = PCI_MATCH_ANY;
	match.subdevice_id = PCI_MATCH_ANY;
	match.device_class = 0;
	match.device_class_mask = 0;
	match.match_data = 0;

	struct pci_device_iterator *iter = pci_id_match_iterator_create(&match);
	struct pci_device *dev = NULL;
	

	while ((dev = pci_device_next(iter))) {
		pci_device_probe(dev);
		if ((dev->device_class & 0x00ffff00) != 0x00030000 &&
			(dev->device_class & 0x00ffff00) != 0x00038000)
			continue;
		snprintf(busid, sizeof(busid), "pci:%04x:%02x:%02x.%u",	dev->domain, dev->bus, dev->dev, dev->func);

		//printf("card pci busid is : %s\n",busid);  //card pci busid is : pci:0000:01:00.0

		if (!bus || bus == dev->bus)
			break;
	}

	pci_iterator_destroy(iter);

	// if (!dev)
	// 	die(("Can't find Radeon cards"));

	return(dev);
}


unsigned char forcemem = 0;
int detect_init_pci(struct pci_device *dev) 
{
	unsigned char bus = 0;
	int ret = 0;

	snprintf(busid, sizeof(busid), "pci:%04x:%02x:%02x.%u",	dev->domain, dev->bus, dev->dev, dev->func);
	// printf("card pci busid is : %s\n",busid); 

	const unsigned int device_id = dev->device_id;
	int reg = 2;
	if (getfamily(device_id) >= BONAIRE)
		reg = 5;

	sysfs_pci_device_linux_probe(dev);

	if (!dev->regions[reg].size)  //if (!dev->size[reg]) 
		printf("Can't get the register area size\n");	
 	// printf("Found area %p, size %lu\n", area, dev->regions[reg].size);

	// DRM support for VRAM

	drm_fd = drmOpen(NULL, busid);
	if (drm_fd >= 0) {
		drmVersionPtr ver = drmGetVersion(drm_fd);
		if (strcmp(ver->name, "radeon") != 0 
		 && strcmp(ver->name, "amdgpu") != 0) {
			close(drm_fd);
			drm_fd = -1;
		}
		strcpy(drm_name, ver->name);
		drmFreeVersion(ver);
	}
	if (drm_fd < 0 && access("/dev/ati/card0", F_OK) == 0) { // fglrx path
	
		drm_fd = open("/dev/ati/card0", O_RDWR);
		printf("--debug  /dev/ati/card0 fglrx path\n");
	}	
	
	if (drm_fd >= 0) {
	
		uint32_t rreg = 0x8010;
		use_ioctl = get_drm_value(drm_fd, RADEON_INFO_READ_REG, &rreg);
		// printf("use_ioctl : %d\n", use_ioctl);
	}


	use_ioctl = 0;
	if (drm_fd >= 0) {
		authenticate_drm(drm_fd);
		uint32_t rreg = 0x8010;
		use_ioctl = get_drm_value(drm_fd, RADEON_INFO_READ_REG, &rreg);
	}

	if (forcemem) {
		printf(_("Forcing the /dev/mem path.\n"));
		use_ioctl = 0;
	}

	if (!use_ioctl) {
		int mem = open("/dev/mem", O_RDONLY);
		if (mem < 0) {
			die(("Cannot access GPU registers, are you root?"));			
		}

		area = mmap(NULL, MMAP_SIZE, PROT_READ, MAP_PRIVATE, mem,
				dev->regions[reg].base_addr + 0x8000);
		if (area == MAP_FAILED){ 
			printf("mmap failed\n"); //die(_("mmap failed"));
		}
		 
	}

	bits.vram = 0;
	bits.gtt = 0;
	if (drm_fd < 0) {
		printf(_("Failed to open DRM node, no VRAM support.\n"));
	} else {
		drmDropMaster(drm_fd);
		drmVersionPtr ver = drmGetVersion(drm_fd);

/*		printf("Version %u.%u.%u, name %s\n",
			ver->version_major,
			ver->version_minor,
			ver->version_patchlevel,
			ver->name);*/

		if (ver->version_major < 2 ||
			(ver->version_major == 2 && ver->version_minor < 36)) {
			printf(_("Kernel too old for VRAM reporting.\n"));
			drmFreeVersion(ver);
			goto out;
		}
		drmFreeVersion(ver);

		// No version indicator, so we need to test once
		// We use different codepaths for radeon and amdgpu
		// We store vram_size and check below if the ret value is sane
		if (strcmp(drm_name, "radeon") == 0) {
			struct drm_radeon_gem_info gem;

			ret = drmCommandWriteRead(drm_fd, DRM_RADEON_GEM_INFO,
							&gem, sizeof(gem));
			vramsize = gem.vram_size;
			gttsize = gem.gart_size;
		} else if (strcmp(drm_name, "amdgpu") == 0) {
#ifdef ENABLE_AMDGPU
			struct drm_amdgpu_info_vram_gtt vram_gtt = {};

			struct drm_amdgpu_info request;
			memset(&request, 0, sizeof(request));
			request.return_pointer = (unsigned long) &vram_gtt;
			request.return_size = sizeof(vram_gtt);
			request.query = AMDGPU_INFO_VRAM_GTT;

			ret = drmCommandWrite(drm_fd, DRM_AMDGPU_INFO,
						&request, sizeof(request));
			vramsize = vram_gtt.vram_size;
			gttsize = vram_gtt.gtt_size;
#else
			printf(_("amdgpu DRM driver is used, but amdgpu VRAM size reporting is not enabled\n"));
#endif
		}
		if (ret) {
			printf(_("Failed to get VRAM size, error %d\n"),
				ret);
			goto out;
		}

		ret = getvram();
		if (ret == 0) {
			if (strcmp(drm_name, "amdgpu") == 0) {
#ifndef ENABLE_AMDGPU
				printf(_("amdgpu DRM driver is used, but amdgpu VRAM usage reporting is not enabled\n"));
#endif
			}
			printf(_("Failed to get VRAM usage, kernel likely too old\n"));
			goto out;
		}

		bits.vram = 1;

		ret = getgtt();
		if (ret == 0) {
			if (strcmp(drm_name, "amdgpu") == 0) {
#ifndef ENABLE_AMDGPU
				printf(_("amdgpu DRM driver is used, but amdgpu GTT usage reporting is not enabled\n"));
#endif
			}
			printf(_("Failed to get GTT usage, kernel likely too old\n"));
			goto out;
		}
		bits.gtt = 1;
	}

	out:
	return device_id;
}


unsigned long long getvram(void) 
{

	int ret = -1;
	unsigned long long val = 0;

	if (strcmp(drm_name, "radeon") == 0) {
		struct drm_radeon_info info;
		memset(&info, 0, sizeof(info));
		info.value = (unsigned long) &val;
		info.request = RADEON_INFO_VRAM_USAGE;

		ret = drmCommandWriteRead(drm_fd, DRM_RADEON_INFO, &info, sizeof(info));
	} else if (strcmp(drm_name, "amdgpu") == 0) {
#ifdef ENABLE_AMDGPU
		struct drm_amdgpu_info request;
		memset(&request, 0, sizeof(request));
		request.return_pointer = (unsigned long) &val;
		request.return_size = sizeof(val);
		request.query = AMDGPU_INFO_VRAM_USAGE;

		ret = drmCommandWrite(drm_fd, DRM_AMDGPU_INFO, &request, sizeof(request));
#endif
	}
	if (ret) return 0;

	return val;
}

unsigned long long getgtt(void) 
{
	int ret = -1;
	unsigned long long val = 0;

	if (strcmp(drm_name, "radeon") == 0) {
		struct drm_radeon_info info;
		memset(&info, 0, sizeof(info));
		info.value = (unsigned long) &val;
		info.request = RADEON_INFO_GTT_USAGE;

		ret = drmCommandWriteRead(drm_fd, DRM_RADEON_INFO, &info, sizeof(info));
	} else if (strcmp(drm_name, "amdgpu") == 0) {
#ifdef ENABLE_AMDGPU
		struct drm_amdgpu_info info;

		memset(&info, 0, sizeof(struct drm_amdgpu_info));
		info.query = AMDGPU_INFO_GTT_USAGE;
		info.return_pointer = (unsigned long)&val;
		info.return_size = sizeof(val);

		ret = drmCommandWriteRead(drm_fd, DRM_AMDGPU_INFO, &info, sizeof(info));
#endif
	}
	if (ret) return 0;

	return val;
}

int getfamily(unsigned int id) 
{

	switch(id) {
		#define CHIPSET(a,b,c) case a: return c;
		#include "r600_pci_ids.h"
		#undef CHIPSET
	}
	return 0;
}

void initbits(int fam) 
{

	// The majority of these is the same from R600 to Southern Islands.

	bits.ee = (1U << 10);
	bits.vgt = (1U << 16) | (1U << 17);
	bits.ta = (1U << 14);
	bits.tc = (1U << 19);
	bits.sx = (1U << 20);
	bits.sh = (1U << 21);
	bits.spi = (1U << 22);
	bits.smx = (1U << 23);
	bits.sc = (1U << 24);
	bits.pa = (1U << 25);
	bits.db = (1U << 26);
	bits.cr = (1U << 27);
	bits.cb = (1U << 30);
	bits.gui = (1U << 31);

	// R600 has a different texture bit, and only R600 has the TC, CR, SMX bits
	if (fam < RV770) {
		bits.ta = (1U << 18);
	} else {
		bits.tc = 0;
		bits.cr = 0;
		bits.smx = 0;
	}
}
//----------------
int get_drm_value(int fd, unsigned request, uint32_t *out) 
{
	struct drm_radeon_info info;
	int retval;

	memset(&info, 0, sizeof(info));

	info.value = (unsigned long)out;
	info.request = request;

	retval = drmCommandWriteRead(fd, DRM_RADEON_INFO, &info, sizeof(info));
	return !retval;
}

unsigned int readgrbm(void) 
{

	if (use_ioctl) { // printf("_debuf_readgrbm_reg\n");	
		uint32_t reg = 0x8010;
		get_drm_value(drm_fd, RADEON_INFO_READ_REG, &reg);
		return reg;
	} else //if (MAP_FAILED != area)  
	{  //printf("_debuf_readgrbm_inta\n");	
		const void *ptr = (const char *) area + 0x10;
		const unsigned int *inta = ptr;
		return *inta;	
		
	}
	//return 0;
}


struct bits_t *results = NULL;

unsigned int ticks = 120;

void det_dumpdata_show(void) 
{

//const unsigned int ticks = * ((unsigned int *) arg);
	

	struct bits_t res[2];

	// Save one second's worth of history
	struct bits_t *history = calloc(ticks, sizeof(struct bits_t));
	unsigned int cur = 0, curres = 0;

	unsigned int cnt = 1 ;

	const useconds_t sleeptime = 1e6 / ticks;

	//while (1) 
	while(0 != cnt% 5)
	{
		unsigned int stat = readgrbm();

		memset(&history[cur], 0, sizeof(struct bits_t));

		if (stat & bits.ee) history[cur].ee = 1;
		if (stat & bits.vgt) history[cur].vgt = 1;
		if (stat & bits.gui) history[cur].gui = 1;
		if (stat & bits.ta) history[cur].ta = 1;
		if (stat & bits.tc) history[cur].tc = 1;
		if (stat & bits.sx) history[cur].sx = 1;
		if (stat & bits.sh) history[cur].sh = 1;
		if (stat & bits.spi) history[cur].spi = 1;
		if (stat & bits.smx) history[cur].smx = 1;
		if (stat & bits.sc) history[cur].sc = 1;
		if (stat & bits.pa) history[cur].pa = 1;
		if (stat & bits.db) history[cur].db = 1;
		if (stat & bits.cr) history[cur].cr = 1;
		if (stat & bits.cb) history[cur].cb = 1; 


		usleep(sleeptime);
		cur++;
		cur %= ticks;

		// One second has passed, we have one sec's worth of data
		if (cur == 0) {
			unsigned int i;
			cnt++;

			memset(&res[curres], 0, sizeof(struct bits_t));

			for (i = 0; i < ticks; i++) {
				res[curres].ee += history[i].ee;
				res[curres].vgt += history[i].vgt;
				res[curres].gui += history[i].gui;
				res[curres].ta += history[i].ta;
				res[curres].tc += history[i].tc;
				res[curres].sx += history[i].sx;
				res[curres].sh += history[i].sh;
				res[curres].spi += history[i].spi;
				res[curres].smx += history[i].smx;
				res[curres].sc += history[i].sc;
				res[curres].pa += history[i].pa;
				res[curres].db += history[i].db;
				res[curres].cb += history[i].cb;
				res[curres].cr += history[i].cr;
			}
			
			res[curres].vram = getvram();
			res[curres].gtt = getgtt();

			// Atomically write it to the pointer
			__sync_bool_compare_and_swap(&results, results, &res[curres]);

			curres++;
			curres %= 2;
		}
	}
	dump2data();
	munmap((void *) area, MMAP_SIZE);
	return ;
}

#define FPRIN  printf
//----------------
//void dump2data(const unsigned int ticks, const char file[], const unsigned int limit) {
void dump2data(void) 
{
// #ifdef ENABLE_NLS
// 	// This is a data format, so disable decimal point localization
// 	setlocale(LC_NUMERIC, "C");			puts(_("ENABLE_NLS \n."));
// #endif

	// This does not need to be atomic. A delay here is acceptable.
	//while(!results)		usleep(12000);

	// Again, no need to protect these. Worst that happens is a slightly
	// wrong number.

	float ee = 100.0 * (float) results->ee / ticks;
	float vgt = 100.0 * (float) results->vgt / ticks;
	float gui = 100.0 * (float) results->gui / ticks;
	float ta = 100.0 * (float) results->ta / ticks;
	float tc = 100.0 * (float) results->tc / ticks;
	float sx = 100.0 * (float) results->sx / ticks;
	float sh = 100.0 * (float) results->sh / ticks;
	float spi = 100.0 * (float) results->spi / ticks;
	float smx = 100.0 * (float) results->smx / ticks;
	float sc = 100.0 * (float) results->sc / ticks;
	float pa = 100.0 * (float) results->pa / ticks;
	float db = 100.0 * (float) results->db / ticks;
	float cr = 100.0 * (float) results->cr / ticks;
	float cb = 100.0 * (float) results->cb / ticks;
	float vram = 100.0 * (float) results->vram / vramsize;
	float vrammb = results->vram / 1024.0f / 1024.0f;
	float gtt = 100.0 * (float) results->gtt / gttsize;
	float gttmb = results->gtt / 1024.0f / 1024.0f;

	FPRIN( "gpu  %3.2f%% \t(Graphics pipe),\n ", gui);
	FPRIN( "ee   %3.2f%% \t(Event Engine ),\n ", ee);
	FPRIN( "vgt  %3.2f%% \t(Vertex Grouper + Tesselator),\n ", vgt);
	FPRIN( "ta   %3.2f%% \t(Texture Addresser),\n ", ta);

	if (bits.tc)
		FPRIN( "tc   %3.2f%% \t(Texture Cache),\n ", tc);

	FPRIN( "sx   %3.2f%% \t(Shader Export),\n ", sx);
	FPRIN( "sh   %3.2f%% \t(Sequencer Instruction Cache),\n ", sh);
	FPRIN( "spi  %3.2f%% \t(Shader Interpolator),\n ", spi);

	if (bits.smx)
		FPRIN( "smx  %3.2f%% \t(Shader Memory Exchange),\n ", smx);

	if (bits.cr)
		FPRIN( "cr   %3.2f%% \t(Clip Rectangle),\n ", cr);

	FPRIN( "sc   %3.2f%% \t(Scan Converter),\n ", sc);
	FPRIN( "pa   %3.2f%% \t(Primitive Assembly),\n ", pa);
	FPRIN( "db   %3.2f%% \t(Depth Block),\n ", db);
	FPRIN( "cb   %3.2f%% \t(Color Block),\n", cb);

	if (bits.vram)
		FPRIN( "vram  %3.2f%% \t(VRAM) %.2fmb,\n", vram, vrammb);
	else
		FPRIN( "\n");

	if (bits.gtt)
		FPRIN( "gtt   %3.2f%% \t(GTT) %.2fmb ,\n", gtt, gttmb);
	else
		FPRIN( "\n");	
}
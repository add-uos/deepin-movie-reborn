/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  vdpau_wrapper.c
 * Author:     shuaijie <shuaijie@uniontech.com> 
 * Maintainer: shuaijie <shuaijie@uniontech.com>
 * descrition: This file is part of deepin-gpuinfo. 
 *
 * This program is free software: you can redistribute it and/or modify,it underthe terms  of the GNU General Public 
 *  License as published by the Free Software Foundation, either version 3 of the License, or any later version. 
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even  the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.You should have
 * received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "../vdpauinfo/vdpauinfo.h"
#include "libgpuinfo_decoder.h"
#include "../lspci/sysfs_pcidev.h"
#include "../gpu_glxinfo/glxinfo_detect.h"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <va/va_str.h>
#include "../vainfo/va_display.h"

#define MAGIC_DECODER_PROBE  (0X175)

extern const char * const dec_str[];

extern VDP_Decoder_e va_to_VDP_Profile(VAProfile profile);
int VDP_create_decodeInfo_Mem(void);
int VAAPI_create_decodeInfo_Mem(void);
char*  csv_search(const char* ss);
char* csv_search_by_opengl(const char* opengl_renderer);
char* csv_search_vo_by_opengl(const char* opengl_renderer);

static bool b_vdpauinfo_init = false;
static VDP_Decoder_t  decoder_infoMem[_decoder_maxnull+10];

//------------------------------------
char *strlowr(char *str)
{
    char *orign=str;
    for (; *str!='\0'; str++)
        *str = tolower(*str);
    return orign;
}

#define HISI_DRM_GPU_MAGIC    (0x1314)
#define PHYTIUM_GPU_MAGIC     (0x1db7)

int is_hisi_GPU(void)
{
	FILE *fp;
	char buffer[80];
	char *ret=NULL;

	fp=popen("dmidecode -s system-product-name","r");
   if(fp == NULL) {    
      return(-1);
   }
	while(fgets(buffer,sizeof(buffer),fp))
	{
    printf("%s",buffer);
    if(NULL == ret)
      ret = strstr(buffer, "KLVV");
    else if(NULL == ret)
      ret = strstr(buffer, "KLVU");
    else if(NULL == ret)
      ret = strstr(buffer, "PGUV");
    else if(NULL == ret)
      ret = strstr(buffer, "PGUW");
	}
	pclose(fp);
  if (ret)
    return HISI_DRM_GPU_MAGIC;
  else 
    return 0;
}

bool try_dec_vdpau = false;
bool try_dec_vaapi = false;

unsigned int  iter_gpu(void)
{

  char pciid[32];
  struct pci_dev_driver mediaDevInfo;

  sysfs_populate_entries();  
 
  for(int nn=0;nn<sysfs_readMediaDevice_number();nn++){
    mediaDevInfo = sysfs_read_MediaDevInfo(nn);
    if(mediaDevInfo.driverName_lenth < 2) 
    {  // no  kernel driver name  skip
      printf("miss  kernel driver name  \n");
      continue;
    }

    snprintf(pciid, sizeof(pciid), "%04x:%04x",	mediaDevInfo.base.vendor_id, mediaDevInfo.base.device_id);    
    // printf("pciid--: %s\n", csv_search(pciid));
    char* tmp = csv_search(pciid);
    if(NULL != tmp)
    {
      if(NULL != strstr(strlowr(tmp),"vaapi"))
      {  
        try_dec_vaapi = true;
      }
        
      if(NULL != strstr(strlowr(tmp), "vdpau"))
      {  
        try_dec_vdpau = true;
      }
      printf("vid:pid %s, csv_search : %s\n",pciid,strlowr(tmp));
    }
    else  //0xffff  通配的方法
    {
      snprintf(pciid, sizeof(pciid), "%04x:%04x",	mediaDevInfo.base.vendor_id, 0xffff);
      char* tmp = csv_search(pciid);
      if(NULL != tmp)
      {
        if(NULL != strstr(strlowr(tmp),"vaapi"))
        {  
          try_dec_vaapi = true;          
        }
          
        if(NULL != strstr(strlowr(tmp), "vdpau"))
        {  
          try_dec_vdpau = true;          
        } 
        printf("vid:pid %s, csv_search : %s\n",pciid,strlowr(tmp));        
      }
    }
    if(try_dec_vaapi || try_dec_vdpau)
      return  mediaDevInfo.base.vendor_id;


    if(mediaDevInfo.base.vendor_id == 0x1002) {
       // printf("amd GPU  support vaapi vdpau now \n");   //try_dec_vdpau
        try_dec_vaapi = true;
        try_dec_vdpau = true;
    }
    if(mediaDevInfo.base.vendor_id == 0x10de) {  //mvidia
        //printf("NVIDIA GPU  support vdpau now \n");    //try_dec_vdpau
        try_dec_vdpau = true;
    }
    if(mediaDevInfo.base.vendor_id == 0x0731) {  //景嘉微
       // printf("JM GPU  support vdpau now \n");   //try_dec_vdpau
        try_dec_vaapi = true;
    }
    if(mediaDevInfo.base.vendor_id == 0x8086) {    //intel
        //printf("Intel GPU  support vdpau now \n");   //try_dec_vaapi
        try_dec_vaapi = true;
    }
    if(mediaDevInfo.base.vendor_id == 0x1ec8) {     //芯动
       // printf("inno GPU  support vdpau now \n");   //try_dec_vaapi
        try_dec_vaapi = true;
    }
    if(mediaDevInfo.base.vendor_id == 0x1d17) {
        //printf("Zhaoxin GPU  support vdpau now \n");   //try_dec_vdpau
        try_dec_vdpau = true;
    }
  }

  if( ((false == try_dec_vdpau) &&  (false == try_dec_vaapi)) || (0==sysfs_readMediaDevice_number())){
    if(HISI_DRM_GPU_MAGIC == is_hisi_GPU())
       return HISI_DRM_GPU_MAGIC;

    // glxinfo fallback: try OpenGL renderer detection
    const char* opengl_renderer = glxinfo_get_renderer();
    if (opengl_renderer) {
        char* codec = csv_search_by_opengl(opengl_renderer);
        if (codec) {
            if (strstr(strlowr(codec), "vaapi")) try_dec_vaapi = true;
            if (strstr(strlowr(codec), "vdpau")) try_dec_vdpau = true;
            // Return magic number when glxinfo detection succeeds
            if (try_dec_vaapi || try_dec_vdpau) {
                printf("glxinfo detected GPU: %s\n", opengl_renderer);
                return PHYTIUM_GPU_MAGIC;
            }
        }
    }
  }

  return  mediaDevInfo.base.vendor_id;
}

//-----------------------------------------------------------------------------------
  char tmpc[2] = ",";
  char newline[2] = "\n";
  char namec[4] = ": ";

//-----vaapi----------------------------
#define CHECK_VASTATUS(va_status, func, ret)                                                             \
  if (va_status != VA_STATUS_SUCCESS)                                                                    \
  {                                                                                                      \
    fprintf(stderr, "%s failed with error code %d (%s),exit\n", func, va_status, vaErrorStr(va_status)); \
    ret_val = ret;                                                                                       \
    goto error;                                                                                          \
  }

int VAAPI_create_decodeInfo_Mem(void)
{
  VADisplay va_dpy;
  VAStatus va_status;
  int major_version, minor_version;
  const char *driver;

  const char *name;
  VAProfile profile, *profile_list = NULL;
  int num_profiles, max_num_profiles, i;
  VAEntrypoint entrypoint, *entrypoints = NULL;
  int num_entrypoint = 0;
  int ret_val = 0;
  VDP_Decoder_t * v_queryinfo;

  name = "deepin-gpuinfo";

  va_dpy = va_open_display();
  if (NULL == va_dpy)  {
    fprintf(stderr, "%s: vaGetDisplay() failed\n", name);
    return 2;
  }

  va_status = vaInitialize(va_dpy, &major_version, &minor_version);
  CHECK_VASTATUS(va_status, "vaInitialize", 3);
  // printf("%s: VA-API version: %d.%d\n", name, major_version, minor_version);

  driver = vaQueryVendorString(va_dpy);
  // printf("%s: va Driver version: %s\n", name, driver ? driver : "<unknown>");

  num_entrypoint = vaMaxNumEntrypoints(va_dpy);
  entrypoints = malloc(num_entrypoint * sizeof(VAEntrypoint));
  if (!entrypoints)  {
    printf("Failed to allocate memory for entrypoint list\n");
    ret_val = -1;
    goto error;
  }

  // printf("%s: Supported profile and entrypoints\n", name);
  max_num_profiles = vaMaxNumProfiles(va_dpy);
  profile_list = malloc(max_num_profiles * sizeof(VAProfile));

  if (!profile_list)  {
    printf("Failed to allocate memory for profile list\n");
    ret_val = 5;
    goto error;
  }

  va_status = vaQueryConfigProfiles(va_dpy, profile_list, &num_profiles);
  CHECK_VASTATUS(va_status, "vaQueryConfigProfiles", 6);

  for (i = 0; i < num_profiles; i++)
  {
    profile = profile_list[i];
    if(VAProfileNone == profile) continue;
    int ii = va_to_VDP_Profile(profile)  ;
    // if(i == VAProfileVC1Simple)
    // printf("VAProfileVC1Simple : %d ,  %d ",  profile,ii);

    memset(decoder_infoMem[ii].ret_info,'\0', sizeof(decoder_infoMem[0].ret_info));
    strcpy(&decoder_infoMem[ii].ret_info,decoder_2profiles[ii].name); 

    va_status = vaQueryConfigEntrypoints(va_dpy, profile, entrypoints, &num_entrypoint);
    if (va_status == VA_STATUS_ERROR_UNSUPPORTED_PROFILE)
      continue;

    CHECK_VASTATUS(va_status, "vaQueryConfigEntrypoints", 4);

    for (entrypoint = 0; entrypoint < num_entrypoint; entrypoint++)   {
      // printf("   %-32s <-->	%-21s ,", vaProfileStr(profile), vaEntrypointStr(entrypoints[entrypoint]));

          VASurfaceAttrib *surface_attribs;
          unsigned int num_image_formats, num_surface_attribs;
          VAConfigID vpp_config_id = VA_INVALID_ID;
          VASurfaceAttrib *va_surface_attribs;
          int va_num_surface_attribs = -1;

          num_image_formats = vaMaxNumImageFormats(va_dpy);
          if (num_image_formats == 0)
              return 0;

          va_status = vaCreateConfig(va_dpy, (profile), VAEntrypointVLD,
              NULL, 0, &vpp_config_id);
          CHECK_VASTATUS(va_status, "vaCreateConfig()",5);

          /* Guess the number of surface attributes, thus including any
          pixel-format supported by the VA driver */
          num_surface_attribs = VASurfaceAttribCount + num_image_formats;

          surface_attribs = (VASurfaceAttrib *)malloc(num_surface_attribs * sizeof(*surface_attribs));
          if (!surface_attribs)
              return 0;

          va_status = vaQuerySurfaceAttributes(va_dpy, vpp_config_id,
              surface_attribs, &num_surface_attribs);
          if (va_status == VA_STATUS_SUCCESS)
              va_surface_attribs =  surface_attribs;
          else if (va_status == VA_STATUS_ERROR_MAX_NUM_EXCEEDED) {
                printf("waring: VA_STATUS_ERROR_MAX_NUM_EXCEEDED");
              va_surface_attribs = (VASurfaceAttrib *)realloc(surface_attribs,
                  num_surface_attribs * sizeof(*va_surface_attribs));
              if (!va_surface_attribs) {
                  free(surface_attribs);
                  return 0;
              }
              va_status = vaQuerySurfaceAttributes(va_dpy, vpp_config_id,
                  va_surface_attribs, &num_surface_attribs);
          }
          CHECK_VASTATUS(va_status, "vaQuerySurfaceAttributes()",6);
          va_num_surface_attribs = num_surface_attribs;

          decoder_infoMem[ii].func = ii;
          decoder_infoMem[ii].is_supported = true;

          for (int j = 0; j < va_num_surface_attribs; j++) {
              const VASurfaceAttrib * const surface_attrib = &va_surface_attribs[j];
              if (surface_attrib->type == VASurfaceAttribMaxWidth) 
                decoder_infoMem[ii].max_width  = surface_attrib->value.value.i;
                  // printf("MaxWidth: %-5d ,",   surface_attrib->value.value.i );
              if (surface_attrib->type == VASurfaceAttribMaxHeight) 
                decoder_infoMem[ii].max_height = surface_attrib->value.value.i;
                  // printf("MaxHeight: %-5d ,", surface_attrib->value.value.i );
                
          }
          // printf("\n");

    } //for(entrypoint....)
  }

  error:
  free(entrypoints);
  free(profile_list);
  vaTerminate(va_dpy);
  va_close_display(va_dpy);

  return ret_val;
}
//-------------------------------------------------------------------

VDPDeviceImpl tmpv;
VDPDeviceImpl *device =&tmpv;
Display *display;
  
void vdpInfo_Decoder(VDP_Decoder_t * v_queryinfo)
{
  VdpStatus rv;
  VdpBool is_supported;
    
  int x = v_queryinfo->func - decoder_profiles_MPEG1;

  memset(v_queryinfo->ret_info,'0',sizeof(v_queryinfo->ret_info));
  v_queryinfo->is_supported = VDP_FALSE;
  v_queryinfo->max_width  = 0;
  v_queryinfo->max_height = 0;
 
  //vdpauinfo_init(device);  //must initial first 
  
  uint32_t max_width, max_height;
  uint32_t max_level, max_macroblocks;

  rv = device->DecoderQueryCapabilities(device->device, decoder_2profiles[x].id,
                                        &is_supported, &max_level, &max_macroblocks, &max_width, &max_height);
  if (rv == VDP_STATUS_OK && is_supported)  {
    //printf("%-30s %2i %5i %5i %5i\n", decoder_2profiles[x].name, max_level, max_macroblocks, max_width, max_height);
      
      strcpy(v_queryinfo->ret_info,decoder_2profiles[x].name);
      //strcat(v_queryinfo->ret_info,tmpc);
      v_queryinfo->max_width  = max_width;
      v_queryinfo->max_height = max_height;
      v_queryinfo->is_supported = is_supported;
      v_queryinfo->max_level  = max_level;
      v_queryinfo->max_macroblocks = max_macroblocks;
    }      
}

int VDP_create_decodeInfo_Mem(void)
{
  VdpStatus rv;
  VdpBool cnt_support = 0;
  VDP_Decoder_e ii;
  VDP_Decoder_t  _Vdp_dec;
  char ret_info[RET_INFO_LENTH_MAX]; //[out]  name

  if(!vdpauinfo_init(device)){  //must initial first
    return -1;
  }
  for(ii = decoder_profiles_MPEG1; ii < _decoder_maxnull; ii++)  {

    decoder_infoMem[ii].func =  ii;
    decoder_infoMem[ii].is_supported = VDP_FALSE;
    decoder_infoMem[ii].max_width  = 0;
    decoder_infoMem[ii].max_height = 0;

    int x = _Vdp_dec.func - decoder_profiles_MPEG1;

    memset(&_Vdp_dec, 0, sizeof(_Vdp_dec));
    _Vdp_dec.func = x;

    vdpInfo_Decoder( &decoder_infoMem[ii]);
  }//end of   for(ii = decoder_profiles_MPEG1;...

//add for x11,/dev/dri/renderD128 FD leak.
  device->DeviceDestroy(device->device);
  XCloseDisplay(display);
   return 0;
}



// the only API interface for deepin-movie
unsigned int vdp_Iter_decoderInfo(decoder_profile index, VDP_Decoder_t* result )
{
  VdpBool is_supported;
  static unsigned int magic_number_have_decoderprobe = 0;

  VdpStatus rv; 
  VdpBool cnt_support = 0;
  VDP_Decoder_e ii;
  VDP_Decoder_t  _Vdp_dec;
  char ret_info[RET_INFO_LENTH_MAX]; //[out]  name

    * result = (VDP_Decoder_t) {
    .is_supported = 0,
    .max_height = 0,
    .max_width =  0,
    .max_level = 0,
    .max_macroblocks = 0,
  };

  is_supported = VDP_FALSE;
  unsigned int tmp = iter_gpu();
  if(!tmp)  //-----------
      return 0 ;
  else if(0x1d17 == tmp){
	printf("Zhaoxin GPU  support forever \n");
	 result -> max_height = 4096;
	 result -> max_width = 4096;
	return tmp;
 }  else if(HISI_DRM_GPU_MAGIC == tmp){
	printf("HISI_DRM_GPU  support forever \n");
	 result -> max_height = 4096*2;
	 result -> max_width = 4096*2;
	return tmp;
 }  else if(PHYTIUM_GPU_MAGIC == tmp){
	printf("Phytium GPU  support forever \n");
	 result -> max_height = 4096;
	 result -> max_width = 4096;
	return tmp;
 }

  if((false == try_dec_vdpau)  &&  (false == try_dec_vaapi) )
  return 0;
  else if(MAGIC_DECODER_PROBE != magic_number_have_decoderprobe)    {
      if(try_dec_vaapi)
          VAAPI_create_decodeInfo_Mem();
      if(try_dec_vdpau)
        VDP_create_decodeInfo_Mem();

        magic_number_have_decoderprobe = MAGIC_DECODER_PROBE;
        printf("MAGIC_DECODER_PROBE \n");
  }

  strcpy(ret_info , namec);
  for(ii = decoder_profiles_MPEG1; ii < _decoder_maxnull; ii++)  {
      // printf("%-30s %2i %5i %5i %5i  %-30s\n", decoder_2profiles[ii].name,  decoder_infoMem[ii].max_level, decoder_infoMem[ii].max_macroblocks,
      //  decoder_infoMem[ii].max_width,  decoder_infoMem[ii].max_height, decoder_infoMem[ii].ret_info);
    if(0 != strcmp(dec_str[index], decoder_2profiles[ii].abbr )){
      continue;
    }
    if(!decoder_infoMem[ii].is_supported)  continue;

     memcpy(&_Vdp_dec, &decoder_infoMem[ii], sizeof(_Vdp_dec));

      // printf("%-30s %2i %5i %5i %5i  %-30s\n", decoder_2profiles[ii].name,  _Vdp_dec.max_level,  _Vdp_dec.max_macroblocks,
      //  _Vdp_dec.max_width,  _Vdp_dec.max_height, _Vdp_dec.ret_info);
    if(_Vdp_dec.is_supported)
    {
      memcpy(result, &_Vdp_dec,sizeof(_Vdp_dec));
      strcat(ret_info , _Vdp_dec.ret_info);
      strcat(ret_info , tmpc);
      cnt_support++;
      // printf("%-30s %2i %5i %5i %5i  %-30s\n", decoder_2profiles[x].name, _Vdp_dec.max_level, _Vdp_dec.max_macroblocks,
      // _Vdp_dec.max_width, _Vdp_dec.max_height,_Vdp_dec.ret_info);
    }

  }//end of   for(ii = decoder_profiles_MPEG1;...

  if(cnt_support)
   strcpy(result->ret_info , ret_info);
  return(cnt_support);
}


#define GET_ADDR(device, function_id, result)                                    \
  do{                                                                            \
    void *tmp;                                                                   \
    if(hwctx->get_proc_address(device, function_id, &tmp) != VDP_STATUS_OK){     \
       return false;                                                            \
    }                                                                           \
    result = tmp;                                                               \
  } while (0)

struct Options {
    char *display_name;
    int screen;
};

bool vdpauinfo_init(VDPDeviceImpl *hwctx)
{
  char *display_name;
  VdpStatus rverr;
  struct Options oo ;
  b_vdpauinfo_init = false;

    oo.display_name = XDisplayName(NULL);
    oo.screen = -1;

  /* Create an X Display */
  display = XOpenDisplay(oo.display_name);
  if(!display)
    return b_vdpauinfo_init; 

  int screen = (oo.screen == -1) ? DefaultScreen(display) : oo.screen;
   //printf("display: %s   screen: %i\n", oo.display_name, screen);


  /* Create device */

  rverr = vdp_device_create_x11(display, screen,
                                &hwctx->device, &hwctx->get_proc_address);
  //  assert(rverr == VDP_STATUS_OK);   //x100 显卡不支持
  if(rverr != VDP_STATUS_OK) {
    return b_vdpauinfo_init;
  }

  GET_ADDR(hwctx->device, VDP_FUNC_ID_GET_ERROR_STRING, hwctx->GetErrorString);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_GET_PROC_ADDRESS, hwctx->GetProcAddress);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_GET_API_VERSION, hwctx->GetApiVersion);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_GET_INFORMATION_STRING, hwctx->GetInformationString);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_DEVICE_DESTROY, hwctx->DeviceDestroy);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_GENERATE_CSC_MATRIX, hwctx->GenerateCSCMatrix);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_SURFACE_QUERY_CAPABILITIES, hwctx->VideoSurfaceQueryCapabilities);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_SURFACE_QUERY_GET_PUT_BITS_Y_CB_CR_CAPABILITIES, hwctx->VideoSurfaceQueryGetPutBitsYCbCrCapabilities);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_SURFACE_CREATE, hwctx->VideoSurfaceCreate);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_SURFACE_DESTROY, hwctx->VideoSurfaceDestroy);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_SURFACE_GET_PARAMETERS, hwctx->VideoSurfaceGetParameters);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_SURFACE_GET_BITS_Y_CB_CR, hwctx->VideoSurfaceGetBitsYCbCr);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_SURFACE_PUT_BITS_Y_CB_CR, hwctx->VideoSurfacePutBitsYCbCr);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_QUERY_CAPABILITIES, hwctx->OutputSurfaceQueryCapabilities);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_QUERY_GET_PUT_BITS_NATIVE_CAPABILITIES, hwctx->OutputSurfaceQueryGetPutBitsNativeCapabilities);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_QUERY_PUT_BITS_INDEXED_CAPABILITIES, hwctx->OutputSurfaceQueryPutBitsIndexedCapabilities);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_QUERY_PUT_BITS_Y_CB_CR_CAPABILITIES, hwctx->OutputSurfaceQueryPutBitsYCbCrCapabilities);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_CREATE, hwctx->OutputSurfaceCreate);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_DESTROY, hwctx->OutputSurfaceDestroy);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_GET_PARAMETERS, hwctx->OutputSurfaceGetParameters);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_GET_BITS_NATIVE, hwctx->OutputSurfaceGetBitsNative);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_PUT_BITS_NATIVE, hwctx->OutputSurfacePutBitsNative);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_PUT_BITS_INDEXED, hwctx->OutputSurfacePutBitsIndexed);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_PUT_BITS_Y_CB_CR, hwctx->OutputSurfacePutBitsYCbCr);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_BITMAP_SURFACE_QUERY_CAPABILITIES, hwctx->BitmapSurfaceQueryCapabilities);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_BITMAP_SURFACE_CREATE, hwctx->BitmapSurfaceCreate);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_BITMAP_SURFACE_DESTROY, hwctx->BitmapSurfaceDestroy);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_BITMAP_SURFACE_GET_PARAMETERS, hwctx->BitmapSurfaceGetParameters);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_BITMAP_SURFACE_PUT_BITS_NATIVE, hwctx->BitmapSurfacePutBitsNative);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_RENDER_OUTPUT_SURFACE, hwctx->OutputSurfaceRenderOutputSurface);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_OUTPUT_SURFACE_RENDER_BITMAP_SURFACE, hwctx->OutputSurfaceRenderBitmapSurface);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_DECODER_QUERY_CAPABILITIES, hwctx->DecoderQueryCapabilities);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_DECODER_CREATE, hwctx->DecoderCreate);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_DECODER_DESTROY, hwctx->DecoderDestroy);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_DECODER_GET_PARAMETERS, hwctx->DecoderGetParameters);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_DECODER_RENDER, hwctx->DecoderRender);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_QUERY_FEATURE_SUPPORT, hwctx->VideoMixerQueryFeatureSupport);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_QUERY_PARAMETER_SUPPORT, hwctx->VideoMixerQueryParameterSupport);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_QUERY_ATTRIBUTE_SUPPORT, hwctx->VideoMixerQueryAttributeSupport);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_QUERY_PARAMETER_VALUE_RANGE, hwctx->VideoMixerQueryParameterValueRange);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_QUERY_ATTRIBUTE_VALUE_RANGE, hwctx->VideoMixerQueryAttributeValueRange);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_CREATE, hwctx->VideoMixerCreate);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_SET_FEATURE_ENABLES, hwctx->VideoMixerSetFeatureEnables);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_SET_ATTRIBUTE_VALUES, hwctx->VideoMixerSetAttributeValues);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_GET_FEATURE_SUPPORT, hwctx->VideoMixerGetFeatureSupport);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_GET_FEATURE_ENABLES, hwctx->VideoMixerGetFeatureEnables);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_GET_PARAMETER_VALUES, hwctx->VideoMixerGetParameterValues);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_GET_ATTRIBUTE_VALUES, hwctx->VideoMixerGetAttributeValues);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_DESTROY, hwctx->VideoMixerDestroy);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_VIDEO_MIXER_RENDER, hwctx->VideoMixerRender);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_TARGET_DESTROY, hwctx->PresentationQueueTargetDestroy);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_CREATE, hwctx->PresentationQueueCreate);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_DESTROY, hwctx->PresentationQueueDestroy);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_SET_BACKGROUND_COLOR, hwctx->PresentationQueueSetBackgroundColor);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_GET_BACKGROUND_COLOR, hwctx->PresentationQueueGetBackgroundColor);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_GET_TIME, hwctx->PresentationQueueGetTime);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_DISPLAY, hwctx->PresentationQueueDisplay);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_BLOCK_UNTIL_SURFACE_IDLE, hwctx->PresentationQueueBlockUntilSurfaceIdle);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_QUERY_SURFACE_STATUS, hwctx->PresentationQueueQuerySurfaceStatus);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PREEMPTION_CALLBACK_REGISTER, hwctx->PreemptionCallbackRegister);
  GET_ADDR(hwctx->device, VDP_FUNC_ID_PRESENTATION_QUEUE_TARGET_CREATE_X11, hwctx->PresentationQueueTargetCreateX11);


  //  XCloseDisplay(display);
  return true;
}


// void test_vdpauinfo4_main(void)
// {
// VDP_Decoder_t head ;
// char rest[1021];
// // head .ret_info = rest;

// int i;
// i = vdp_Iter_decoderInfo( MPEG2, &head );
// printf("test: %d, %s",i, head.ret_info);
  
// }




//------------------------------------

// Declaration of function in csv_parse.c
extern char* csv_search_vo(const char* pci_id);

// Helper: copy vo string to buffer, skip "NUL"
static const char* vo_to_buffer(const char* vo)
{
    if (vo == NULL || strcmp(vo, "NUL") == 0)
        return NULL;
    static char vo_buffer[128];
    strncpy(vo_buffer, vo, sizeof(vo_buffer) - 1);
    vo_buffer[sizeof(vo_buffer) - 1] = '\0';
    free(vo);
    return vo_buffer;
}

// Helper: try glxinfo fallback for vo config
static const char* try_glxinfo_vo_fallback(void)
{
    const char* opengl_renderer = glxinfo_get_renderer();
    if (opengl_renderer) {
        return vo_to_buffer(csv_search_vo_by_opengl(opengl_renderer));
    }
    return NULL;
}

// Get vo config for current GPU from csv
const char* gpuinfo_get_vo(void)
{
    char pciid[32];
    struct pci_dev_driver mediaDevInfo;

    sysfs_populate_entries();
    unsigned int num = sysfs_readMediaDevice_number();
    if (num == 0) {
        return try_glxinfo_vo_fallback();
    }

    // Iterate all GPU devices
    for (unsigned int i = 0; i < num; i++) {
        mediaDevInfo = sysfs_read_MediaDevInfo(i);
        if (mediaDevInfo.driverName_lenth < 2) {
            continue;
        }

        snprintf(pciid, sizeof(pciid), "%04x:%04x",
                 mediaDevInfo.base.vendor_id, mediaDevInfo.base.device_id);

        const char* vo = vo_to_buffer(csv_search_vo(pciid));
        if (vo) return vo;

        // Try wildcard matching
        snprintf(pciid, sizeof(pciid), "%04x:ffff", mediaDevInfo.base.vendor_id);
        vo = vo_to_buffer(csv_search_vo(pciid));
        if (vo) return vo;
    }

    return try_glxinfo_vo_fallback();
}

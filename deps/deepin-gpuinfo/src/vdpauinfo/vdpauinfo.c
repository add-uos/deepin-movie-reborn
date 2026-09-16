/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  vdpauinfo.c
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
#include "vdpauinfo.h"

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "../libgpuinfo/libgpuinfo_decoder.h"
#include "../lspci/sysfs_pcidev.h"
#include "..//queryinfo/pci.h"



/**************** Video surface ************/
 Desc chroma_types[] = {
    {"420", VDP_CHROMA_TYPE_420},
    {"422", VDP_CHROMA_TYPE_422},
    {"444", VDP_CHROMA_TYPE_444},
};

 Desc ycbcr_types[] = {
    {"NV12",      VDP_YCBCR_FORMAT_NV12},
    {"YV12",      VDP_YCBCR_FORMAT_YV12},
    {"UYVY",      VDP_YCBCR_FORMAT_UYVY},
    {"YUYV",      VDP_YCBCR_FORMAT_YUYV},
    {"Y8U8V8A8",  VDP_YCBCR_FORMAT_Y8U8V8A8},
    {"V8U8Y8A8",  VDP_YCBCR_FORMAT_V8U8Y8A8},
};
 Desc rgb_types[] = {
    {"B8G8R8A8",    VDP_RGBA_FORMAT_B8G8R8A8},
    {"R8G8B8A8",    VDP_RGBA_FORMAT_R8G8B8A8},
    {"R10G10B10A2", VDP_RGBA_FORMAT_R10G10B10A2},
    {"B10G10R10A2", VDP_RGBA_FORMAT_B10G10R10A2},
    {"A8",          VDP_RGBA_FORMAT_A8},
};
 Desc indexed_types[] = {
    {"A4I4", VDP_INDEXED_FORMAT_A4I4},
    {"I4A4", VDP_INDEXED_FORMAT_I4A4},
    {"A8I8", VDP_INDEXED_FORMAT_A8I8},
    {"I8A8", VDP_INDEXED_FORMAT_I8A8},
};

Desc mixer_features[] = {
    {"DEINTERLACE_TEMPORAL",      VDP_VIDEO_MIXER_FEATURE_DEINTERLACE_TEMPORAL},
    {"DEINTERLACE_TEMPORAL_SPATIAL", VDP_VIDEO_MIXER_FEATURE_DEINTERLACE_TEMPORAL_SPATIAL},
    {"INVERSE_TELECINE",          VDP_VIDEO_MIXER_FEATURE_INVERSE_TELECINE},
    {"NOISE_REDUCTION",           VDP_VIDEO_MIXER_FEATURE_NOISE_REDUCTION},
    {"SHARPNESS",                 VDP_VIDEO_MIXER_FEATURE_SHARPNESS},
    {"LUMA_KEY",                  VDP_VIDEO_MIXER_FEATURE_LUMA_KEY},
    {"HIGH QUALITY SCALING - L1", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L1},
    {"HIGH QUALITY SCALING - L2", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L2},
    {"HIGH QUALITY SCALING - L3", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L3},
    {"HIGH QUALITY SCALING - L4", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L4},
    {"HIGH QUALITY SCALING - L5", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L5},
    {"HIGH QUALITY SCALING - L6", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L6},
    {"HIGH QUALITY SCALING - L7", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L7},
    {"HIGH QUALITY SCALING - L8", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L8},
    {"HIGH QUALITY SCALING - L9", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L9},
};

Desc mixer_parameters[] = {
    {"VIDEO_SURFACE_WIDTH",   VDP_VIDEO_MIXER_PARAMETER_VIDEO_SURFACE_WIDTH, DT_UINT},
    {"VIDEO_SURFACE_HEIGHT",  VDP_VIDEO_MIXER_PARAMETER_VIDEO_SURFACE_HEIGHT, DT_UINT},
    {"CHROMA_TYPE",           VDP_VIDEO_MIXER_PARAMETER_CHROMA_TYPE, DT_NONE},
    {"LAYERS",               VDP_VIDEO_MIXER_PARAMETER_LAYERS, DT_UINT},
};


Desc mixer_attributes[] = {
    {"BACKGROUND_COLOR",      VDP_VIDEO_MIXER_ATTRIBUTE_BACKGROUND_COLOR, DT_NONE},
    {"CSC_MATRIX",            VDP_VIDEO_MIXER_ATTRIBUTE_CSC_MATRIX, DT_NONE},
    {"NOISE_REDUCTION_LEVEL", VDP_VIDEO_MIXER_ATTRIBUTE_NOISE_REDUCTION_LEVEL, DT_FLOAT},
    {"SHARPNESS_LEVEL",       VDP_VIDEO_MIXER_ATTRIBUTE_SHARPNESS_LEVEL, DT_FLOAT},
    {"LUMA_KEY_MIN_LUMA",     VDP_VIDEO_MIXER_ATTRIBUTE_LUMA_KEY_MIN_LUMA, DT_NONE},
    {"LUMA_KEY_MAX_LUMA",     VDP_VIDEO_MIXER_ATTRIBUTE_LUMA_KEY_MAX_LUMA, DT_NONE},
};


const size_t chroma_type_count      = sizeof(chroma_types) / sizeof(Desc);
const size_t ycbcr_type_count       = sizeof(ycbcr_types) / sizeof(Desc);
const size_t rgb_type_count         = sizeof(rgb_types) / sizeof(Desc);
const size_t indexed_type_count     = sizeof(indexed_types) / sizeof(Desc);
const size_t mixer_features_count   = sizeof(mixer_features) / sizeof(Desc);
const size_t mixer_parameters_count = sizeof(mixer_parameters) / sizeof(Desc);
const size_t mixer_attributes_count = sizeof(mixer_attributes) / sizeof(Desc);


/**************** queryBaseInfo ************/
void queryBaseInfo(VDPDeviceImpl *device)
{
  uint32_t api;
  //  char arryinfo[254];
   char const *info ;

  device->GetInformationString(&info);
  device->GetApiVersion(&api);

  printf("API version: %i\n", api);
 printf("Information string: %s\n", info);
}


/**************** Video surface ************/
void queryVideoSurface(VDPDeviceImpl *device)
{
  VdpStatus rv;
  printf("\n#  Video surface:\n");
  printf("name   width height types\n");
  printf("-------------------------------------------\n");
  for (int x = 0; x < chroma_type_count; ++x)
  {
    VdpBool is_supported;
    uint32_t max_width, max_height;

    rv = device->VideoSurfaceQueryCapabilities(device->device, chroma_types[x].id,
                                               &is_supported, &max_width, &max_height);
    if (rv == VDP_STATUS_OK && is_supported)
    {
      printf("%-6s %5i %5i  ", chroma_types[x].name,  max_width, max_height);
      /* Find out supported formats */
      for (int y = 0; y < ycbcr_type_count; ++y)
      {
        rv = device->VideoSurfaceQueryGetPutBitsYCbCrCapabilities(device->device, chroma_types[x].id, ycbcr_types[y].id,
            &is_supported);
        if (rv == VDP_STATUS_OK && is_supported)
        {
          printf("%s ", ycbcr_types[y].name);
        }
      }
      printf("\n");
    }
  }
}

/***************** Output surface ****************/
void queryOutputSurface(VDPDeviceImpl *device)
{
  VdpStatus rv;
  printf("\n#  Output surface:\n");
  printf("name              width height nat types\n");
  printf("----------------------------------------------------\n");
  for (int x = 0; x < rgb_type_count; ++x)
  {
    VdpBool is_supported, native;
    uint32_t max_width, max_height;

    rv = device->OutputSurfaceQueryCapabilities(device->device, rgb_types[x].id, &is_supported, &max_width, &max_height);
    device->OutputSurfaceQueryGetPutBitsNativeCapabilities(device->device, rgb_types[x].id,   &native);
    if (rv == VDP_STATUS_OK && is_supported)    {
      printf("%-16s %5i %5i    %c  ", rgb_types[x].name, max_width, max_height, native ? 'y' : '-');
      /* Find out supported formats */
      for (int y = 0; y < ycbcr_type_count; ++y)
      {
        rv = device->OutputSurfaceQueryPutBitsYCbCrCapabilities(
            device->device, rgb_types[x].id, ycbcr_types[y].id,
            &is_supported);
        if (rv == VDP_STATUS_OK && is_supported)
        {
          printf("%s ", ycbcr_types[y].name);
        }
      }

      for (int y = 0; y < indexed_type_count; ++y)
      {
        // There is currently only one color table format.  This will
        // have to be reconsidered if additional color table formats are
        // added.
        rv = device->OutputSurfaceQueryPutBitsIndexedCapabilities(
            device->device, rgb_types[x].id, indexed_types[y].id,
            VDP_COLOR_TABLE_FORMAT_B8G8R8X8, &is_supported);
        if (rv == VDP_STATUS_OK && is_supported)
        {
          printf("%s ", indexed_types[y].name);
        }
      }
      printf("\n");
    }
  }
}

/***************** Bitmap surface ****************/
void queryBitmapSurface(VDPDeviceImpl *device)
{
  VdpStatus rv;
  printf("\n#  Bitmap surface:\n");
  printf("name              width height\n");
  printf("------------------------------\n");
  for (int x = 0; x < rgb_type_count; ++x)
  {
    VdpBool is_supported;
    uint32_t max_width, max_height;

    rv = device->BitmapSurfaceQueryCapabilities(device->device, rgb_types[x].id,
                                                &is_supported, &max_width, &max_height);
    if (rv == VDP_STATUS_OK && is_supported)
    {
      printf("%-16s %5i %5i\n", rgb_types[x].name,  max_width, max_height);
    }
  }
}

/******************* Video mixer ****************/
void display_range(uint32_t aux, uint32_t minval, uint32_t maxval)
{
  switch (aux)
  {
  case DT_INT:
    printf("%8i %8i", minval, maxval);
    break;
  case DT_UINT:
    printf("%8u %8u", minval, maxval);
    break;
  case DT_FLOAT:
    printf("%8.2f %8.2f", *((float *)&minval), *((float *)&maxval));
    break;
  default: /* Ignore value which we don't know how to display */;
  }
}

void queryVideoMixer(VDPDeviceImpl *device)
{
  VdpStatus rv;
  printf("\n#  Video mixer:\n");
  // Features
  printf("feature name                    sup\n");
  printf("------------------------------------\n");
  for (int x = 0; x < mixer_features_count; ++x)
  {
    VdpBool is_supported;

    rv = device->VideoMixerQueryFeatureSupport(device->device, mixer_features[x].id,
                                               &is_supported);
    is_supported = (rv == VDP_STATUS_OK && is_supported);
    // printf("%-32s %c\n", mixer_features[x].name,   is_supported ? 'y' : '-');
    if(is_supported)  printf("%-32s %c\n", mixer_features[x].name, 'y');
  }
  printf("\n");
  // Parameters (+range)
  printf("parameter name                  sup      min      max\n");
  printf("-----------------------------------------------------\n");
  for (int x = 0; x < mixer_parameters_count; ++x)
  {
    VdpBool is_supported;

    rv = device->VideoMixerQueryParameterSupport(device->device, mixer_parameters[x].id,
                                                 &is_supported);
    is_supported = (rv == VDP_STATUS_OK && is_supported);
    printf("%-32s %c  ", mixer_parameters[x].name,      is_supported ? 'y' : '-');
    /* VDPAU spec does not allow range query for DT_NONE types */
    if (is_supported && mixer_parameters[x].aux != DT_NONE)
    {
      uint32_t minval, maxval;
      rv = device->VideoMixerQueryParameterValueRange(device->device, mixer_parameters[x].id,
                                                      (void *)&minval, (void *)&maxval);
      if (rv == VDP_STATUS_OK)
        display_range(mixer_parameters[x].aux, minval, maxval);
    }
    printf("\n");
  }
  printf("\n");

  // Attributes (+range)
  printf("attribute name                  sup      min      max\n");
  printf("-----------------------------------------------------\n");
  for (int x = 0; x < mixer_attributes_count; ++x)
  {
    VdpBool is_supported;

    rv = device->VideoMixerQueryAttributeSupport(device->device, mixer_attributes[x].id,
                                                 &is_supported);
    is_supported = (rv == VDP_STATUS_OK && is_supported);
    printf("%-32s %c  ", mixer_attributes[x].name,
           is_supported ? 'y' : '-');
    /* VDPAU spec does not allow range query for DT_NONE types */
    if (is_supported && mixer_attributes[x].aux != DT_NONE)
    {
      uint32_t minval, maxval;
      rv = device->VideoMixerQueryAttributeValueRange(device->device, mixer_parameters[x].id,
                                                      (void *)&minval, (void *)&maxval);
      if (rv == VDP_STATUS_OK)
        display_range(mixer_attributes[x].aux, minval, maxval);
    }
    printf("\n");
  }
  printf("\n");
}

/******************* Decoder ****************/
void queryDecoderCaps(VDPDeviceImpl *device)
{
  VdpStatus rv;
  printf("\n#  Decoder capabilities:\n");
  printf("name                        level macbs width height\n");
  printf("----------------------------------------------------\n");
  for (int x = 0; x < decoder_profile_count; ++x)
  {
    VdpBool is_supported;
    uint32_t max_level, max_macroblocks, max_width, max_height;

    rv = device->DecoderQueryCapabilities(device->device, decoder_2profiles[x].id,
                                          &is_supported, &max_level, &max_macroblocks, &max_width, &max_height);
    if (rv == VDP_STATUS_OK && is_supported)
    {
      printf("%-30s %2i %5i %5i %5i\n", decoder_2profiles[x].name,
             max_level, max_macroblocks, max_width, max_height);
    }
    // else    {
    //   printf("%-30s --- not supported ---\n", decoder_2profiles[x].name);
    // }
  }
}

void test_vainfo_main(void);
void test_vdpauinfo_main(void);

void test_vdpauinfo_main(void)
{
  VDP_Decoder_t head ;
char rest[1021];

int i;
i = vdp_Iter_decoderInfo(H264, &head );
printf("test: %d, %s",i, head.ret_info);

}

uint8_t is_hwSupport(void)  //make sure call  sysfs_populate_entries() before
{
  int8_t is_hwsupport = 0;
  VDP_Decoder_t head ;

  decoder_profile ep_dec = MPEG1;
  for(ep_dec = MPEG1; ep_dec<_MAXNULL;ep_dec++){
    if(vdp_Iter_decoderInfo(ep_dec, &head))
	    return 1;
  }

return is_hwsupport;
}

void vdpauinfo_main(void)
{

  //  test_vdpauinfo_main();

  VDPDeviceImpl mm;
  VDPDeviceImpl *hwctx = &mm;

  if(0 == is_hwSupport())
    return;
  if(!vdpauinfo_init(hwctx)) { //must initial first
    printf(" vdpauinfo_init return \n");
    return;
  }


  queryBaseInfo( hwctx);
  queryVideoSurface(hwctx);
  queryDecoderCaps(hwctx);
  queryOutputSurface(hwctx);
  queryBitmapSurface(hwctx);
  queryVideoMixer(hwctx);
}




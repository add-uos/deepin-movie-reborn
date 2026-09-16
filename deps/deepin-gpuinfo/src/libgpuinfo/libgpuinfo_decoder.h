/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  libgpuinfo_decoder.h
 * Author:     shuaijie <shuaijie@uniontech.com> 
 * Maintainer: shuaijie <shuaijie@uniontech.com>
 * descrition:  This file is part of libgpuinfo. 
 *
 * This program is free software: you can redistribute it and/or modify,it underthe terms  of the GNU General Public 
 *  License as published by the Free Software Foundation, either version 3 of the License, or any later version. 
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even  the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.You should have
 * received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * libgpuinfo_decoder API Specification
 * Revision History:
 * rev 0.10 (12/09/2021 shuaijie) - Initial draft
 */

#ifndef _LIBGPUINFO_DECODER_H
#define _LIBGPUINFO_DECODER_H

#include <bits/stdint-uintn.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


//for vainfo
int va_getMaxNumProfile(void);
int vainfo_getNameProfile(char * va_support_profile);

//for vdpauinfo
typedef int VdpBool;

/* Generic description structure */
typedef struct Desc{
  const char *name;
  uint32_t id;
  uint32_t aux; /* optional extra parameter... */
} Desc;

typedef enum  {
base_Information = 0,
base_GetApiVersion,
_base_maxnull
}VDP_base_e;

typedef struct  {
  VDP_base_e  func;                 //[option  no need]
  char* ret_info;                   //[out]
}VDP_base_t;

/**************** Video surface ************/
typedef enum  {
chroma_420 = 0,         // {"420", VDP_CHROMA_TYPE_420},
chroma_422,             // {"422", VDP_CHROMA_TYPE_422},
chroma_444,             // {"444", VDP_CHROMA_TYPE_444},
_VideoSurface_maxnull
}VDP_VideoSurface_e;

typedef struct  {
  VDP_VideoSurface_e  func;        //[in]
  VdpBool is_supported;            //[out]
  uint32_t max_width;
  uint32_t max_height;
  char* ret_info;                  //[out]  name : types
}VDP_VideoSurface_t;

// printf("%-6s %5i %5i  ", chroma_types[x].name,  max_width, max_height);  printf("%s ", ycbcr_types[y].name);

#define  RET_INFO_LENTH_MAX  (512)

typedef struct _decoder_t{
  char      *abbr;
  char      *name;
  uint32_t   id;
  uint32_t  max_width;
  uint32_t  max_height; 

} decoder_t;


extern const size_t decoder_profile_count ;
extern decoder_t decoder_2profiles[] ;


/***************** Output surface **************queryOutputSurface(device)**/
typedef enum  {
  rgb_B8G8R8A8 = 0,   // {"B8G8R8A8", VDP_RGBA_FORMAT_B8G8R8A8},
  rgb_R8G8B8A8,       // {"R8G8B8A8", VDP_RGBA_FORMAT_R8G8B8A8},
  rgb_R10G10B10A2,    // {"R10G10B10A2", VDP_RGBA_FORMAT_R10G10B10A2},
  rgb_B10G10R10A2,    // {"B10G10R10A2", VDP_RGBA_FORMAT_B10G10R10A2},
  rgb_A8,             // {"A8", VDP_RGBA_FORMAT_A8}, 
  _rgb_maxnull
}rgb_e;

typedef struct  {
  rgb_e  func;                      //[in]
  VdpBool is_supported;             //[out]
  uint32_t max_width;               //[out]
  uint32_t max_height;              //[out]
  char* ret_info;                   //[out]  name :types
}VDP_OutputSurface_t;


typedef struct  {
  rgb_e  func;        //[in]
  VdpBool is_supported;             //[out]
  uint32_t max_width;               //[out]
  uint32_t max_height;              //[out]
  char* ret_info;                   //[out]  name
}VDP_BitmapSurface_t;


/******************* Video mixer ***************queryVideoMixer(device);*/
typedef enum  {
  mixer_features_DEINTERLACE_TEMPORAL = 0,      //     {"DEINTERLACE_TEMPORAL", VDP_VIDEO_MIXER_FEATURE_DEINTERLACE_TEMPORAL},
  mixer_features_DEINTERLACE_TEMPORAL_SPATIAL,  //     {"DEINTERLACE_TEMPORAL", VDP_VIDEO_MIXER_FEATURE_DEINTERLACE_TEMPORAL_SPATIAL},
  mixer_features_INVERSE_TELECINE,              //     {"INVERSE_TELECINE", VDP_VIDEO_MIXER_FEATURE_INVERSE_TELECINE},
  mixer_features_NOISE_REDUCTION,               //     {"NOISE_REDUCTION", VDP_VIDEO_MIXER_FEATURE_NOISE_REDUCTION},
  mixer_features_SHARPNESS,                     //     {"SHARPNESS", VDP_VIDEO_MIXER_FEATURE_SHARPNESS},
  mixer_features_LUMA_KEY,                      //     {"LUMA_KEY", VDP_VIDEO_MIXER_FEATURE_LUMA_KEY},
  mixer_features_HIGHQUALITYSCALING_L1,         //     {"HIGH QUALITY SCALING - L1", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L1},
  mixer_features_HIGHQUALITYSCALING_L2,         //     {"HIGH QUALITY SCALING - L2", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L2},
  mixer_features_HIGHQUALITYSCALING_L3,         //     {"HIGH QUALITY SCALING - L3", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L3},
  mixer_features_HIGHQUALITYSCALING_L4,         //     {"HIGH QUALITY SCALING - L4", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L4},
  mixer_features_HIGHQUALITYSCALING_L5,         //     {"HIGH QUALITY SCALING - L5", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L5},
  mixer_features_HIGHQUALITYSCALING_L6,         //     {"HIGH QUALITY SCALING - L6", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L6},
  mixer_features_HIGHQUALITYSCALING_L7,         //     {"HIGH QUALITY SCALING - L7", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L7},
  mixer_features_HIGHQUALITYSCALING_L8,         //     {"HIGH QUALITY SCALING - L8", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L8},
  mixer_features_HIGHQUALITYSCALING_L9,         //     {"HIGH QUALITY SCALING - L9", VDP_VIDEO_MIXER_FEATURE_HIGH_QUALITY_SCALING_L9},
  _mixer_features_maxnull
}VDP_mixer_features_e;

typedef struct  {
  VDP_mixer_features_e  func;        //[in]
  VdpBool is_supported;             //[out]
  char* ret_info;                   //[out]  name
}VDP_mixer_features_t;


typedef enum  {
mixer_parameters_VIDEO_SURFACE_WIDTH = 0,   //     {"VIDEO_SURFACE_WIDTH", VDP_VIDEO_MIXER_PARAMETER_VIDEO_SURFACE_WIDTH, DT_UINT},
mixer_parameters_VIDEO_SURFACE_HEIGHT,   //     {"VIDEO_SURFACE_HEIGHT", VDP_VIDEO_MIXER_PARAMETER_VIDEO_SURFACE_HEIGHT, DT_UINT},
mixer_parameters_CHROMA_TYPE,   //     {"CHROMA_TYPE", VDP_VIDEO_MIXER_PARAMETER_CHROMA_TYPE, DT_NONE},
mixer_parameters_LAYERS,   //     {"LAYERS", VDP_VIDEO_MIXER_PARAMETER_LAYERS, DT_UINT},
_mixer_parameters_maxnull
}VDP_mixer_parameters_e;

typedef struct  {
  VDP_mixer_parameters_e  func;      //[in]
  VdpBool is_supported;             //[out]
  uint32_t minval;                  //[out]
  uint32_t maxval;                  //[out]
  char* ret_info;                   //[out]  name
}VDP_mixer_parameters_t;

typedef enum  {
mixer_attributes_BACKGROUND_COLOR = 0,    //     {"BACKGROUND_COLOR", VDP_VIDEO_MIXER_ATTRIBUTE_BACKGROUND_COLOR, DT_NONE},
mixer_attributes_CSC_MATRIX,              //     {"CSC_MATRIX", VDP_VIDEO_MIXER_ATTRIBUTE_CSC_MATRIX, DT_NONE},
mixer_attributes_NOISE_REDUCTION_LEVEL,   //     {"NOISE_REDUCTION_LEVEL", VDP_VIDEO_MIXER_ATTRIBUTE_NOISE_REDUCTION_LEVEL, DT_FLOAT},
mixer_attributes_SHARPNESS_LEVEL,         //     {"SHARPNESS_LEVEL", VDP_VIDEO_MIXER_ATTRIBUTE_SHARPNESS_LEVEL, DT_FLOAT},
mixer_attributes_LUMA_KEY_MIN_LUMA,       //     {"LUMA_KEY_MIN_LUMA", VDP_VIDEO_MIXER_ATTRIBUTE_LUMA_KEY_MIN_LUMA, DT_NONE},
mixer_attributes_LUMA_KEY_MAX_LUMA,       //     {"LUMA_KEY_MAX_LUMA", VDP_VIDEO_MIXER_ATTRIBUTE_LUMA_KEY_MAX_LUMA, DT_NONE},
_mixer_attributes_maxnull
}VDP_mixer_attributes_e;

typedef struct  {
  VDP_mixer_attributes_e  func;     //[in]
  VdpBool is_supported;             //[out]
  uint32_t minval;                  //[out]
  uint32_t maxval;                  //[out]
  char* ret_info;                   //[out]  name
}VDP_mixer_attributes_t;


typedef enum {
  UN_KNOW = 0 ,   //初始无效值　，可表示为不支持
  MPEG1 ,   
  MPEG2, 
  MPEG4,   
  H264,   
  VC1 ,      
  DIVX4 ,  
  DIVX5,    
  HEVC,   
  _MAXNULL        //最大值，为空，　取值应小于他，防止越界
}decoder_profile;


/******************* Decoder ***************  queryDecoderCaps(device);*/
typedef enum  {
  decoder_profiles_MPEG1 = 0 ,          //     {"MPEG1", VDP_DECODER_PROFILE_MPEG1},
  decoder_profiles_MPEG2_SIMPLE,        //     {"MPEG2_SIMPLE", VDP_DECODER_PROFILE_MPEG2_SIMPLE},
  decoder_profiles_MPEG2_MAIN ,         //     {"MPEG2_MAIN", VDP_DECODER_PROFILE_MPEG2_MAIN},
  decoder_profiles_H264_BASELINE,       //     {"H264_BASELINE", VDP_DECODER_PROFILE_H264_BASELINE},
  decoder_profiles_H264_MAIN,           //     {"H264_MAIN", VDP_DECODER_PROFILE_H264_MAIN},
  decoder_profiles_H264_HIGH,           //     {"H264_HIGH", VDP_DECODER_PROFILE_H264_HIGH},
  decoder_profiles_VC1_SIMPLE ,         //     {"VC1_SIMPLE", VDP_DECODER_PROFILE_VC1_SIMPLE},
  decoder_profiles_VC1_MAIN,            //     {"VC1_MAIN", VDP_DECODER_PROFILE_VC1_MAIN},
  decoder_profiles_VC1_ADVANCED ,       //     {"VC1_ADVANCED", VDP_DECODER_PROFILE_VC1_ADVANCED},
  decoder_profiles_MPEG4_PART2_SP,      //     {"MPEG4_PART2_SP", VDP_DECODER_PROFILE_MPEG4_PART2_SP},
  decoder_profiles_MPEG4_PART2_ASP,     //     {"MPEG4_PART2_ASP", VDP_DECODER_PROFILE_MPEG4_PART2_ASP},
  decoder_profiles_DIVX4_QMOBILE,       //     {"DIVX4_QMOBILE", VDP_DECODER_PROFILE_DIVX4_QMOBILE},
  decoder_profiles_DIVX4_MOBILE,        //     {"DIVX4_MOBILE", VDP_DECODER_PROFILE_DIVX4_MOBILE},
  decoder_profiles_DIVX4_HOME_THEATER , //     {"DIVX4_HOME_THEATER", VDP_DECODER_PROFILE_DIVX4_HOME_THEATER},
  decoder_profiles_DIVX4_HD_1080P ,     //     {"DIVX4_HD_1080P", VDP_DECODER_PROFILE_DIVX4_HD_1080P},
  decoder_profiles_DIVX5_QMOBILE,       //     {"DIVX5_QMOBILE", VDP_DECODER_PROFILE_DIVX5_QMOBILE},
  decoder_profiles_DIVX5_MOBILE ,       //     {"DIVX5_MOBILE", VDP_DECODER_PROFILE_DIVX5_MOBILE},
  decoder_profiles_DIVX5_HOME_THEATER,  //     {"DIVX5_HOME_THEATER", VDP_DECODER_PROFILE_DIVX5_HOME_THEATER},
  decoder_profiles_DIVX5_HD_1080P,      //     {"DIVX5_HD_1080P", VDP_DECODER_PROFILE_DIVX5_HD_1080P},
  decoder_profiles_H264_CONSTRAINED_BASELINE ,   //     {"H264_CONSTRAINED_BASELINE", VDP_DECODER_PROFILE_H264_CONSTRAINED_BASELINE},
  decoder_profiles_H264_EXTENDED ,                //     {"H264_EXTENDED", VDP_DECODER_PROFILE_H264_EXTENDED},
  decoder_profiles_H264_PROGRESSIVE_HIGH,         //     {"H264_PROGRESSIVE_HIGH", VDP_DECODER_PROFILE_H264_PROGRESSIVE_HIGH},
  decoder_profiles_H264_CONSTRAINED_HIGH,         //     {"H264_CONSTRAINED_HIGH", VDP_DECODER_PROFILE_H264_CONSTRAINED_HIGH},
  decoder_profiles_H264_HIGH_444_PREDICTIVE,      //     {"H264_HIGH_444_PREDICTIVE", VDP_DECODER_PROFILE_H264_HIGH_444_PREDICTIVE},
  decoder_profiles_HEVC_MAIN ,                    //     {"HEVC_MAIN", VDP_DECODER_PROFILE_HEVC_MAIN},
  decoder_profiles_HEVC_MAIN_10,                  //     {"HEVC_MAIN_10", VDP_DECODER_PROFILE_HEVC_MAIN_10},
  decoder_profiles_HEVC_MAIN_STILL,               //     {"HEVC_MAIN_STILL", VDP_DECODER_PROFILE_HEVC_MAIN_STILL},
  decoder_profiles_HEVC_MAIN_12,                  //     {"HEVC_MAIN_12", VDP_DECODER_PROFILE_HEVC_MAIN_12},
  decoder_profiles_HEVC_MAIN_444,                 //     {"HEVC_MAIN_444", VDP_DECODER_PROFILE_HEVC_MAIN_444},
  _decoder_maxnull
}VDP_Decoder_e;


typedef struct  {
  VDP_Decoder_e  func;              //[in]  the decoder profiles
  VdpBool is_supported;             //[out] the support result
  uint32_t max_width;               //[out] the max  width
  uint32_t max_height;              //[out] the max  height
  uint32_t max_level;               //[out] the max profiles level
  uint32_t max_macroblocks;         //[out] the max  macroblocks
  char ret_info[RET_INFO_LENTH_MAX]; //[out]  name
}VDP_Decoder_t;

unsigned int vdp_Iter_decoderInfo(decoder_profile index, VDP_Decoder_t *result ); //in [index]  out  返回　能支持的个数，为０则不支持

/**
 * @brief Get vo config for current GPU
 * @return vo config string, NULL if not configured
 */
const char* gpuinfo_get_vo(void);

#ifdef __cplusplus
}
#endif

#endif  //ENDOF   #define _LIBGPUINFO_DECODER_H
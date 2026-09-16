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

#include <assert.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include <va/va_str.h>
#include "../vainfo/va_display.h"


#define str(a) #a
const char * const dec_str[] = {
	str(_NULL),      //初始无效值　，可表示为不支持
	str(MPEG1),
	str(MPEG2),
	str(MPEG4),
	str(H264),
	str(VC1),
	str(DIVX4),
	str(DIVX5),
 	str(HEVC),
	str(UN_KNOW_DEC),    //最大值，为空，　取值应小于他，防止越界
};


/******************* Decoder ****************/
decoder_t decoder_2profiles[] = {
    {str(MPEG1), "MPEG1",         VDP_DECODER_PROFILE_MPEG1},
    {str(MPEG2), "MPEG2_SIMPLE",  VDP_DECODER_PROFILE_MPEG2_SIMPLE},
    {str(MPEG2), "MPEG2_MAIN",    VDP_DECODER_PROFILE_MPEG2_MAIN},
    {str(H264),  "H264_BASELINE", VDP_DECODER_PROFILE_H264_BASELINE},
    {str(H264),  "H264_MAIN",     VDP_DECODER_PROFILE_H264_MAIN},
    {str(H264),  "H264_HIGH",     VDP_DECODER_PROFILE_H264_HIGH},
    {str(VC1),   "VC1_SIMPLE",    VDP_DECODER_PROFILE_VC1_SIMPLE},
    {str(VC1),   "VC1_MAIN",      VDP_DECODER_PROFILE_VC1_MAIN},
    {str(VC1),   "VC1_ADVANCED",  VDP_DECODER_PROFILE_VC1_ADVANCED},
    {str(MPEG4), "MPEG4_PART2_SP",    VDP_DECODER_PROFILE_MPEG4_PART2_SP},
    {str(MPEG4), "MPEG4_PART2_ASP",   VDP_DECODER_PROFILE_MPEG4_PART2_ASP},
    {str(DIVX4), "DIVX4_QMOBILE",     VDP_DECODER_PROFILE_DIVX4_QMOBILE},
    {str(DIVX4), "DIVX4_MOBILE",      VDP_DECODER_PROFILE_DIVX4_MOBILE},
    {str(DIVX4), "DIVX4_HOME_THEATER",    VDP_DECODER_PROFILE_DIVX4_HOME_THEATER},
    {str(DIVX4), "DIVX4_HD_1080P",        VDP_DECODER_PROFILE_DIVX4_HD_1080P},
    {str(DIVX5), "DIVX5_QMOBILE",         VDP_DECODER_PROFILE_DIVX5_QMOBILE},
    {str(DIVX5), "DIVX5_MOBILE",          VDP_DECODER_PROFILE_DIVX5_MOBILE},
    {str(DIVX5), "DIVX5_HOME_THEATER",    VDP_DECODER_PROFILE_DIVX5_HOME_THEATER},
    {str(DIVX5), "DIVX5_HD_1080P",        VDP_DECODER_PROFILE_DIVX5_HD_1080P},
    {str(H264),  "H264_CONSTRAINED_BASELINE",  VDP_DECODER_PROFILE_H264_CONSTRAINED_BASELINE},
    {str(H264),  "H264_EXTENDED",           VDP_DECODER_PROFILE_H264_EXTENDED},
    {str(H264),  "H264_PROGRESSIVE_HIGH",   VDP_DECODER_PROFILE_H264_PROGRESSIVE_HIGH},
    {str(H264),  "H264_CONSTRAINED_HIGH",   VDP_DECODER_PROFILE_H264_CONSTRAINED_HIGH},
    {str(H264),  "H264_HIGH_444_PREDICTIVE",VDP_DECODER_PROFILE_H264_HIGH_444_PREDICTIVE},
    {str(HEVC),  "HEVC_MAIN",               VDP_DECODER_PROFILE_HEVC_MAIN},
    {str(HEVC),  "HEVC_MAIN_10",            VDP_DECODER_PROFILE_HEVC_MAIN_10},
    {str(HEVC),  "HEVC_MAIN_STILL",         VDP_DECODER_PROFILE_HEVC_MAIN_STILL},
    {str(HEVC),  "HEVC_MAIN_12",            VDP_DECODER_PROFILE_HEVC_MAIN_12},
    {str(HEVC),  "HEVC_MAIN_444",           VDP_DECODER_PROFILE_HEVC_MAIN_444},
    {str(HEVC),  "HEVC_MAIN_444",           VDP_DECODER_PROFILE_HEVC_MAIN_444},
    {str(UN_KNOW_DEC), "UN_KNOW_DECODER",   0xffff},      //  }
};

const size_t decoder_profile_count  = sizeof(decoder_2profiles) / sizeof(decoder_t);

#define TO_VDP(e_VACase,e_VDPCase) case e_VACase: return e_VDPCase

VDP_Decoder_e va_to_VDP_Profile(VAProfile profile)
{
    switch (profile) {
        TO_VDP(VAProfileNone,  _decoder_maxnull);
        TO_VDP(VAProfileMPEG2Simple,  decoder_profiles_MPEG2_SIMPLE);
        TO_VDP(VAProfileMPEG2Main,  decoder_profiles_MPEG2_MAIN);
        TO_VDP(VAProfileMPEG4Simple,  decoder_profiles_MPEG4_PART2_SP);
        TO_VDP(VAProfileMPEG4AdvancedSimple,  _decoder_maxnull);
        TO_VDP(VAProfileMPEG4Main,  decoder_profiles_MPEG4_PART2_ASP);
        TO_VDP(VAProfileH264Main,  decoder_profiles_H264_MAIN);
        TO_VDP(VAProfileH264High,  decoder_profiles_H264_HIGH);
        TO_VDP(VAProfileVC1Simple,  decoder_profiles_VC1_SIMPLE);
        TO_VDP(VAProfileVC1Main,  decoder_profiles_VC1_MAIN);
        TO_VDP(VAProfileVC1Advanced,  decoder_profiles_VC1_ADVANCED);
        TO_VDP(VAProfileH263Baseline,  _decoder_maxnull);
        TO_VDP(VAProfileH264ConstrainedBaseline,  decoder_profiles_H264_CONSTRAINED_BASELINE);
        TO_VDP(VAProfileJPEGBaseline,  _decoder_maxnull);
        TO_VDP(VAProfileVP8Version0_3,  _decoder_maxnull);
        TO_VDP(VAProfileH264MultiviewHigh,  decoder_profiles_H264_PROGRESSIVE_HIGH);
        TO_VDP(VAProfileH264StereoHigh,  decoder_profiles_H264_CONSTRAINED_HIGH);
        TO_VDP(VAProfileHEVCMain,  decoder_profiles_HEVC_MAIN_STILL);
        TO_VDP(VAProfileHEVCMain10,  decoder_profiles_HEVC_MAIN_STILL);
        TO_VDP(VAProfileVP9Profile0,  _decoder_maxnull);
        TO_VDP(VAProfileVP9Profile1,  _decoder_maxnull);
        TO_VDP(VAProfileVP9Profile2,  _decoder_maxnull);
        TO_VDP(VAProfileVP9Profile3,  _decoder_maxnull);
        TO_VDP(VAProfileHEVCMain12,  decoder_profiles_HEVC_MAIN_12);
        TO_VDP(VAProfileHEVCMain422_10,  _decoder_maxnull);
        TO_VDP(VAProfileHEVCMain422_12,  _decoder_maxnull);
        TO_VDP(VAProfileHEVCMain444,  _decoder_maxnull);
        TO_VDP(VAProfileHEVCMain444_10,  _decoder_maxnull);
        TO_VDP(VAProfileHEVCMain444_12,  _decoder_maxnull);
        TO_VDP(VAProfileHEVCSccMain,  decoder_profiles_HEVC_MAIN);
        TO_VDP(VAProfileHEVCSccMain10,  decoder_profiles_HEVC_MAIN_10);
        TO_VDP(VAProfileHEVCSccMain444,  decoder_profiles_HEVC_MAIN_444);
        // TO_VDP(VAProfileAV1Profile0,  _decoder_maxnull);
        // TO_VDP(VAProfileAV1Profile1,  _decoder_maxnull);
        // TO_VDP(VAProfileHEVCSccMain444_10,  _decoder_maxnull);
        // TO_VDP(VAProfileProtected,  _decoder_maxnull);
    default:
        break;
    }
    return _decoder_maxnull;
}



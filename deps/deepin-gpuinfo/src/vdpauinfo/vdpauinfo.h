/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  vdpauinfo.h
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

#ifndef _VDPAUINFO_H
#define _VDPAUINFO_H  1

#include <vdpau/vdpau.h>
#include <vdpau/vdpau_x11.h>
#include <string.h>
#include <stdbool.h>

/******************* Video mixer ****************/

/* Type for value ranges */
enum DataType
{
  DT_NONE,
  DT_INT,
  DT_UINT,
  DT_FLOAT
};


typedef struct VDPDeviceImpl {
    VdpGetProcAddress *get_proc_address; //VDPDeviceImpl(VdpDevice device, VdpGetProcAddress *get_proc_address);
    VdpDevice device;    
    VdpGetErrorString *GetErrorString;
    VdpGetProcAddress *GetProcAddress;  
    VdpGetApiVersion *GetApiVersion;
    VdpGetInformationString *GetInformationString;
    VdpDeviceDestroy *DeviceDestroy;
    VdpGenerateCSCMatrix *GenerateCSCMatrix;
    VdpVideoSurfaceQueryCapabilities *VideoSurfaceQueryCapabilities;
    VdpVideoSurfaceQueryGetPutBitsYCbCrCapabilities *VideoSurfaceQueryGetPutBitsYCbCrCapabilities;
    VdpVideoSurfaceCreate *VideoSurfaceCreate;
    VdpVideoSurfaceDestroy *VideoSurfaceDestroy;
    VdpVideoSurfaceGetParameters *VideoSurfaceGetParameters;
    VdpVideoSurfaceGetBitsYCbCr *VideoSurfaceGetBitsYCbCr;
    VdpVideoSurfacePutBitsYCbCr *VideoSurfacePutBitsYCbCr;
    VdpOutputSurfaceQueryCapabilities *OutputSurfaceQueryCapabilities;
    VdpOutputSurfaceQueryGetPutBitsNativeCapabilities *OutputSurfaceQueryGetPutBitsNativeCapabilities;
    VdpOutputSurfaceQueryPutBitsIndexedCapabilities *OutputSurfaceQueryPutBitsIndexedCapabilities;
    VdpOutputSurfaceQueryPutBitsYCbCrCapabilities *OutputSurfaceQueryPutBitsYCbCrCapabilities;
    VdpOutputSurfaceCreate *OutputSurfaceCreate;
    VdpOutputSurfaceDestroy *OutputSurfaceDestroy;
    VdpOutputSurfaceGetParameters *OutputSurfaceGetParameters;
    VdpOutputSurfaceGetBitsNative *OutputSurfaceGetBitsNative;
    VdpOutputSurfacePutBitsNative *OutputSurfacePutBitsNative;
    VdpOutputSurfacePutBitsIndexed *OutputSurfacePutBitsIndexed;
    VdpOutputSurfacePutBitsYCbCr *OutputSurfacePutBitsYCbCr;
    VdpBitmapSurfaceQueryCapabilities *BitmapSurfaceQueryCapabilities;
    VdpBitmapSurfaceCreate *BitmapSurfaceCreate;
    VdpBitmapSurfaceDestroy *BitmapSurfaceDestroy;
    VdpBitmapSurfaceGetParameters *BitmapSurfaceGetParameters;
    VdpBitmapSurfacePutBitsNative *BitmapSurfacePutBitsNative;
    VdpOutputSurfaceRenderOutputSurface *OutputSurfaceRenderOutputSurface;
    VdpOutputSurfaceRenderBitmapSurface *OutputSurfaceRenderBitmapSurface;
    VdpDecoderQueryCapabilities *DecoderQueryCapabilities;
    VdpDecoderCreate *DecoderCreate;
    VdpDecoderDestroy *DecoderDestroy;
    VdpDecoderGetParameters *DecoderGetParameters;
    VdpDecoderRender *DecoderRender;
    VdpVideoMixerQueryFeatureSupport *VideoMixerQueryFeatureSupport;
    VdpVideoMixerQueryParameterSupport *VideoMixerQueryParameterSupport;
    VdpVideoMixerQueryAttributeSupport *VideoMixerQueryAttributeSupport;
    VdpVideoMixerQueryParameterValueRange *VideoMixerQueryParameterValueRange;
    VdpVideoMixerQueryAttributeValueRange *VideoMixerQueryAttributeValueRange;
    VdpVideoMixerCreate *VideoMixerCreate;
    VdpVideoMixerSetFeatureEnables *VideoMixerSetFeatureEnables;
    VdpVideoMixerSetAttributeValues *VideoMixerSetAttributeValues;
    VdpVideoMixerGetFeatureSupport *VideoMixerGetFeatureSupport;
    VdpVideoMixerGetFeatureEnables *VideoMixerGetFeatureEnables;
    VdpVideoMixerGetParameterValues *VideoMixerGetParameterValues;
    VdpVideoMixerGetAttributeValues *VideoMixerGetAttributeValues;
    VdpVideoMixerDestroy *VideoMixerDestroy;
    VdpVideoMixerRender *VideoMixerRender;
    VdpPresentationQueueTargetDestroy *PresentationQueueTargetDestroy;
    VdpPresentationQueueCreate *PresentationQueueCreate;
    VdpPresentationQueueDestroy *PresentationQueueDestroy;
    VdpPresentationQueueSetBackgroundColor *PresentationQueueSetBackgroundColor;
    VdpPresentationQueueGetBackgroundColor *PresentationQueueGetBackgroundColor;
    VdpPresentationQueueGetTime *PresentationQueueGetTime;
    VdpPresentationQueueDisplay *PresentationQueueDisplay;
    VdpPresentationQueueBlockUntilSurfaceIdle *PresentationQueueBlockUntilSurfaceIdle;
    VdpPresentationQueueQuerySurfaceStatus *PresentationQueueQuerySurfaceStatus;
    VdpPreemptionCallbackRegister *PreemptionCallbackRegister;
    VdpPresentationQueueTargetCreateX11 *PresentationQueueTargetCreateX11;
}VDPDeviceImpl;


bool vdpauinfo_init(VDPDeviceImpl *hwctx);
uint8_t is_hwSupport(void);  //make sure call  sysfs_populate_entries() before

// extern unsigned int get_pcilist(struct _pci_st *stp);
/*
chroma_types[x].id, ycbcr_types[y].id, VideoSurfaceQueryCapabilities       is_support_queryVideoSurface
rgb_types[x].id, ycbcr_types[y].id, OutputSurfaceQueryPutBitsYCbCrCapabilities  is_support_ycbcr_OutputSurface



*/
#endif /* _VDPAUINFO_H */

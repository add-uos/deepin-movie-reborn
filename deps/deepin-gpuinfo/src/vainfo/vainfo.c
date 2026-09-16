/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  vainfo.c
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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <va/va_str.h>

#include "va_display.h"

//#ifdef ANDROID

/* Macros generated from configure */
// #define LIBVA_VERSION_S "2.4"

//#endif

#define CHECK_VASTATUS(va_status, func, ret)                                                             \
  if (va_status != VA_STATUS_SUCCESS)                                                                    \
  {                                                                                                      \
    fprintf(stderr, "%s failed with error code %d (%s),exit\n", func, va_status, vaErrorStr(va_status)); \
    ret_val = ret;                                                                                       \
    goto error;                                                                                          \
  }

//int va_main(int argc,  char* argv[])
int va_main(void)
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

  name = "deepin-gpuinfo";

  va_dpy = va_open_display();
  if (NULL == va_dpy)  {
    fprintf(stderr, "%s: vaGetDisplay() failed\n", name);
    return 2;
  }

  va_status = vaInitialize(va_dpy, &major_version, &minor_version);
  CHECK_VASTATUS(va_status, "vaInitialize", 3);
  printf("%s: VA-API version: %d.%d \n", name, major_version, minor_version);

  driver = vaQueryVendorString(va_dpy);
  printf("%s: va Driver version: %s\n", name, driver ? driver : "<unknown>");

  num_entrypoint = vaMaxNumEntrypoints(va_dpy);
  entrypoints = malloc(num_entrypoint * sizeof(VAEntrypoint));
  if (!entrypoints)  {
    printf("Failed to allocate memory for entrypoint list\n");
    ret_val = -1;
    goto error;
  }

  printf("%s: Supported profile and entrypoints\n", name);
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
    
    va_status = vaQueryConfigEntrypoints(va_dpy, profile, entrypoints, &num_entrypoint);
    if (va_status == VA_STATUS_ERROR_UNSUPPORTED_PROFILE)
      continue;

    CHECK_VASTATUS(va_status, "vaQueryConfigEntrypoints", 4);
  // printf("   %-32s <-->	%d ,", vaProfileStr(profile), i);
   for (entrypoint = 0; entrypoint < num_entrypoint; entrypoint++)   
    {
     printf("	%d,   %-32s <-->	%-21s ,", i, vaProfileStr(profile), vaEntrypointStr(entrypoints[entrypoint]));
      

          VASurfaceAttrib *surface_attribs;
          unsigned int num_image_formats, num_surface_attribs;
          VAConfigID vpp_config_id = VA_INVALID_ID;
          VASurfaceAttrib *va_surface_attribs;
          int va_num_surface_attribs = -1;

          // if (va_num_surface_attribs >= 0)
          //     return va_num_surface_attribs;

          num_image_formats = vaMaxNumImageFormats(va_dpy);
          if (num_image_formats == 0)
              return 0;

          va_status = vaCreateConfig(va_dpy, profile,entrypoints[entrypoint],
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
          // printf("num_surface_attribs : %d ,  num_image_formats: %d ",
          // num_surface_attribs,num_image_formats);

          for (int i = 0; i < va_num_surface_attribs; i++) {
              const VASurfaceAttrib * const surface_attrib = &va_surface_attribs[i];
              if (surface_attrib->type == VASurfaceAttribMaxWidth) 
                  printf("MaxWidth: %-5d ,",   surface_attrib->value.value.i );
              if (surface_attrib->type == VASurfaceAttribMaxHeight) 
                  printf("MaxHeight: %-5d ,", surface_attrib->value.value.i );
              if (surface_attrib->type == VASurfaceAttribMinWidth) 
                  printf("MinHeight: %-5d ,", surface_attrib->value.value.i );
              if (surface_attrib->type == VASurfaceAttribMinHeight) 
                  printf("MinHeight: %-5d ,", surface_attrib->value.value.i );   
              // if (surface_attrib->type == VASurfaceAttribPixelFormat) 
              //     printf("%s ,", map_vafourcc_to_str(surface_attrib->value.value.i) );                  

          }
          printf("\n");
                
    } //for(entrypoint....)
  }

error:
  free(entrypoints);
  free(profile_list);
  vaTerminate(va_dpy);
  va_close_display(va_dpy);

  return ret_val;
}


int va_getMaxNumProfile(void)
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

  // name = "deepin-gpuinfo";

  va_dpy = va_open_display();
  if (NULL == va_dpy)  {  
    return 0;
  }

  va_status = vaInitialize(va_dpy, &major_version, &minor_version);
  CHECK_VASTATUS(va_status, "vaInitialize", 3);
  // printf("%s: VA-API version: %d.%d (libva %s)\n", name, major_version, minor_version, LIBVA_VERSION_S);

  driver = vaQueryVendorString(va_dpy);
  num_entrypoint = vaMaxNumEntrypoints(va_dpy);

  error:
  return(num_entrypoint);
}

int vainfo_getNameProfile(char * va_support_profile)
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
   char tmpc[2] = ",";

  // name = "deepin-gpuinfo";

  va_dpy = va_open_display();
  if (NULL == va_dpy)  {  
    return 2;
  }

  va_status = vaInitialize(va_dpy, &major_version, &minor_version);
  CHECK_VASTATUS(va_status, "vaInitialize", 3);
  // printf("%s: VA-API version: %d.%d (libva %s)\n", name, major_version, minor_version, LIBVA_VERSION_S);

  driver = vaQueryVendorString(va_dpy);


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
    // printf("Failed to allocate memory for profile list\n");
    ret_val = 5;
    goto error;
  }

  va_status = vaQueryConfigProfiles(va_dpy, profile_list, &num_profiles);
  CHECK_VASTATUS(va_status, "vaQueryConfigProfiles", 6);

  for (i = 0; i < num_profiles; i++)
  {
    profile = profile_list[i];
    va_status = vaQueryConfigEntrypoints(va_dpy, profile, entrypoints, &num_entrypoint);
    if (va_status == VA_STATUS_ERROR_UNSUPPORTED_PROFILE)
      continue;

    CHECK_VASTATUS(va_status, "vaQueryConfigEntrypoints", 4);

    for (entrypoint = 0; entrypoint < num_entrypoint; entrypoint++)
    {
      printf("      %-32s:	%s\n", vaProfileStr(profile), vaEntrypointStr(entrypoints[entrypoint]));
      strcpy(va_support_profile, vaProfileStr(profile) );
      strcat(va_support_profile, tmpc);
    }
  }

error:
  free(entrypoints);
  free(profile_list);
  vaTerminate(va_dpy);
  va_close_display(va_dpy);

  return ret_val;
}


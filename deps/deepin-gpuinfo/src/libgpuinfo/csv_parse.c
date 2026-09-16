/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  main_test.c
 * Author:     shuaijie <shuaijie@uniontech.com> 
 * Maintainer: shuaijie <shuaijie@uniontech.com>
 * descrition:  This file is part of test libgpuinfo. 
 *
 * This program is free software: you can redistribute it and/or modify,it underthe terms  of the GNU General Public 
 *  License as published by the Free Software Foundation, either version 3 of the License, or any later version. 
 *
 * This program is distributed in the hope that it will be useful,but WITHOUT ANY WARRANTY; without even  the implied warranty 
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.You should have
 * received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// #include "./usr/include/libgpuinfo/libgpuinfo_decoder.h"
//  gcc main_test.c -g -o mtest -ldl 
#include <getopt.h>

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <dlfcn.h>
#include <limits.h>
#include <string.h>


#include <dirent.h>

#include "config.h"
// char* get_field(char *line, int num);
// char* remove_quoted(char *str);




# define COLUM_MAX  ( 9 )

// Helper: find csv file from XDG_DATA_DIRS
static FILE* open_gpuinfo_csv(void)
{
    FILE *fp = NULL;
    const char *xdg_data_dirs;
    char path[PATH_MAX];
    char *dirs, *dir, *saveptr;

    fp = fopen("../config/gpuinfolib.csv", "r");
    if (fp) { printf("[open_gpuinfo_csv] using: ../config/gpuinfolib.csv\n"); return fp; }

    xdg_data_dirs = getenv("XDG_DATA_DIRS");
    if (xdg_data_dirs) {
        dirs = strdup(xdg_data_dirs);
        if (dirs) {
            dir = strtok_r(dirs, ":", &saveptr);
            while (dir != NULL) {
                snprintf(path, sizeof(path), "%s/deepin-gpuinfo/gpuinfolib.csv", dir);
                fp = fopen(path, "r");
                if (fp) {
                    printf("[open_gpuinfo_csv] using: %s\n", path);
                    free(dirs);
                    return fp;
                }
                dir = strtok_r(NULL, ":", &saveptr);
            }
            free(dirs);
        }
    }

    fp = fopen("/usr/local/share/deepin-gpuinfo/gpuinfolib.csv", "r");
    if (fp) { printf("[open_gpuinfo_csv-fix] using: /usr/local/share/deepin-gpuinfo/gpuinfolib.csv\n"); return fp; }

    fp = fopen("/usr/share/deepin-gpuinfo/gpuinfolib.csv", "r");
    if (fp) { printf("[open_gpuinfo_csv-fix] using: /usr/share/deepin-gpuinfo/gpuinfolib.csv\n"); return fp; }

    fp = fopen("/opt/apps/org.deepin.movie/files/share/deepin-gpuinfo/gpuinfolib.csv", "r");
    if (fp) { printf("[open_gpuinfo_csv-fix] using: /opt/apps/org.deepin.movie/files/share/deepin-gpuinfo/gpuinfolib.csv\n"); return fp; }

    printf("[open_gpuinfo_csv] no csv file found\n");
    return fp;
}


char* remove_quoted(char *str)
{
    int length = strlen(str);
    char *result = malloc(length + 1);
    int index = 0;
    for (int i = 0; i < length; i++) {
        if (str[i] != '\"') {
            result[index] = str[i];
            index++;
        }
    }
    result[index] = '\0';
    return result;
}

char* get_field(char *line, int num)
{
    char *tok;
    tok = strtok(line, ",");
    for (int i = 1; i != num; i++) {
        tok = strtok(NULL, ",");
    }
	if(tok != NULL)
	{
		char *result = remove_quoted(tok);
		return result;
	}
	return NULL;
}

struct _gpuinfo {
    char *pci_vid_pid;
    char *chip_name;
    char *device_name;
	char *vendor_name;
	char *class_id;
	char *sub_vid_pid;
	char *driver_name;
	char *codec_support;
	char *opengl_renderer;  // 新增：用于 OpenGL renderer 匹配
};

enum {
	epci_vid_pid = 1,
	echip_name,
	edevice_name,
	evendor_name,
	eclass_id,
	esub_vid_pid,
	edriver_name,
	ecodec_support,
	estream_format,
	eapi_support,
	evo,
	eopengl_renderer,  // 新增：第12列
	eColumMax
};


// #define INFOID_PATH DATADIR"/usr/share/deepin-gpuinfo/gpuinfolib.csv"
// parse the VAAPI VDPAU API support status

char*  csv_search(const char* ss)
{
	FILE *fp = NULL;
	char* ret = NULL;
	char *line,*record;
	char buffer[1024];

	fp = open_gpuinfo_csv();
	if (fp != NULL)
	{
		char delims[] = ",";
		char *result = NULL;
		int j = 0;
		// fseek(fp, 170L, SEEK_SET);  //定位到第二行，每个英文字符大小为1
		struct _gpuinfo gpuinfo;
		while ((line = fgets(buffer, sizeof(buffer), fp))!=NULL)//当没有读取到文件末尾时循环继续
		{
			gpuinfo.pci_vid_pid = get_field(strdup(buffer), epci_vid_pid);
			gpuinfo.chip_name = get_field(strdup(buffer), echip_name);
			gpuinfo.device_name = get_field(strdup(buffer), edevice_name);
			gpuinfo.class_id = get_field(strdup(buffer), eclass_id);
			gpuinfo.driver_name = get_field(strdup(buffer), edriver_name);
			gpuinfo.codec_support = get_field(strdup(buffer), ecodec_support);
			// printf("%s\t%s\t%s\t%s\t%s\t%s\n", gpuinfo.pci_vid_pid, gpuinfo.chip_name,
			// 								   gpuinfo.device_name,gpuinfo.class_id,
			// 								   gpuinfo.driver_name,gpuinfo.codec_support);
			if(0 == strcmp(ss, gpuinfo.pci_vid_pid))
			{
				//printf("gpuinfo.pci_vid_pid \n");
				ret =  gpuinfo.codec_support;
				break;
			}
		}
		fclose(fp);
		fp = NULL;
	}
	return ret;
}

// Search vo config from csv by pci id
char* csv_search_vo(const char* ss)
{
	FILE *fp = NULL;
	char* ret = NULL;
	char buffer[1024];

	fp = open_gpuinfo_csv();
	if (fp != NULL) {
		while (fgets(buffer, sizeof(buffer), fp) != NULL) {
			if (buffer[0] == '#' || buffer[0] == ';') continue;
			char* vid_pid = get_field(strdup(buffer), epci_vid_pid);
			if (vid_pid && strcmp(vid_pid, ss) == 0) {
				ret = get_field(strdup(buffer), evo);
				break;
			}
		}
		fclose(fp);
	}
	return ret;
}

// Helper function: search field by opengl_renderer string
static char* csv_search_field_by_opengl(const char* opengl_renderer, int field_index)
{
	FILE *fp = NULL;
	char* ret = NULL;
	char buffer[1024];

	if (opengl_renderer == NULL) return NULL;

	fp = open_gpuinfo_csv();
	if (fp != NULL) {
		while (fgets(buffer, sizeof(buffer), fp) != NULL) {
			if (buffer[0] == '#' || buffer[0] == ';') continue;
			char* csv_opengl = get_field(strdup(buffer), eopengl_renderer);
			if (csv_opengl &&
			    strcmp(csv_opengl, "END") != 0 &&
			    strstr(opengl_renderer, csv_opengl)) {
				ret = get_field(strdup(buffer), field_index);
				free(csv_opengl);
				break;
			}
			if (csv_opengl) free(csv_opengl);
		}
		fclose(fp);
	}
	return ret;
}

// Search codec_support by opengl_renderer string
char* csv_search_by_opengl(const char* opengl_renderer)
{
	return csv_search_field_by_opengl(opengl_renderer, ecodec_support);
}

// Search vo config by opengl_renderer string
char* csv_search_vo_by_opengl(const char* opengl_renderer)
{
	return csv_search_field_by_opengl(opengl_renderer, evo);
}

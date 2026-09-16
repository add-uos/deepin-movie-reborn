/* Copyright (C) 2020 ~ 2026 Uniontech Software Technology Co., Ltd.
 * file name:  glxinfo_detect.c
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "glxinfo_detect.h"

#define MAX_INFO_LEN 256

static char g_vendor[MAX_INFO_LEN] = {0};
static char g_renderer[MAX_INFO_LEN] = {0};
static char g_version[MAX_INFO_LEN] = {0};
static int g_detected = 0;

// 解析 glxinfo -B 输出
static int parse_glxinfo_output(void)
{
    FILE *fp = NULL;
    char buffer[512];

    fp = popen("glxinfo -B 2>/dev/null", "r");
    if (fp == NULL) {
        return -1;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        // 解析 OpenGL vendor string
        if (strncmp(buffer, "OpenGL vendor string:", 20) == 0) {
            char *start = buffer + 21;
            while (*start == ' ') start++;
            char *end = start + strlen(start) - 1;
            while (end > start && (*end == '\n' || *end == '\r')) *end-- = '\0';
            strncpy(g_vendor, start, MAX_INFO_LEN - 1);
        }
        // 解析 OpenGL renderer string
        else if (strncmp(buffer, "OpenGL renderer string:", 22) == 0) {
            char *start = buffer + 23;
            while (*start == ' ') start++;
            char *end = start + strlen(start) - 1;
            while (end > start && (*end == '\n' || *end == '\r')) *end-- = '\0';
            strncpy(g_renderer, start, MAX_INFO_LEN - 1);
        }
        // 解析 OpenGL version string
        else if (strncmp(buffer, "OpenGL version string:", 21) == 0) {
            char *start = buffer + 22;
            while (*start == ' ') start++;
            char *end = start + strlen(start) - 1;
            while (end > start && (*end == '\n' || *end == '\r')) *end-- = '\0';
            strncpy(g_version, start, MAX_INFO_LEN - 1);
        }
    }

    pclose(fp);

    // 判断是否检测成功
    if (g_vendor[0] != '\0' || g_renderer[0] != '\0') {
        g_detected = 1;
        return 0;
    }

    return -1;
}

int glxinfo_detect_gpu(void)
{
    if (g_detected) {
        return 1;
    }
    return parse_glxinfo_output() == 0 ? 1 : 0;
}

const char* glxinfo_get_renderer(void)
{
    if (!g_detected) {
        parse_glxinfo_output();
    }
    return g_renderer[0] != '\0' ? g_renderer : NULL;
}

const char* glxinfo_get_vendor(void)
{
    if (!g_detected) {
        parse_glxinfo_output();
    }
    return g_vendor[0] != '\0' ? g_vendor : NULL;
}

const char* glxinfo_get_version(void)
{
    if (!g_detected) {
        parse_glxinfo_output();
    }
    return g_version[0] != '\0' ? g_version : NULL;
}

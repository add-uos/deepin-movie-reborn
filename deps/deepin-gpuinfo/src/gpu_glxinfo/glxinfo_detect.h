/* Copyright (C) 2020 ~ 2026 Uniontech Software Technology Co., Ltd.
 * file name:  glxinfo_detect.h
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

#ifndef _GLXINFO_DETECT_H_
#define _GLXINFO_DETECT_H_

#ifdef __cplusplus
extern "C" {
#endif

// 通过 glxinfo 检测 GPU 信息，返回检测到的 GPU 数量
int glxinfo_detect_gpu(void);

// 获取 OpenGL renderer string
const char* glxinfo_get_renderer(void);

// 获取 OpenGL vendor string
const char* glxinfo_get_vendor(void);

// 获取 OpenGL version string
const char* glxinfo_get_version(void);

#ifdef __cplusplus
}
#endif

#endif

/* Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co., Ltd.
 * file name:  va_display.h
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

#ifndef VA_DISPLAY_H
#define VA_DISPLAY_H

#include <va/va.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *name;
    VADisplay (*open_display)   (void);
    void      (*close_display)  (VADisplay va_dpy);
    VAStatus  (*put_surface)    (VADisplay va_dpy, VASurfaceID surface,
                                 const VARectangle *src_rect,
                                 const VARectangle *dst_rect);
} VADisplayHooks;

void
va_init_display_args(int *argc, char *argv[]);

VADisplay
va_open_display(void);

void
va_close_display(VADisplay va_dpy);

VAStatus
va_put_surface(
    VADisplay          va_dpy,
    VASurfaceID        surface,
    const VARectangle *src_rect,
    const VARectangle *dst_rect
);

void
va_print_display_options(FILE *stream);

#ifdef __cplusplus
}
#endif

#endif /* VA_DISPLAY_H */

/***************************************************************************
*    Copyright (c) 2013, Broadcom Corporation
*    All rights reserved.
*
*  Statement regarding contribution of copyrighted materials to VESA:
*
*  This code is owned by Broadcom Corporation and is contributed to VESA
*  for inclusion and use in its VESA Display Stream Compression specification.
*  Accordingly, VESA is hereby granted a worldwide, perpetual, non-exclusive
*  license to revise, modify and create derivative works to this code and
*  VESA shall own all right, title and interest in and to any derivative 
*  works authored by VESA.
*
*  Terms and Conditions
*
*  Without limiting the foregoing, you agree that your use
*  of this software program does not convey any rights to you in any of
*  Broadcom’s patent and other intellectual property, and you
*  acknowledge that your use of this software may require that
*  you separately obtain patent or other intellectual property
*  rights from Broadcom or third parties.
*
*  Except as expressly set forth in a separate written license agreement
*  between you and Broadcom, if applicable:
*
*  1. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
*  "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
*  REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
*  OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
*  DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
*  NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
*  ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
*  CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
*  OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
* 
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
*  BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
*  SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
*  IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*  IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
*  ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
*  OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
*  NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
***************************************************************************/
#ifndef VDO_H
#define VDO_H

#include <math.h> /* for fmod(3) */

#ifdef _MSC_VER
#ifndef _DEPRECATION_DISABLE
#define _DEPRECATION_DISABLE
#if (_MSC_VER >= 1400)
#pragma warning(disable: 4996)
#endif
#endif
//#define _CRT_SECURE_NO_WARNINGS
#endif

#define CLAMP(X, MIN, MAX) ((X)>(MAX) ? (MAX) : ((X)<(MIN) ? (MIN) : (X)))
#define MAX(X, Y) ((X)>(Y) ? (X) : (Y))
#define MIN(X, Y) ((X)<(Y) ? (X) : (Y))
#define ABS(X) ((X)<0 ? (-1*(X)) : (X))
#define FMOD(X,Y) fmod(fmod(X,Y)+Y,Y)

typedef enum format_e {FRAME, TOP, BOTTOM,                     UNDEFINED_FORMAT} format_t;
typedef enum color_e  {RGB, YUV_SD, YUV_HD,                    UNDEFINED_COLOR}  color_t;
typedef enum chroma_e {YUV_420, YUV_422, YUV_444, YUV_4444,    UNDEFINED_CHROMA} chroma_t;

typedef struct yuv_s {
    int **y;
    int **u;
    int **v;
    int **a;
    int **m;
} yuv_t;

typedef struct rgb_s {
    int **r;
    int **g;
    int **b;
    int **a;
    int **m;
} rgb_t;

typedef struct pic_s {
    format_t format;
    color_t  color;
    chroma_t chroma;
    int      alpha; // 0: not used, 1: used (same resolution as Y)
    int      w;
    int      h;
    int      bits;
    int      ar1;     // aspect ratio (h)
    int      ar2;     // aspect ratio (w)
    int      frm_no;  // frame number in seq
    int      seq_len; // num images in sequence
    float    framerate;
    int      interlaced; // prog(0) or int(1) content

    union data_u {
        rgb_t rgb;
        yuv_t yuv;
    } data;

} pic_t;

typedef struct pix_s {
    int r; // R or V
    int g; // G or Y
    int b; // B or U
    int a; // alpha
} pix_t;

//
// Data type for a polyphase filter. In addition, it can be used as a 2D
// filter by using "tap" as the vertical dimension, and "sub" as the
// horizontal dimension.
//
typedef struct fir_s {
    int   tap;
    int   sub;
    float coeff[256];
} fir_t;


#endif


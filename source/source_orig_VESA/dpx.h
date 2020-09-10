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
#ifndef DPX_H
#define DPX_H


/*** dpx.h ***/
#include "vdo.h"

#define DPX_ERROR_UNRECOGNIZED_CHROMA  -1
#define DPX_ERROR_UNSUPPORTED_BPP      -2
#define DPX_ERROR_NOT_IMPLEMENTED      -3
#define DPX_ERROR_MISMATCH             -4
#define DPX_ERROR_BAD_FILENAME         -5
#define DPX_ERROR_CORRUPTED_FILE       -6
#define DPX_ERROR_MALLOC_FAIL          -7

enum DPX_VER_e { STD_DPX_VER = 0, DVS_DPX_VER = 1};

format_t determine_field_format(char* file_name);
int      ends_in_percentd(char* str, int length);

int      dpx_write(char *fname, pic_t *p, int pad_ends, int datum_order, int force_packing, int swaprb, int wbswap);
int      dpx_read(char *fname, pic_t **p, int pad_ends, int noswap, int datum_order, int swap_r_and_b);
void     set_dpx_colorspace(int color);

int      dpx_read_hl(char *fname, pic_t **p, int *high, int *low, int dpx_bugs, int noswap, int datum_order, int swap_r_and_b);

int      write_dpx_ver(char *fname, pic_t *p, int ar1, int ar2, int frameno, int seqlen, float framerate, int interlaced, int bpp, int ver, int pad_ends, int datum_order, int force_packing, int swaprb, int wbswap);
int      write_dpx(char *fname, pic_t *p, int ar1, int ar2, int frameno, int seqlen, float framerate, int interlaced, int bpp, int pad_ends, int datum_order, int force_packing, int swaprb, int wbswap);
int      read_dpx(char *fname, pic_t **p, int *ar1, int *ar2, int *frameno, int *seqlen, float *framerate, int *interlaced, int *bpp, int pad_ends, int noswap, int datum_order, int swap_r_and_b);

#endif

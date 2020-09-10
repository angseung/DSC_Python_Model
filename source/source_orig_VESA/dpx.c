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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "vdo.h"
#include "utl.h"
#include "dpx.h"
#include "cmd_parse.h"
#include "fifo.h"

#define BCMDPX_VERSION   "1.00"
#define BCMDPX_420CODE   104
#define READ_DPX_32(x)   (bswap ? (((((x) & 0xff) << 24) | (((x) & 0xff00) << 8) | (((x) & 0xff0000) >> 8) | ((x) >> 24))) : (x))
#define READ_DPX_16(x)   (bswap ? ((((x) & 0xff) << 8) | (((x) & 0xff00) >> 8)) : (x))
//#define SINGLE_BS(x)   data32b = *((DWORD *)(&(x)));  *((DWORD *)(&(x))) = READ_DPX_32(data32b);
//#define SINGLE_BS(x)   { char t; union { SINGLE s; BYTE b[4]; } d; d.s = (x); t=d.b[0]; d.b[0]=d.b[3]; d.b[3]=t; t=d.b[1]; d.b[1]=d.b[2]; d.b[2]=t; (x)=d.s; } 
#define SINGLE_BS(x)  if(bswap) { int o; char t; o=offsetof(DPXFILEFORMAT,x); t=f_ptr[o]; f_ptr[o]=f_ptr[o+3]; f_ptr[o+3]=t; t=f_ptr[o+1]; f_ptr[o+1]=f_ptr[o+2]; f_ptr[o+2]=t; }

#ifdef WIN32
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef float SINGLE;
#define snprintf sprintf_s
#else
#if (defined(SOLARIS) || defined(__sparc__))
typedef unsigned long DWORD;
typedef unsigned short WORD;
typedef unsigned char BYTE;
typedef float SINGLE;
#else
#if (defined(__linux__) || defined(__APPLE__))
#include <stdint.h>
typedef uint32_t DWORD;
typedef uint16_t WORD;
typedef uint8_t BYTE;
typedef float SINGLE;
#endif /* linux */
#endif /* !sparc */
#endif /* !win */

#define FILENAME_SIZE 99

format_t determine_field_format(char* file_name)
{
	char*    name   = chop_ext(file_name);
	int      leng   = strlen(name);
	char     let_c  = name[leng-1]; 
	char     num_c  = name[leng-2];
	format_t format = UNDEFINED_FORMAT;
	if ((let_c >= '0' && let_c <= '9') || ends_in_percentd(name, leng)) {
		format = FRAME;
	}
	else if ((num_c >= '0' && num_c <= '9') || ends_in_percentd(name, leng-1)) {
		format = ('a' == let_c) ? TOP
			: ('b' == let_c) ? BOTTOM
			: format;
	}
	free(name);
	return format;
}

int ends_in_percentd(char* str, int length)
{
	if ((length > 0) && ('d' == str[length-1])) {
		int i;
		for (i=length-2; i>=0; i-=1) {
			if ('%' == str[i]) {
				return 1;
			}
			if (('0' > str[i]) || ('9' < str[i])) {
				return 0;
			}
		}
	}
	return 0;
}

typedef struct _GenericFileHeader
{
	DWORD Magic;           /* Magic number */
	DWORD ImageOffset;     /* Offset to start of image data (bytes) */
	char  Version[8];      /* Version stamp of header format */
	DWORD FileSize;        /* Total DPX file size (bytes) */
	DWORD DittoKey;        /* Image content specifier */
	DWORD GenericSize;     /* Generic section header length (bytes) */
	DWORD IndustrySize;    /* Industry-specific header length (bytes) */
	DWORD UserSize;        /* User-defined data length (bytes) */
	char  FileName[FILENAME_SIZE+1];   /* Name of DPX file */
	char  TimeDate[24];    /* Time & date of file creation */
	char  Creator[100];    /* Name of file creator */
	char  Project[200];    /* Name of project */
	char  Copyright[200];  /* Copyright information */
	DWORD EncryptKey;      /* Encryption key */
	char  Reserved[104];   /* Padding */
} GENERICFILEHEADER;

typedef struct _ImageElement
{
	DWORD  DataSign;          /* Data sign extension */
	DWORD  LowData;           /* Reference low data code value */
	SINGLE LowQuantity;       /* reference low quantity represented */
	DWORD  HighData;          /* Reference high data code value */
	SINGLE HighQuantity;      /* reference high quantity represented */
	BYTE   Descriptor;        /* Descriptor for image element */
	BYTE   Transfer;          /* Transfer characteristics for element */
	BYTE   Colorimetric;      /* Colorimetric specification for element */
	BYTE   BitSize;           /* Bit size for element */
	WORD   Packing;           /* Packing for element */
	WORD   Encoding;          /* Encoding for element */
	DWORD  DataOffset;        /* Offset to data of element */
	DWORD  EndOfLinePadding;  /* End of line padding used in element */
	DWORD  EndOfImagePadding; /* End of image padding used in element */
	char   Description[32];   /* Description of element */
} IMAGEELEMENT; 

typedef struct _GenericImageHeader
{
	WORD         Orientation;        /* Image orientation */
	WORD         NumberElements;     /* Number of image elements */
	DWORD        PixelsPerLine;      /* Pixels per line */
	DWORD        LinesPerElement;    /* Lines per image element */
	IMAGEELEMENT ImageElement[8];
	BYTE         Reserved[52];       /* Padding */
} GENERICIMAGEHEADER;

typedef struct _GenericOrientationHeader
{
	DWORD  XOffset;         /* X offset */
	DWORD  YOffset;         /* Y offset */
	SINGLE XCenter;         /* X center */
	SINGLE YCenter;         /* Y center */
	DWORD  XOriginalSize;   /* X original size */
	DWORD  YOriginalSize;   /* Y original size */
	char   FileName[FILENAME_SIZE+1];   /* Source image file name */
	char   TimeDate[24];    /* Source image date & time */
	char   InputName[32];   /* Input device name */
	char   InputSN[32];     /* Input device serial number */
	WORD   Border[4];       /* Border validity (XL, XR, YT, YB) */
	DWORD  AspectRatio[2];  /* Pixel aspect ratio H:V */
	BYTE   Reserved[28];    /* Padding */
} GENERICORIENTATIONHEADER;

typedef struct _IndustryFilmInfoHeader
{
	char   FilmMfgId[2];      /* Film manufacturer ID code */
	char   FilmType[2];       /* Film type */
	char   Offset[2];         /* Offset in perfs */
	char   Prefix[6];         /* Prefix */
	char   Count[4];          /* Count */
	char   Format[32];        /* Format */
	DWORD  FramePosition;     /* Frame position in sequence */
	DWORD  SequenceLen;       /* Sequence length in frames */
	DWORD  HeldCount;         /* Held count */
	SINGLE FrameRate;         /* Frame rate of original in frames/sec */
	SINGLE ShutterAngle;      /* Shutter angle of camera in degrees */
	char   FrameId[32];       /* Frame identification */
	char   SlateInfo[100];    /* Slate information */
	BYTE   Reserved[56];      /* Padding */
} INDUSTRYFILMINFOHEADER;

typedef struct _IndustryTelevisionInfoHeader
{
	DWORD  TimeCode;         /* SMPTE time code */
	DWORD  UserBits;         /* SMPTE user bits */
	BYTE   Interlace;        /* Interlace */
	BYTE   FieldNumber;      /* Field number */
	BYTE   VideoSignal;      /* Video signal standard */
	BYTE   Padding;          /* Structure alignment padding */
	SINGLE HorzSampleRate;   /* Horizontal sample rate in Hz */
	SINGLE VertSampleRate;   /* Vertical sample rate in Hz */
	SINGLE FrameRate;        /* Temporal sampling rate or frame rate in Hz */
	SINGLE TimeOffset;       /* time offset from sync to first pixel */
	SINGLE Gamma;            /* gamma value */
	SINGLE BlackLevel;       /* Black level code value */
	SINGLE BlackGain;        /* Black gain */
	SINGLE Breakpoint;       /* Breakpoint */
	SINGLE WhiteLevel;       /* Reference white level code value */
	SINGLE IntegrationTimes; /* Integration time(s) */
	BYTE   Reserved[76];     /* Padding */
} INDUSTRYTELEVISIONINFOHEADER;

typedef struct _DpxFileFormat
{
	GENERICFILEHEADER            FileHeader;
	GENERICIMAGEHEADER           ImageHeader;
	GENERICORIENTATIONHEADER     OrientHeader;
	INDUSTRYFILMINFOHEADER       FilmHeader;
	INDUSTRYTELEVISIONINFOHEADER TvHeader;
} DPXFILEFORMAT;


static DWORD generate_timecode(int frameno, float framerate);
static int create_dpx_pic(pic_t **p, chroma_t chroma, color_t color, int w, int h, int bits);
static int read_dpx_image_data(FILE *fp, pic_t **p, int orientation, int sign, int bpp, int descriptor, int rle, int bugs, int w, int h, int bswap, int packing, int datum_order, int swap_r_and_b);


static color_t dpxcolor = 0;  /* implied init to 0 because global */

/* write_dpx() :
Currently supports bpp=8, 10, 12 and 16 (in 2.0/Brdm mode) */
int dpx_write(char *fname, pic_t *p, int pad_ends, int datum_order, int force_packing, int swaprb, int wbswap)
{
	return write_dpx_ver(fname, p, p->ar1, p->ar2, p->frm_no, p->seq_len, p->framerate, p->interlaced, p->bits, STD_DPX_VER, pad_ends, datum_order, force_packing, swaprb, wbswap);
}

int write_dpx(char *fname, pic_t *p, int ar1, int ar2, int frameno, int seqlen, float framerate, int interlaced, int bpp, int pad_ends, int datum_order, int force_packing, int swaprb, int wbswap)
{
	return write_dpx_ver(fname, p, ar1, ar2, frameno, seqlen, framerate, interlaced, bpp, STD_DPX_VER, pad_ends, datum_order, force_packing, swaprb, wbswap);
}

int write_dpx_ver(char *fname, pic_t *p, int ar1, int ar2, int frameno, int seqlen, float framerate, int interlaced, int bpp, int ver, int pad_ends, int datum_order, int force_packing, int swaprb, int wbswap)
{
	FILE *fp;
	int i, j, k, xindex, b;
	DPXFILEFORMAT f;
	int element;
	color_t c;
	int **datum[4][6];
	int nbuffer = 1;// Min = 1, Max = 0;
	int ndatum[4];
	int subsampled[4];
	int wbuff[4];
	int hbuff[4] = {0, 0, 0, 0};
	DWORD data32b = 0;
	char *f_ptr;
	int bswap;
	union
	{
		unsigned int ui;
		unsigned char c[4];
	} endiantest;
	fifo_t fifo;

	endiantest.ui = 0x12345678;
	if(endiantest.c[0] == 0x12)  // Big endian
		bswap = 0;
	else
		bswap = 1;  // Write headers as big-endian so picture data can be big endian - to reduce compatibility problems

	if(wbswap) bswap = !bswap;  // Invert

	memset(&f, 0, sizeof(DPXFILEFORMAT));
	f_ptr = (char *)(&f);
	ndatum[0] = 0; ndatum[1] = 0; ndatum[2] = 0; ndatum[3] = 0;
	subsampled[0] = 0; subsampled[1] = 0; subsampled[2] = 0; subsampled[3] = 0;

	// Color fix: Jan 2, 2007
	//
	c = dpxcolor ? dpxcolor: p->color;

	/* File header details */
	f.FileHeader.Magic = READ_DPX_32(0x53445058);
	f.FileHeader.ImageOffset = READ_DPX_32(8192);
	sprintf(f.FileHeader.Version, "V2.0");
	f.FileHeader.FileSize = READ_DPX_32(p->h * p->w);
	if ((p->color == RGB) || (p->chroma == YUV_444))
	{
		f.FileHeader.FileSize *= 3;
	} else if (p->chroma == YUV_422)
	{
		f.FileHeader.FileSize *= 2;
	} else if (p->chroma == YUV_420)
	{
		f.FileHeader.FileSize *= 3;
		f.FileHeader.FileSize /= 2;
	} else
	{
		return(DPX_ERROR_UNRECOGNIZED_CHROMA);
	}
	if ((bpp == 16) || (bpp == 12))
	{
		f.FileHeader.FileSize = f.FileHeader.FileSize * 2;
	}
	else if (bpp==10)
	{
		f.FileHeader.FileSize = f.FileHeader.FileSize * 4 / 3;
	} else if (bpp!=8)
	{
		return(DPX_ERROR_UNSUPPORTED_BPP);
	}
	f.FileHeader.FileSize = READ_DPX_32(f.FileHeader.FileSize + 8192);

	f.FileHeader.DittoKey = READ_DPX_32(~0);
	f.FileHeader.GenericSize = READ_DPX_32(sizeof(GENERICFILEHEADER) + sizeof(GENERICIMAGEHEADER) + sizeof(GENERICORIENTATIONHEADER));
	f.FileHeader.IndustrySize = READ_DPX_32(sizeof(INDUSTRYTELEVISIONINFOHEADER) + sizeof(INDUSTRYFILMINFOHEADER));
	f.FileHeader.UserSize = READ_DPX_32(0);

	snprintf(f.FileHeader.FileName, FILENAME_SIZE, "Broadcom file");
	sprintf(f.FileHeader.TimeDate, "2006:05:24:12:00:00:-08");
	sprintf(f.FileHeader.Creator, "Broadcom Corp");
	if (DVS_DPX_VER == ver)
	{
		sprintf(f.FileHeader.Project, "bcmdpx%s DVS", BCMDPX_VERSION);
	}
	//else if (XNVIEW_DPX_VER == ver) {  /* format no longer required */
	//  sprintf(f.FileHeader.Project, "bcmdpx%s", BCMDPX_VERSION);
	//}
	else if (ver == STD_DPX_VER)
		sprintf(f.FileHeader.Project, "Std dpx 2.0");
	sprintf(f.FileHeader.Copyright, " ");

	f.FileHeader.EncryptKey = READ_DPX_32(~0);

	/* Image information header details */
	f.ImageHeader.Orientation     = READ_DPX_16(0);   /* L->R, T->B */
	f.ImageHeader.NumberElements  = READ_DPX_16(1);
	f.ImageHeader.PixelsPerLine   = READ_DPX_32(p->w);
	f.ImageHeader.LinesPerElement = READ_DPX_32(p->h);

	f.ImageHeader.ImageElement[0].DataSign     = READ_DPX_32(0);  /* unsigned */
	f.ImageHeader.ImageElement[0].LowQuantity  = 0;
	SINGLE_BS(ImageHeader.ImageElement[0].LowQuantity);
	f.ImageHeader.ImageElement[0].HighQuantity = 700.0;
	SINGLE_BS(ImageHeader.ImageElement[0].HighQuantity);
	f.ImageHeader.ImageElement[0].LowData  = READ_DPX_32((RGB == p->color)? 0
		: (    16 << (bpp-8)));
	f.ImageHeader.ImageElement[0].HighData = READ_DPX_32((RGB == p->color)? (0xffff >> (16-bpp))
		: (   235 << (bpp- 8)));

	if (p->color == RGB)
	{
		if (p->alpha == 0) 
			f.ImageHeader.ImageElement[0].Descriptor = 50;
		else
			f.ImageHeader.ImageElement[0].Descriptor = 51;
	} 
	else if (p->chroma == YUV_444)
	{
		if (p->alpha == 0)
			f.ImageHeader.ImageElement[0].Descriptor = 102;
		else
			f.ImageHeader.ImageElement[0].Descriptor = 103;
	}
	else if (p->chroma == YUV_422)
	{
		if (p->alpha == 0)
			f.ImageHeader.ImageElement[0].Descriptor = 100;  /* 4:2:2 UYVY */
		else
			f.ImageHeader.ImageElement[0].Descriptor = 101;  /* 4:2:2 UYAVYA */
	}
	else if (p->chroma == YUV_420)
	{
		f.ImageHeader.ImageElement[0].Descriptor = BCMDPX_420CODE;   /* 4:2:0 Y in 1st buffer, UV in 2nd buffer */
	}

	//switch(p->color)
	switch (c) // Color fix: Jan 2, 2007
	{
	case YUV_SD:
		if (p->h == 576)
		{
			f.ImageHeader.ImageElement[0].Transfer = 7;  /* PAL */
			f.ImageHeader.ImageElement[0].Colorimetric = 7;
		} else
		{
			f.ImageHeader.ImageElement[0].Transfer = 8;  /* NTSC */
			f.ImageHeader.ImageElement[0].Colorimetric = 8;
		}
		break;
	case YUV_HD:
		f.ImageHeader.ImageElement[0].Transfer = 5;  /* SMPTE 274M */
		f.ImageHeader.ImageElement[0].Colorimetric = 5;
		break;
	default:
		f.ImageHeader.ImageElement[0].Transfer = 4;  /* Unspecified */
		f.ImageHeader.ImageElement[0].Colorimetric = 4;
	}
	f.ImageHeader.ImageElement[0].BitSize = bpp; /* bits per pixel component */
	if((bpp == 8) || (bpp == 16))
		f.ImageHeader.ImageElement[0].Packing = READ_DPX_16(0);
	else if((bpp == 10) || (bpp == 12))
		f.ImageHeader.ImageElement[0].Packing = READ_DPX_16(force_packing);   /* Use method that was passed in */
	f.ImageHeader.ImageElement[0].Encoding = READ_DPX_16(0);  /* No RLE, hopefully can add later */
	f.ImageHeader.ImageElement[0].DataOffset = READ_DPX_32(8192);
	f.ImageHeader.ImageElement[0].EndOfImagePadding = READ_DPX_32(0);
	f.ImageHeader.ImageElement[0].EndOfLinePadding = READ_DPX_32(0);
	sprintf(f.ImageHeader.ImageElement[0].Description, "pic1");

	for (i=1; i<8; ++i)
		memset(&(f.ImageHeader.ImageElement[i]), 0xff, sizeof(struct _ImageElement));

	/* Image source info header details */
	f.OrientHeader.XOffset = READ_DPX_32(0);
	f.OrientHeader.YOffset = READ_DPX_32(0);
	f.OrientHeader.XCenter = 0;
	SINGLE_BS(OrientHeader.XCenter);
	f.OrientHeader.YCenter = 0;
	SINGLE_BS(OrientHeader.YCenter);
	f.OrientHeader.XOriginalSize = READ_DPX_32(p->w);
	f.OrientHeader.YOriginalSize = READ_DPX_32(p->h);
	snprintf(f.OrientHeader.FileName, FILENAME_SIZE, "Broadcom file");
	sprintf(f.OrientHeader.TimeDate, "2006:05:24:12:00:00:-08");
	sprintf(f.OrientHeader.InputName, "DVS Clipster");
	sprintf(f.OrientHeader.InputSN, "0001");
	f.OrientHeader.Border[0] = READ_DPX_16(0);
	f.OrientHeader.Border[1] = READ_DPX_16(p->w);
	f.OrientHeader.Border[2] = READ_DPX_16(0);
	f.OrientHeader.Border[3] = READ_DPX_16(p->h);
	f.OrientHeader.AspectRatio[0] = READ_DPX_32(ar1);
	f.OrientHeader.AspectRatio[1] = READ_DPX_32(ar2);

	/* Film header info details */
	memset(&(f.FilmHeader), 0, sizeof(INDUSTRYFILMINFOHEADER));
	sprintf(f.FilmHeader.Format, "Academy");
	f.FilmHeader.FramePosition = READ_DPX_32(frameno);
	f.FilmHeader.SequenceLen = READ_DPX_32(seqlen);
	f.FilmHeader.HeldCount = READ_DPX_32(1);
	f.FilmHeader.FrameRate = framerate;
	SINGLE_BS(FilmHeader.FrameRate);
	f.FilmHeader.ShutterAngle = 0;
	SINGLE_BS(FilmHeader.ShutterAngle);
	sprintf(f.FilmHeader.FrameId, "frame");
	sprintf(f.FilmHeader.SlateInfo, "slate");

	/* TV header info details */
	f.TvHeader.TimeCode    = READ_DPX_32(generate_timecode(frameno, framerate));
	f.TvHeader.UserBits    = READ_DPX_32(0);
	f.TvHeader.Interlace   = interlaced;
	f.TvHeader.FieldNumber = interlaced? frameno % 2 + 1: 0;

	switch (p->h)
	{
	case 480:
	case 486:
		f.TvHeader.VideoSignal = 1;  /* NTSC */
		break;
	case 576:
		f.TvHeader.VideoSignal = 2;  /* PAL */
		break;
	default:
		f.TvHeader.VideoSignal = 0;  /* undefined */
	}

	if ((p->h==486) || (p->h==480))
	{  /* NTSC */
		f.TvHeader.HorzSampleRate = 13500000;
	} else if ((p->h==720) || ((p->h==1080) && (interlaced || (framerate < 50.0))))
	{   /* 720p | 1080i */
		f.TvHeader.HorzSampleRate = ((framerate == 24.0) || (framerate == 30.0) || (framerate == 60.0))
			? (SINGLE)74250000
			: (SINGLE)(74250000.0 * 1000.0/1001.0);
	} else if (p->h==1080)
	{   /* 1080p */
		f.TvHeader.HorzSampleRate = (framerate==60.0)
			? (SINGLE)148500000.0
			: (SINGLE)(148500000.0 * 1000.0/1001.0);
	} else
	{
		f.TvHeader.HorzSampleRate = framerate * p->h * p->w;
	}

	SINGLE_BS(TvHeader.HorzSampleRate);

	f.TvHeader.VertSampleRate = interlaced? framerate * 2
		: framerate;
	SINGLE_BS(TvHeader.VertSampleRate);

	f.TvHeader.FrameRate  = framerate;
	SINGLE_BS(TvHeader.FrameRate);
	f.TvHeader.TimeOffset = 0;
	SINGLE_BS(TvHeader.TimeOffset);
	f.TvHeader.Gamma      = 2;
	SINGLE_BS(TvHeader.Gamma);

	f.TvHeader.BlackLevel = (SINGLE)(0x10 << (bpp-8));
	SINGLE_BS(TvHeader.BlackLevel);
	f.TvHeader.BlackGain  = 1;
	SINGLE_BS(TvHeader.BlackGain);
	f.TvHeader.Breakpoint = 0;
	SINGLE_BS(TvHeader.Breakpoint);
	f.TvHeader.WhiteLevel = (SINGLE)(0xeb << (bpp-8));
	SINGLE_BS(TvHeader.WhiteLevel);
	f.TvHeader.IntegrationTimes = 0;
	SINGLE_BS(TvHeader.IntegrationTimes);

	if ((fp = fopen(fname, "wb")) == NULL)
	{
		fprintf(stderr, "Cannot open %s for output\n", fname);
		exit(1);
	}

	fwrite(&f, sizeof(DPXFILEFORMAT), 1, fp);

	fseek(fp, 8192, SEEK_SET);

#define BYTE_SWAP(x)  x = (((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x & 0xff0000) >> 8) | (x >> 24))

	if (p->color == RGB)
	{
		nbuffer = 1;
		if(swaprb)
		{
			datum[0][0] = p->data.rgb.r;
			datum[0][1] = p->data.rgb.g;
			datum[0][2] = p->data.rgb.b;
		} else {
		datum[0][0] = p->data.rgb.b;
		datum[0][1] = p->data.rgb.g;
		datum[0][2] = p->data.rgb.r;
		}
		if (p->alpha == 0)
		{
			ndatum[0] = 3;
		}
		else
		{
			ndatum[0] = 4;
			datum[0][3] = p->data.rgb.a;
		}

		subsampled[0] = 0;
		wbuff[0] = p->w;
		hbuff[0] = p->h;
	} else if (p->chroma == YUV_422)
	{
		nbuffer = 1;
		if (p->alpha == 0)
		{
			datum[0][0] = p->data.yuv.u;
			datum[0][1] = p->data.yuv.y;
			datum[0][2] = p->data.yuv.v;
			datum[0][3] = p->data.yuv.y;
			ndatum[0] = 4;
		}
		else
		{
			datum[0][0] = p->data.yuv.u;
			datum[0][1] = p->data.yuv.y;
			datum[0][2] = p->data.yuv.a;
			datum[0][3] = p->data.yuv.v;
			datum[0][4] = p->data.yuv.y;
			datum[0][5] = p->data.yuv.a;
			ndatum[0] = 6;
		}
		subsampled[0] = 1;
		wbuff[0] = p->w;
		hbuff[0] = p->h;
	} else if (p->chroma == YUV_444)
	{
		nbuffer = 1;
		datum[0][0] = p->data.yuv.u;
		datum[0][1] = p->data.yuv.y;
		datum[0][2] = p->data.yuv.v;
		if (p->alpha == 0)
		{
			ndatum[0] = 3;
		}
		else
		{
			datum[0][3] = p->data.yuv.a;
			ndatum[0] = 4;
		}	      
		subsampled[0] = 0;
		wbuff[0] = p->w;
		hbuff[0] = p->h;
	} else if (p->chroma == YUV_420)
	{
		nbuffer = 2;
		ndatum[0] = 1;
		ndatum[1] = 2;
		datum[0][0] = p->data.yuv.y;
		datum[1][0] = p->data.yuv.u;
		datum[1][1] = p->data.yuv.v;
		subsampled[0] = 0;
		subsampled[1] = 0;
		wbuff[0] = p->w;
		hbuff[0] = p->h;
		wbuff[1] = p->w / 2;
		hbuff[1] = p->h / 2;
	}
	element = 0;
	fifo_init(&fifo, 64);
	for (b = 0; b < nbuffer; b++) // Loop over buffers
	{
		for (i = 0; i < hbuff[b]; ++i) // every row
		{
			for (j = 0; j<(subsampled[b] ? wbuff[b]/2 : wbuff[b]); ++j) // every pixel
			{
				for (k=0; k<ndatum[b]; ++k)
				{
					xindex = (ndatum[b] == 6) ? ((k%3 == 0) ? j : j*2 + (k/3)) : // "Hack" for UYAVYA (422 with alpha), only know mode to have ndatum == 6
						(subsampled[b] && (k & 0x1)) ? j*2 + (k>>1) : j;
					//if (element == 0) data32b = 0;
					switch (bpp)
					{
					case 8:
						if(datum_order)
							fifo_put_bits(&fifo, datum[b][k][i][xindex], 8);
						else
							fifo_flip_put_bits(&fifo, datum[b][k][i][xindex], 8);
						break;
					case 10:
						if(datum_order)
						{
							if(force_packing == 0)
								fifo_put_bits(&fifo, datum[b][k][i][xindex], 10);
							else if(force_packing == 1)
							{
								fifo_put_bits(&fifo, datum[b][k][i][xindex], 10);
								if(element == 2)
									fifo_put_bits(&fifo, 0, 2);
							}
							else if (force_packing == 2)
							{
								if(element == 0)
									fifo_put_bits(&fifo, 0, 2);
								fifo_put_bits(&fifo, datum[b][k][i][xindex], 10);
							}
						}
						else
						{
							if(force_packing == 0)
								fifo_flip_put_bits(&fifo, datum[b][k][i][xindex], 10);
							else if(force_packing == 1)
							{
								if(element == 0)
									fifo_flip_put_bits(&fifo, 0, 2);
								fifo_flip_put_bits(&fifo, datum[b][k][i][xindex], 10);
							}
							else if (force_packing == 2)
							{
								fifo_flip_put_bits(&fifo, datum[b][k][i][xindex], 10);
								if(element == 2)
									fifo_flip_put_bits(&fifo, 0, 2);
							}
						}
						element = (element+1) % 3;
						break;
					case 12:
						if(datum_order)
						{
							if(force_packing == 0)
								fifo_put_bits(&fifo, datum[b][k][i][xindex], 12);
							else if(force_packing == 1)
							{
								fifo_put_bits(&fifo, datum[b][k][i][xindex], 12);
								fifo_put_bits(&fifo, 0, 4);
							}
							else if (force_packing == 2)
							{
								fifo_put_bits(&fifo, 0, 4);
								fifo_put_bits(&fifo, datum[b][k][i][xindex], 12);
							}
						}
						else
						{
							if(force_packing == 0)
								fifo_flip_put_bits(&fifo, datum[b][k][i][xindex], 12);
							else if(force_packing == 1)
							{
								fifo_flip_put_bits(&fifo, 0, 4);
								fifo_flip_put_bits(&fifo, datum[b][k][i][xindex], 12);
							}
							else if (force_packing == 2)
							{
								fifo_flip_put_bits(&fifo, datum[b][k][i][xindex], 12);
								fifo_flip_put_bits(&fifo, 0, 4);
							}
						}
						break;
					case 16:
						if(datum_order)
							fifo_put_bits(&fifo, datum[b][k][i][xindex], 16);
						else
							fifo_flip_put_bits(&fifo, datum[b][k][i][xindex], 16);
						break;
					default:
						return(DPX_ERROR_UNSUPPORTED_BPP);
					}
					if (fifo.fullness >= 32)
					{
						data32b = fifo_get_bits(&fifo, 32, 0);
						data32b = READ_DPX_32(data32b);
						fwrite(&data32b, 4, 1, fp);
					}
				}
			} // end of a row
			// fill in and start new from a new line.  -- this is to match XnView 1.93.
			// Not sure this is standard compliant. Q.
			if (pad_ends && 
				(bpp == 8) &&
				(fifo.fullness > 0))
			{
				if(datum_order)
					fifo_put_bits(&fifo, 0, 32 - fifo.fullness);
				else
					fifo_flip_put_bits(&fifo, 0, 32 - fifo.fullness);
				data32b = fifo_get_bits(&fifo, 32, 0);
				data32b = READ_DPX_32(data32b);
				fwrite(&data32b, 4, 1, fp);
			}
		}
	}
	if(fifo.fullness > 0)
	{
		if(datum_order)
			fifo_put_bits(&fifo, 0, 32 - fifo.fullness);
		else
			fifo_flip_put_bits(&fifo, 0, 32 - fifo.fullness);
		data32b = fifo_get_bits(&fifo, 32, 0);
		data32b = READ_DPX_32(data32b);
		fwrite(&data32b, 4, 1, fp);
	}

	fclose(fp);
	return(0);
}

#define DEC2HEX(a) ( (((a)/10) << 4) | ((a) % 10) )
static DWORD generate_timecode(int frameno, float framerate)
{
	int h, m, s, f;
	DWORD tc;

	s = (int)(frameno / framerate);

	f = (int)((((float)frameno / framerate) - (float)s) * framerate);
	h = s / 3600;
	s = s % 3600;
	m = s / 60;
	s = s % 60;

	tc = (DEC2HEX(h) << 24) | (DEC2HEX(m) << 16) | (DEC2HEX(s) << 8) | DEC2HEX(f);
	return(tc);
}


/*** Return pointer to picture with correct raster ***/
int read_dpx(char *fname, pic_t **p, int *ar1, int *ar2, int *frameno, int *seqlen, float *framerate, int *interlaced, int *bpp, int dpx_bugs, int noswap, int datum_order, int swap_r_and_b)
{
	int high, low;
	int ret_val = dpx_read_hl(fname, p, &high, &low, dpx_bugs, noswap, datum_order, swap_r_and_b);
	if (!ret_val) {
		*ar1        = (*p)->ar1;
		*ar2        = (*p)->ar2;
		*frameno    = (*p)->frm_no;
		*seqlen     = (*p)->seq_len;
		*framerate  = (*p)->framerate;
		*interlaced = (*p)->interlaced;
		*bpp        = (*p)->bits;
	}

	return ret_val;
}

int dpx_read(char *fname, pic_t **p, int pad_ends, int noswap, int datum_order, int swap_r_and_b)
{
	int high, low;
	return dpx_read_hl(fname, p, &high, &low, pad_ends, noswap, datum_order, swap_r_and_b);
}

int dpx_read_hl(char *fname, pic_t **p, int* highdata, int*lowdata, int pad_ends, int noswap, int datum_order, int swap_r_and_b)
{
	FILE *fp;
	DWORD magic;
	int bswap = 0;
	DWORD d;
	DPXFILEFORMAT f;
	int i;
	int ecode;
	int w, h;
	static int show_range_warn = 1; // Allow range warning for only the first file.
	int bpp;
	int system_is_le;
	int data_bswap;

	if ((fp = fopen(fname, "rb")) == NULL)
	{
		fprintf(stderr, "Error: Cannot read DPX file %s\n", fname); 
		perror("Cannot read DPX file");
		return(DPX_ERROR_BAD_FILENAME);
	}
	if (fread(&magic, 1, 4, fp) != 4)
	{
		return(DPX_ERROR_CORRUPTED_FILE);
	}
	fseek(fp, 0, SEEK_SET);
	system_is_le = (fgetc(fp) == 'S') ^ (magic == 0x53445058);

	if (magic == 0x53445058)
		bswap = 0;
	else if (magic == 0x58504453)
		bswap = 1;
	else
		return(DPX_ERROR_CORRUPTED_FILE);

	if(noswap)  // Force BE order (swap if LE system)
		data_bswap = system_is_le;
	else        // Follow magic number order (swap if magic number came back flipped)
		data_bswap = bswap;

	fseek(fp, 0, SEEK_SET);
	memset(&f, 0, sizeof(DPXFILEFORMAT));
	if (fread(&f, sizeof(DPXFILEFORMAT), 1, fp) == 0)
	{
		fprintf(stderr, "Error: Tried to read corrupted DPX file %s\n", fname);
		return(DPX_ERROR_CORRUPTED_FILE);
	}

	w = READ_DPX_32(f.ImageHeader.PixelsPerLine);
	h = READ_DPX_32(f.ImageHeader.LinesPerElement);
	*p = NULL;

 	if(strstr(f.FileHeader.Copyright, "Broadcom") != NULL)
	{
		data_bswap = bswap;  // Follow the spec
		datum_order = 0;
		swap_r_and_b = 0;
		printf("DPX file appears to be from old version of DSC C model, using SMPTE-268M native switches\n");
	}
	if((strcmp(f.FileHeader.Version, "V1.0") == 0) && f.ImageHeader.ImageElement[0].BitSize == 8)
	{
		data_bswap = system_is_le;
		swap_r_and_b = 0;
		printf("DPX file appears to be 8-bit V1.0, assuming BE, BGR order\n");
	}
	if((strcmp(f.FileHeader.Version, "V1.0") == 0) && f.ImageHeader.ImageElement[0].BitSize == 10)
	{
		datum_order = 0;
		swap_r_and_b = 0;
		printf("DPX file appears to be 10-bit V1.0, assuming datum order 0, BGR component order\n");
	}

	for (i=0; i<READ_DPX_16(f.ImageHeader.NumberElements); ++i)
	{
		fseek(fp, READ_DPX_32(f.ImageHeader.ImageElement[i].DataOffset), SEEK_SET);

		ecode = read_dpx_image_data(fp, p, READ_DPX_16(f.ImageHeader.Orientation), READ_DPX_32(f.ImageHeader.ImageElement[i].DataSign),
			f.ImageHeader.ImageElement[i].BitSize, f.ImageHeader.ImageElement[i].Descriptor,
			READ_DPX_16(f.ImageHeader.ImageElement[i].Encoding), pad_ends, w, h, data_bswap, READ_DPX_16(f.ImageHeader.ImageElement[i].Packing), datum_order, swap_r_and_b);
		if (ecode)
		{
			fprintf(stderr, "DPX read warning (file=%s): unexpected non-zero padding bits. Trying opposite endianness.\n", fname);
			fseek(fp, READ_DPX_32(f.ImageHeader.ImageElement[i].DataOffset), SEEK_SET);
			ecode = read_dpx_image_data(fp, p, READ_DPX_16(f.ImageHeader.Orientation), READ_DPX_32(f.ImageHeader.ImageElement[i].DataSign),
				f.ImageHeader.ImageElement[i].BitSize, f.ImageHeader.ImageElement[i].Descriptor,
				READ_DPX_16(f.ImageHeader.ImageElement[i].Encoding), pad_ends, w, h, !data_bswap, READ_DPX_16(f.ImageHeader.ImageElement[i].Packing), datum_order, swap_r_and_b);
			if (ecode)
			{
				return(ecode);
			}
		}
		if((*p)==NULL)
			return(DPX_ERROR_MALLOC_FAIL);
	}
	if ((f.ImageHeader.ImageElement[0].Transfer == 5) || (f.ImageHeader.ImageElement[0].Transfer == 6))
	{
		(*p)->color = YUV_HD;
	}
	if ((f.ImageHeader.ImageElement[0].Colorimetric == 5) || (f.ImageHeader.ImageElement[0].Colorimetric == 6))
	{
		(*p)->color = YUV_HD;
	}
	if (f.OrientHeader.AspectRatio[0] == 0xffffffffl)
	{
		(*p)->ar1 = -1;
		(*p)->ar2 = -1;
	} else
	{
		(*p)->ar1 = READ_DPX_32(f.OrientHeader.AspectRatio[0]);
		(*p)->ar2 = READ_DPX_32(f.OrientHeader.AspectRatio[1]);
	}
	if (f.FilmHeader.FramePosition == 0xffffffffl)
	{
		(*p)->frm_no = -1;
	} else    
	{
		(*p)->frm_no = READ_DPX_32(f.FilmHeader.FramePosition);
	}
	if (f.FilmHeader.SequenceLen == 0xffffffffl)
	{
		(*p)->seq_len = -1;
	} else
	{
		(*p)->seq_len = READ_DPX_32(f.FilmHeader.SequenceLen);
	}
	memcpy(&d, &f.TvHeader.FrameRate, 4);
	// ((float *)(&d)) = (float)f.TvHeader.FrameRate;
	if (d == 0xffffffffl)
	{
		(*p)->framerate = 0;
	} else
	{
		d = READ_DPX_32(d);
		memcpy(&((*p)->framerate), &d, 4);
		//(*p)->framerate = *((float *)(&d));
	}
	if (f.TvHeader.Interlace == 0xff)
	{
		(*p)->interlaced = -1;
	} else
	{
		(*p)->interlaced = f.TvHeader.Interlace;
	}

	bpp = (*p)->bits = f.ImageHeader.ImageElement[0].BitSize;

	// fix Endian issue with Low and High Data fields
	if (((*p)->color != RGB) && (bswap))
	{
		BYTE_SWAP(f.ImageHeader.ImageElement[0].LowData);
		BYTE_SWAP(f.ImageHeader.ImageElement[0].HighData); 
	}

	*lowdata  = f.ImageHeader.ImageElement[0].LowData;
	*highdata = f.ImageHeader.ImageElement[0].HighData;

	if (show_range_warn &&
		f.ImageHeader.ImageElement[0].Descriptor >= 100 &&
		f.ImageHeader.ImageElement[0].Descriptor <= 104 &&
		(f.ImageHeader.ImageElement[0].LowData  != ( 16u << (bpp -8)) ||
		f.ImageHeader.ImageElement[0].HighData != (235u << (bpp -8))) )
	{
		show_range_warn = 0;  // only show the warning one time
		fprintf(stderr, "\nWARNING: Header reference low/high data of %s ignored. \n", fname);
		fprintf(stderr, "           reference low  = %5d => Defaults to %d\n", f.ImageHeader.ImageElement[0].LowData, (16 << (bpp-8)));
		fprintf(stderr, "           reference high = %5d => Defaults to %d\n\n", f.ImageHeader.ImageElement[0].HighData, (235 << (bpp-8)));
	}
	fclose(fp);

	(*p)->format = determine_field_format(fname);
	return(0);
}


/**************************************************/
/* bugs            Compatibility                  */
/* 0               Standard                       */
/* 1               XNView 1.82 (not supported)    */
/* 2               DVS 2.1.2                      */
/**************************************************/
static int read_dpx_image_data(FILE *fp, pic_t **p, int orientation, int sign, int bpp, int descriptor, int rle, int pad_ends, int w, int h, int bswap, int packing, int datum_order, int swap_r_and_b)
{
	int **ptr[4][6];
	int nbuffer = 1; // min = 1, max = 4
	int ncomponents[4];
	int wbuff[4];
	int hbuff[4];
	int subsampled_x[4]; // Horizontal sub-sampling present in buffer i for at least 1 (but not all components)
	int subs[4][6];
	int i, j, k, b;
	int xindex;
	int nelement = 0;
	DWORD data32b;
	int skip_read[6] = {0, 0, 0, 0, 0, 0};
	fifo_t fifo;

	fifo_init(&fifo, 3*4);  // 3 32-bit words

	subsampled_x[0] = 0; subsampled_x[1] = 0; subsampled_x[2] = 0; subsampled_x[3] = 0;
	if (0 != rle)
	{
		return DPX_ERROR_NOT_IMPLEMENTED;
	}
	if (orientation != 0)
	{
		return(DPX_ERROR_NOT_IMPLEMENTED);
	}

	for (b = 0; b < 4; b++)
	{
		for (i=0; i<6; ++i)
		{
			subs[b][i] = 0;
		}
	}

	switch (descriptor)
	{
	case 1:                   /* Red */
		if (create_dpx_pic(p, YUV_444, RGB, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		ncomponents[0] = 1;
		ptr[0][0] = (*p)->data.rgb.r;
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 2:                   /* Green */
		if (create_dpx_pic(p, YUV_444, RGB, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		ncomponents[0] = 1;
		ptr[0][0] = (*p)->data.rgb.g;
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 3:                   /* Blue */
		if (create_dpx_pic(p, YUV_444, RGB, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		ncomponents[0] = 1;
		ptr[0][0] = (*p)->data.rgb.b;
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 4:                   /* Alpha */
		ncomponents[0] = 1;
		return(DPX_ERROR_NOT_IMPLEMENTED);
		/*          if((*p)->color == RGB)
		ptr[0] = (*p)->data.rgb.a;
		else
		ptr[0] = p->data.yuv.a; */
		// wbuff[0] = w;
		// hbuff[0] = h;
		break;
	case 6:                   /* Luma */
		if (create_dpx_pic(p, YUV_422, YUV_SD, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		ncomponents[0] = 1;
		ptr[0][0] = (*p)->data.yuv.y;
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 7:                   /* Cb, Cr subsampled x2 */
		if (create_dpx_pic(p, YUV_422, YUV_SD, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		ncomponents[0] = 2;
		ptr[0][0] = (*p)->data.yuv.u;
		ptr[0][1] = (*p)->data.yuv.v;
		subsampled_x[0] = 1;
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 50:                  /* RGB/BGR */
		if (create_dpx_pic(p, YUV_444, RGB, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		ncomponents[0] = 3;
		if(swap_r_and_b)
		{
			ptr[0][0] = (*p)->data.rgb.r;
			ptr[0][1] = (*p)->data.rgb.g;
			ptr[0][2] = (*p)->data.rgb.b;
		} else {
			ptr[0][0] = (*p)->data.rgb.b;
			ptr[0][1] = (*p)->data.rgb.g;
			ptr[0][2] = (*p)->data.rgb.r;
		}
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 51:                  /* BGRA */
		if (create_dpx_pic(p, YUV_444, RGB, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		(*p)->alpha = 1;
		ncomponents[0] = 4;
		if(swap_r_and_b)
		{
			ptr[0][0] = (*p)->data.rgb.r;
			ptr[0][1] = (*p)->data.rgb.g;
			ptr[0][2] = (*p)->data.rgb.b;
		} else {
			ptr[0][0] = (*p)->data.rgb.b;
			ptr[0][1] = (*p)->data.rgb.g;
			ptr[0][2] = (*p)->data.rgb.r;
		}
		ptr[0][3] = (*p)->data.rgb.a;
		skip_read[3] = 1;  // Alpha doesn't go into a buffer
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 52:                  /* ARGB */
		if (create_dpx_pic(p, YUV_444, RGB, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		(*p)->alpha = 1;
		ncomponents[0] = 4;
		ptr[0][0] = (*p)->data.rgb.a;
		skip_read[0] = 1;  // Alpha doesn't go into a buffer
		ptr[0][1] = (*p)->data.rgb.r;
		ptr[0][2] = (*p)->data.rgb.g;
		ptr[0][3] = (*p)->data.rgb.b;
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 100:                 /* Cb Y Cr Y (4:2:2) */
		if (create_dpx_pic(p, YUV_422, YUV_SD, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		ncomponents[0] = 4;
		ptr[0][0] = (*p)->data.yuv.u;
		ptr[0][1] = (*p)->data.yuv.y;
		ptr[0][2] = (*p)->data.yuv.v;
		ptr[0][3] = (*p)->data.yuv.y;
		subsampled_x[0] = 1;
		subs[0][1] = 1;
		subs[0][3] = 2;
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 101:                 /* Cb Y A Cr Y A (4:2:2:4) */
		if (create_dpx_pic(p, YUV_422, YUV_SD, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		(*p)->alpha = 1;
		ncomponents[0] = 6;
		ptr[0][0] = (*p)->data.yuv.u;
		ptr[0][1] = (*p)->data.yuv.y;
		ptr[0][2] = (*p)->data.yuv.a;
		skip_read[2] = 1;  // Alpha doesn't go into a buffer
		ptr[0][3] = (*p)->data.yuv.v;
		ptr[0][4] = (*p)->data.yuv.y;
		ptr[0][5] = (*p)->data.yuv.a;
		skip_read[5] = 1;  // Alpha doesn't go into a buffer
		subsampled_x[0] = 1;
		subs[0][1] = 1;
		subs[0][2] = 1;
		subs[0][4] = 2;
		subs[0][5] = 2;
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 102:                 /* Cb Y Cr (4:4:4) */
		if (create_dpx_pic(p, YUV_444, YUV_SD, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		ncomponents[0] = 3;
		ptr[0][0] = (*p)->data.yuv.u;
		ptr[0][1] = (*p)->data.yuv.y;
		ptr[0][2] = (*p)->data.yuv.v;
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case 103:                 /* Cb Y Cr A (4:4:4:4) */
		if (create_dpx_pic(p, YUV_444, YUV_SD, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		(*p)->alpha = 1;
		ncomponents[0] = 4;
		ptr[0][0] = (*p)->data.yuv.u;
		ptr[0][1] = (*p)->data.yuv.y;
		ptr[0][2] = (*p)->data.yuv.v;
		ptr[0][3] = (*p)->data.yuv.a;
		skip_read[3] = 1;  // Alpha doesn't go into a buffer
		wbuff[0] = w;
		hbuff[0] = h;
		break;
	case BCMDPX_420CODE:     /* Y in 1st buffer, Cb Cr in 2nd buffer */
		if (create_dpx_pic(p, YUV_420, YUV_SD, w, h, bpp))
		{
			return(DPX_ERROR_MISMATCH);
		}
		nbuffer        = 2;
		ncomponents[0] = 1;
		ncomponents[1] = 2;
		ptr[0][0] = (*p)->data.yuv.y;
		ptr[1][0] = (*p)->data.yuv.u;
		ptr[1][1] = (*p)->data.yuv.v;
		wbuff[0] = w;
		hbuff[0] = h;
		wbuff[1] = w/2;
		hbuff[1] = h/2;
		break;
	default:
		return(DPX_ERROR_NOT_IMPLEMENTED);
	}

	//fprintf(stderr, "descriptor=%d bugs=%d bswap=%d\n", descriptor, bugs, bswap);
	for (b = 0; b < nbuffer; b++)
	{
		for (i = 0; i < hbuff[b]; ++i)
		{
			for (j = 0; j < (subsampled_x[b] ? wbuff[b]/2 : wbuff[b]); ++j)
			{
				for (k = 0; k < ncomponents[b]; ++k)
				{
					xindex = (subsampled_x[b] && (subs[b][k] == 1))? j*2
						: (subsampled_x[b] && (subs[b][k] == 2))? j*2+1
						: j;
					if(fifo.fullness < 32+16)   // Most data we can grab
					{
						if (fread(&data32b, 4, 1, fp) == 0)
							data32b = 0;
						if (bswap)
						{
							BYTE_SWAP(data32b);
						}
						fifo_put_bits(&fifo, data32b, 32);
					}
					switch (bpp)
					{
					case 8:
						if(datum_order)
						{
							if(!skip_read[k])
								ptr[b][k][i][xindex] = fifo_get_bits(&fifo, 8, sign);
							else
								fifo_get_bits(&fifo, 8, sign);
						} else {
							if(!skip_read[k])
								ptr[b][k][i][xindex] = fifo_flip_get_bits(&fifo, 8, sign);
							else
								fifo_flip_get_bits(&fifo, 8, sign);
						}
						break;
					case 10:
						if(datum_order)
						{
							if(packing==0)  // No padding bits
							{
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_get_bits(&fifo, 10, sign);
								else
									fifo_get_bits(&fifo, 10, sign);
							}
							else if(packing == 1)  // Method A
							{
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_get_bits(&fifo, 10, sign);
								else
									fifo_get_bits(&fifo, 10, sign);
								if(nelement == 2)
									if((fifo_get_bits(&fifo, 2, 0) != 0) && !feof(fp))
										return(DPX_ERROR_CORRUPTED_FILE);
							}
							else if(packing == 2)  // Method B
							{
								if(nelement == 2)
									if((fifo_get_bits(&fifo, 2, 0) == 0) && !feof(fp))
										return(DPX_ERROR_CORRUPTED_FILE);
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_get_bits(&fifo, 10, sign);
								else
									fifo_get_bits(&fifo, 10, sign);
							}								
						} else {
							if(packing==0)  // No padding bits
							{
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_flip_get_bits(&fifo, 10, sign);
								else
									fifo_flip_get_bits(&fifo, 10, sign);
							}
							else if(packing == 1)  // Method A
							{
								if(nelement == 0)
									if((fifo_flip_get_bits(&fifo, 2, 0) != 0) && !feof(fp))
										return(DPX_ERROR_CORRUPTED_FILE);
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_flip_get_bits(&fifo, 10, sign);
								else
									fifo_flip_get_bits(&fifo, 10, sign);
							}
							else if(packing == 2)  // Method B
							{
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_flip_get_bits(&fifo, 10, sign);
								else
									fifo_flip_get_bits(&fifo, 10, sign);
								if(nelement == 2)
									if((fifo_flip_get_bits(&fifo, 2, 0) == 0) && !feof(fp))
										return(DPX_ERROR_CORRUPTED_FILE);
							}								
						}
						nelement = (nelement+1)%3;
						break;
					case 12:  /* Method B */
						if(datum_order)
						{
							if(packing==0)  // No padding bits
							{
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_get_bits(&fifo, 12, sign);
								else
									fifo_get_bits(&fifo, 12, sign);
							}
							else if(packing == 1)  // Method A
							{
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_get_bits(&fifo, 12, sign);
								else
									fifo_get_bits(&fifo, 12, sign);
								if((fifo_get_bits(&fifo, 4, 0) != 0) && !feof(fp))
									return(DPX_ERROR_CORRUPTED_FILE);
							}
							else if(packing == 2)  // Method B
							{
								if((fifo_get_bits(&fifo, 4, 0) == 0) && !feof(fp))
									return(DPX_ERROR_CORRUPTED_FILE);
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_get_bits(&fifo, 12, sign);
								else
									fifo_get_bits(&fifo, 12, sign);
							}
						} else {
							if(packing==0)  // No padding bits
							{
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_flip_get_bits(&fifo, 12, sign);
								else
									fifo_flip_get_bits(&fifo, 12, sign);
							}
							else if(packing == 1)  // Method A
							{
								if((fifo_flip_get_bits(&fifo, 4, 0) != 0) && !feof(fp))
									return(DPX_ERROR_CORRUPTED_FILE);
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_flip_get_bits(&fifo, 12, sign);
								else
									fifo_flip_get_bits(&fifo, 12, sign);
							}
							else if(packing == 2)  // Method B
							{
	 							if(!skip_read[k])
 									ptr[b][k][i][xindex] = fifo_flip_get_bits(&fifo, 12, sign);
								else
									fifo_flip_get_bits(&fifo, 12, sign);
								if((fifo_flip_get_bits(&fifo, 4, 0) == 0) && !feof(fp))
									return(DPX_ERROR_CORRUPTED_FILE);
							}
						}
						break;
					case 16:
						if(datum_order)
						{
							if(!skip_read[k])
								ptr[b][k][i][xindex] = fifo_get_bits(&fifo, 16, sign);
							else
								fifo_get_bits(&fifo, 16, sign);
						}
						else
						{
							if(!skip_read[k])
								ptr[b][k][i][xindex] = fifo_flip_get_bits(&fifo, 16, sign);
							else
								fifo_flip_get_bits(&fifo, 16, sign);
						}
						break;
					default:
						return(DPX_ERROR_NOT_IMPLEMENTED);
					}
				}
			}
			/* End of line */
			//if ((bpp==8) && (bugs==1))
			if((pad_ends && bpp == 8) ||
			   (packing==0 && (bpp==10 || bpp==12)))  // Required per spec
			{
				while(fifo.fullness % 32)
				{
					if(datum_order)
						fifo_get_bits(&fifo, 1, 0);
					else
						fifo_flip_get_bits(&fifo, 1, 0);
				}
			}
		}
	}
	return(0);
}

static int create_dpx_pic(pic_t **p, chroma_t chroma, color_t color, int w, int h, int bits)
{
	int i, j;

	if (*p==NULL)
	{
		*p = (pic_t *)pcreate(0, color, chroma, w, h);
	} else
	{
		if ((chroma != (*p)->chroma) || (color != (*p)->color))
		{
			return(DPX_ERROR_MISMATCH);
		}
	}

	// Set U/V to midpoint
	if(chroma==YUV_444)
	{
		for(i=0; i<(*p)->h; ++i)
		{
			for(j=0; j<(*p)->w; ++j)
			{
				if((*p)->color != RGB)
				{
					(*p)->data.yuv.u[i][j] = 1<<(bits-1);
					(*p)->data.yuv.v[i][j] = 1<<(bits-1);
				}
			}
		}
	} else if(chroma==YUV_422)
	{
		for(i=0; i<(*p)->h; ++i)
		{
			for(j=0; j<(*p)->w/2; ++j)
			{
				(*p)->data.yuv.u[i][j] = 1<<(bits-1);
				(*p)->data.yuv.v[i][j] = 1<<(bits-1);			
			}
		}
	} else if(chroma==YUV_420)
	{
		for(i=0; i<(*p)->h/2; ++i)
		{
			for(j=0; j<(*p)->w/2; ++j)
			{
				(*p)->data.yuv.u[i][j] = 1<<(bits-1);
				(*p)->data.yuv.v[i][j] = 1<<(bits-1);			
			}
		}
	}
	return(0);
}

/* Set color = 0 to disable colorspace override */
void set_dpx_colorspace(int color)
{
	dpxcolor = color;
}

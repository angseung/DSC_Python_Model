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

/*! \file utl.c
 *    Utility functions */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "vdo.h"
#include "utl.h"
#include "logging.h"


//! Allocate the memory for pixel data
/*! \param w       Picture width
    \param h       Picture height
    \return        Pointer to 2D pixel buffer */
void *palloc(int w, int h)
{
	int **p;
	int   i;

	p = (int **) calloc(h, sizeof(short *));
	if (p == NULL)
	{
		fprintf(stderr, "ERROR: Failed to allocate memory.\n");
		exit(1);
	}
	else
	{
		for (i = 0; i < h; i++)
		{
			p[i] = calloc(w, sizeof(int));
			if (p[i] == NULL)
			{
				fprintf(stderr, "ERROR: Failed to allocate memory.\n");
				exit(1);
			}
		}
	}
	return p;
}


//! Create a picture object
/*! \param format  indicates field or frame
    \param color   color space
	\param chroma  Chroma subsampling
	\param w       Picture width
	\param h       Picture height
    \return        Pointer to picture (pic_t) object */
pic_t *pcreate(int format, int color, int chroma, int w, int h)
{
	pic_t *p;    

	p = malloc(sizeof(pic_t));

	if (p == NULL)
	{
		fprintf(stderr, "ERROR: Failed to allocate memory.\n");
		exit(1);
	}

	p->format = format;
	p->color  = color;
	p->chroma = chroma;

	p->w = w;
	p->h = h;

	if (color == RGB)
	{
		p->data.rgb.r = (int **) palloc(w, h);
		p->data.rgb.g = (int **) palloc(w, h);
		p->data.rgb.b = (int **) palloc(w, h);
		//p->data.rgb.a = (int **) palloc(w, h);
	}
	else if (chroma == YUV_420)
	{
		p->data.yuv.y = (int **) palloc(w, h);
		p->data.yuv.u = (int **) palloc(w / 2, h / 2);
		p->data.yuv.v = (int **) palloc(w / 2, h / 2);
		//p->data.yuv.a = (int **) palloc(w, h);
	}
	else if (chroma == YUV_422)
	{

		p->data.yuv.y = (int **) palloc(w, h);
		p->data.yuv.u = (int **) palloc(w / 2, h);
		p->data.yuv.v = (int **) palloc(w / 2, h);
		//p->data.yuv.a = (int **) palloc(w, h);
	}
	else
	{
		p->data.yuv.y = (int **) palloc(w, h);
		p->data.yuv.u = (int **) palloc(w, h);
		p->data.yuv.v = (int **) palloc(w, h);
		//p->data.yuv.a = (int **) palloc(w, h);
	}

	// Set defaults for deterministic DPX output
	p->alpha = 0;
	p->ar1 = 16;
	p->ar2 = 9;
	p->framerate = 60.0;
	p->frm_no = 0;
	p->interlaced = 0;
	p->seq_len = 1;

	return p;
}


//! Destroy a picture object
/*! \param p        Pointer to picture (pic_t) object
	\return         NULL pointer */
void *pdestroy(pic_t *p)
{
	int i;

	if (p->color == RGB)
	{
		for (i = 0; i < p->h; i++)
		{
			free(p->data.rgb.r[i]);
			free(p->data.rgb.g[i]);
			free(p->data.rgb.b[i]);
			//free(p->data.rgb.a[i]);
		}
		free(p->data.rgb.r);
		free(p->data.rgb.g);
		free(p->data.rgb.b);
		//free(p->data.rgb.a);
	}
	else if (p->chroma == YUV_420)
	{
		for (i = 0; i < p->h; i++)
		{
			free(p->data.yuv.y[i]);
			//free(p->data.yuv.a[i]);
		}

		for (i = 0; i < p->h / 2; i++)
		{
			free(p->data.yuv.u[i]);
			free(p->data.yuv.v[i]);
		}
		free(p->data.yuv.y);
		free(p->data.yuv.u);
		free(p->data.yuv.v);
		//free(p->data.yuv.a);
	}
	else
	{
		for (i = 0; i < p->h; i++)
		{
			free(p->data.yuv.y[i]);
			free(p->data.yuv.u[i]);
			free(p->data.yuv.v[i]);
			//free(p->data.yuv.a[i]);
		}
		free(p->data.yuv.y);
		free(p->data.yuv.u);
		free(p->data.yuv.v);
		//free(p->data.yuv.a);
	}
	free(p);
	return(NULL);
}


//! Convert RGB to YCbCr (unsupported)
/*! \param ip      Input picture (pic_t)
    \param op      Output picture (pic_t) */
void rgb2yuv(pic_t *ip, pic_t *op)
{
    int i, j;
    float r, g, b;
    float y, u, v;
	 int black, half, max;

    if (ip->chroma != YUV_444)
    {
        fprintf(stderr, "ERROR: rgb2yuv() Incorrect input chroma type.\n");
        exit(1);
    }

    if (ip->color != RGB)
    {
        fprintf(stderr, "ERROR: rgb2yuv() Incorrect input color type.\n");
        exit(1);
    }

    if (ip->w != op->w || ip->h != op->h)
    {
        fprintf(stderr, "ERROR: rgb2yuv() Incorrect picture size.\n");
        exit(1);
    }

    if (op->chroma != YUV_444)
    {
        fprintf(stderr, "ERROR: rgb2yuv() Incorrect output chroma type.\n");
        exit(1);
    }

    if (op->color != YUV_SD && op->color != YUV_HD)
    {
        fprintf(stderr, "ERROR: rgb2yuv() Incorrect output color type.\n");
        exit(1);
    }
	if((ip->bits < 8) || (ip->bits > 16))
	{
		fprintf(stderr, "ERROR: rgb2yuv() Unsupported bit resolution (bits=%d).\n", ip->bits);
	exit(1);
	}

	black = 16 << (ip->bits - 8);
	half = 128 << (ip->bits - 8);
	max = (256 << (ip->bits - 8)) - 1;   

    for (i = 0; i < op->h; i++)
        for (j = 0; j < op->w; j++)
        {
            r = (float)ip->data.rgb.r[i][j];
            g = (float)ip->data.rgb.g[i][j];
            b = (float)ip->data.rgb.b[i][j];

            if (op->color == YUV_SD) // 601
            {
                y =  (float)(0.257 * r + 0.504 * g + 0.098 * b + black);
                u = (float)(-0.148 * r - 0.291 * g + 0.439 * b + half);
                v =  (float)(0.439 * r - 0.368 * g - 0.071 * b + half);
            }
            else if (op->color == YUV_HD) // 709
            {
                y =  (float)(0.183 * r + 0.614 * g + 0.062 * b + black);
                u = (float)(-0.101 * r - 0.338 * g + 0.439 * b + half);
                v =  (float)(0.439 * r - 0.399 * g - 0.040 * b + half);
            }
            else
            {
                fprintf(stderr, "ERROR: rgb2yuv() Incorrect output color type.\n");
                exit(1);
            }

            // rounding
            y += 0.5;
            u += 0.5;
            v += 0.5;

            op->data.yuv.y[i][j] = (int)CLAMP(y, 0, max);
            op->data.yuv.u[i][j] = (int)CLAMP(u, 0, max);
            op->data.yuv.v[i][j] = (int)CLAMP(v, 0, max);
        }
}


//! Convert YCbCr to RGB (unsupported)
/*! \param ip      Input picture (pic_t)
    \param op      Output picture (pic_t) */
void yuv2rgb(pic_t *ip, pic_t *op)
{
    int i, j;

    double r, g, b;
    float y, u, v;
	int black, half, max;

    if (ip->chroma != YUV_444)
    {
        fprintf(stderr, "ERROR: Incorrect input chroma type.\n");
        exit(1);
    }

    if (ip->color != YUV_SD && ip->color != YUV_HD)
    {
        fprintf(stderr, "ERROR: Incorrect input color type.\n");
        exit(1);
    }

    if (ip->w != op->w || ip->h != op->h)
    {
        fprintf(stderr, "ERROR: Incorrect picture size.\n");
	exit(1);
    }

    if (op->chroma != YUV_444)
    {
        fprintf(stderr, "ERROR: Incorrect output chroma type.\n");
        exit(1);
    }

    if (op->color != RGB)
    {
        fprintf(stderr, "ERROR: Incorrect output color type.\n");
        exit(1);
    }

	black = 16<<(ip->bits-8);
	half = 128<<(ip->bits-8);
	max = (256<<(ip->bits-8))-1;

    for (i = 0; i < op->h; i++)
        for (j = 0; j < op->w; j++)
        {
            y = (float)ip->data.yuv.y[i][j];
            u = (float)ip->data.yuv.u[i][j];
            v = (float)ip->data.yuv.v[i][j];

            if (ip->color == YUV_SD) // 601
            {
                r = 1.164 * (y - black) + 1.596 * (v - half);
                g = 1.164 * (y - black) - 0.813 * (v - half) - 0.391 * (u - half);
                b = 1.164 * (y - black)                     + 2.018 * (u - half);
            }
            else if (ip->color == YUV_HD) // 709
            {
                r = 1.164 * (y - black) + 1.793 * (v - half);
                g = 1.164 * (y - black) - 0.534 * (v - half) - 0.213 * (u - half);
                b = 1.164 * (y - black)                     + 2.115 * (u - half);
            }
            else
            {
                fprintf(stderr, "ERROR: Incorrect output color type.\n");
                exit(1);
            }

            r += 0.5;
            g += 0.5;
            b += 0.5;

            op->data.rgb.r[i][j] = (int)CLAMP(r, 0, max);
            op->data.rgb.g[i][j] = (int)CLAMP(g, 0, max);
            op->data.rgb.b[i][j] = (int)CLAMP(b, 0, max);
        }
}


//! Convert YCbCr 4:2:2 to YCbCr 4:4:4 (unsupported)
/*! \param ip      Input picture (pic_t)
    \param op      Output picture (pic_t) */
//
// Chroma 444 to 422 conversion.
//
void yuv_422_444(pic_t *ip, pic_t *op)
{
	int i, j, max;
	int m1, z, p1, p2;

	max = (1<<ip->bits)-1;

	for(i=0; i<ip->h; ++i)
	{
		for(j=0; j<ip->w/2; ++j)
		{
			op->data.yuv.y[i][2*j] = ip->data.yuv.y[i][2*j];
			op->data.yuv.y[i][2*j+1] = ip->data.yuv.y[i][2*j+1];
			op->data.yuv.u[i][2*j] = ip->data.yuv.u[i][j];
			op->data.yuv.v[i][2*j] = ip->data.yuv.v[i][j];
			m1 = ip->data.yuv.u[i][CLAMP(j-1,0,ip->w/2-1)];
			z = ip->data.yuv.u[i][j];
			p1 = ip->data.yuv.u[i][CLAMP(j+1,0,ip->w/2-1)];
			p2 = ip->data.yuv.u[i][CLAMP(j+2,0,ip->w/2-1)];
			op->data.yuv.u[i][2*j+1] = CLAMP((9 * (z + p1) - (m1 + p2) + 8) >> 4, 0, max);
			m1 = ip->data.yuv.v[i][CLAMP(j-1,0,ip->w/2-1)];
			z = ip->data.yuv.v[i][j];
			p1 = ip->data.yuv.v[i][CLAMP(j+1,0,ip->w/2-1)];
			p2 = ip->data.yuv.v[i][CLAMP(j+2,0,ip->w/2-1)];
			op->data.yuv.v[i][2*j+1] = CLAMP((9 * (z + p1) - (m1 + p2) + 8) >> 4, 0, max);
		}
	}
}


//! Convert YCbCr 4:4:4 to YCbCr 4:2:2 (unsupported)
/*! \param ip      Input picture (pic_t)
    \param op      Output picture (pic_t) */
void yuv_444_422(pic_t *ip, pic_t *op)
{
	fprintf(stderr, "ERROR: 444 to 422 conversion not supported.\n");
	exit(1);
}


//! Convert YCbCr 4:2:2 to YCbCr 4:2:0 (unsupported)
/*! \param ip      Input picture (pic_t)
    \param op      Output picture (pic_t) */
void yuv_422_420(pic_t *ip, pic_t *op)
{
	fprintf(stderr, "ERROR: 422 to 420 conversion not supported.\n");
	exit(1);
}


//! Convert YCbCr 4:2:0 to YCbCr 4:2:2 (unsupported)
/*! \param ip      Input picture (pic_t)
    \param op      Output picture (pic_t) */
void yuv_420_422(pic_t *ip, pic_t *op)
{
    int   i, j, max, w;
    int u, v;

    if (ip->chroma != YUV_420)
    {
		printf("ERROR: Incorrect input chroma type.\n");
	exit(1);
    }
    else
    {
	    if (ip->w != op->w || ip->h != op->h)
	    {
			printf("ERROR: Incorrect picture size.\n");
			exit(1);
		}

	    op->chroma = YUV_422; // force to 4:2:0

		max = (1<<ip->bits)-1;

	//
	// Y
	//
		for (i = 0; i < op->h; i++)
			for (j = 0; j < op->w; j++)
				op->data.yuv.y[i][j] = ip->data.yuv.y[i][j];

	//
	// Cb
	//
		for (i = 0; i < op->h; i++)
			for (j = 0; j < op->w / 2; j++)
			{
				if(i%2) w=3; else w=1;
				u = (ip->data.yuv.u[CLAMP((i+1)/2-1,0,ip->h/2-1)][j] * w + ip->data.yuv.u[CLAMP((i+1)/2,0,ip->h/2-1)][j] * (4-w) + 2) >> 2;
				op->data.yuv.u[i][j] = (int)CLAMP(u, 0, max);
			}

	//
	// Cr
	//
		for (i = 0; i < op->h; i++)
			for (j = 0; j < op->w / 2; j++)
			{
				if(i%2) w=3; else w=1;
				v = (ip->data.yuv.v[CLAMP((i+1)/2-1,0,ip->h/2-1)][j] * w + ip->data.yuv.v[CLAMP((i+1)/2,0,ip->h/2-1)][j] * (4-w) + 2) >> 2;
				op->data.yuv.v[i][j] = (int)CLAMP(v, 0, max);
			}
	}
}


void convertbits(pic_t *p, int newbits)
{
	int ii, jj;
	int error = 0;
	int rs = 0, ls = 0;

	if(p->bits > newbits)
		rs = p->bits - newbits;
	else
		ls = newbits - p->bits;

	if (p->color == RGB) {           
		if (p->chroma == YUV_444) {
			// RGB 4:4:4
			for(ii=0; ii<p->h; ++ii) {
				for(jj=0; jj<p->w; ++jj) {
					p->data.rgb.r[ii][jj] = (p->data.rgb.r[ii][jj]<<ls)>>rs;
					p->data.rgb.g[ii][jj] = (p->data.rgb.g[ii][jj]<<ls)>>rs;
					p->data.rgb.b[ii][jj] = (p->data.rgb.b[ii][jj]<<ls)>>rs;
					//p->data.rgb.a[ii][jj] = (p->data.rgb.a[ii][jj]<<ls)>>rs;
				}
			}
		} else {
			// we don't handle any RGB that's not 4:4:4 at this point
			printf(" RGB 422 not supported yet.\n");
			error = 1;
		}
	} else {
		// YUV format (either SD or HD)
		if (p->chroma == YUV_444) {
			// YUV 4:4:4
			for(ii=0; ii<p->h; ++ii) {
				for(jj=0; jj<p->w; ++jj) {
					p->data.yuv.y[ii][jj] = (p->data.yuv.y[ii][jj]<<ls)>>rs;
					p->data.yuv.u[ii][jj] = (p->data.yuv.u[ii][jj]<<ls)>>rs;
					p->data.yuv.v[ii][jj] = (p->data.yuv.v[ii][jj]<<ls)>>rs;
				}
			}
		} else {
			if (p->chroma == YUV_422) {
				for(ii=0; ii<p->h; ++ii) {
					for(jj=0; jj<(p->w/2); ++jj) {
						p->data.yuv.u[ii][jj] = (p->data.yuv.u[ii][jj]<<ls)>>rs;
						p->data.yuv.y[ii][jj*2] = (p->data.yuv.y[ii][jj*2]<<ls)>>rs;
						p->data.yuv.v[ii][jj] = (p->data.yuv.v[ii][jj]<<ls)>>rs;
						p->data.yuv.y[ii][jj*2+1] = (p->data.yuv.y[ii][jj*2+1]<<ls)>>rs;
					}
				}
			} else if (p->chroma == YUV_420) {
				for(ii=0; ii<p->h/2; ++ii) {
					for(jj=0; jj<(p->w/2); ++jj) {
						p->data.yuv.u[ii][jj] = (p->data.yuv.u[ii][jj]<<ls)>>rs;
						p->data.yuv.v[ii][jj] = (p->data.yuv.v[ii][jj]<<ls)>>rs;
						p->data.yuv.y[ii*2][jj*2] = (p->data.yuv.y[ii*2][jj*2]<<ls)>>rs;
						p->data.yuv.y[ii*2][jj*2+1] = (p->data.yuv.y[ii*2][jj*2+1]<<ls)>>rs;
						p->data.yuv.y[ii*2+1][jj*2] = (p->data.yuv.y[ii*2+1][jj*2]<<ls)>>rs;
						p->data.yuv.y[ii*2+1][jj*2+1] = (p->data.yuv.y[ii*2+1][jj*2+1]<<ls)>>rs;
					}
				}
			} else 	{
				error = 1;
			}
		}
	}

	if (error) {
		fprintf(stderr, "ERROR: Calling convert8to10() with incompatible format.  Only handle RGB444 or YUV422 or YUV444.\n");
		exit(1);
	}
	p->bits = newbits;
}

void gettoken(FILE *fp, char *token, char *line, int *pos)
{
	char c;
	int count = 0;

	// Get whitespace
	c = line[(*pos)++];
	while((c=='\0')||(c==' ')||(c=='\t')||(c==10)||(c==13)||(c=='#'))
	{
		if(c=='\0' || c=='\n' || c=='#')
		{
			if (fgets(line, 1000, fp)==NULL)
			{
				token[0] = '\0';
				return;
			}
			*pos = 0;
		}
		c = line[(*pos)++];
	}

	// Get token
	while(count<999 && !((c=='\0')||(c==' ')||(c=='\t')||(c==10)||(c==13)||(c=='#')))
	{
		token[count++] = c;
		c = line[(*pos)++];
	}
	token[count] = '\0';
}

pic_t *readppm(FILE *fp)
{
    pic_t *p;

    char magicnum[128];
    char line[1000];
	char token[1000];

    int w, h;
    int i, j;
    int g = 0;
    int maxval;
	int pos = 0;

    if (fgets(line, 1000, fp)==NULL)
		Err("Error reading PPM file\n");
	gettoken(fp, token, line, &pos);

    if (token[0] != 'P')
    {
        Err("Incorrect file type.");
    }
	strcpy(magicnum, token);

	gettoken(fp, token, line, &pos);
    w = atoi(token);
	gettoken(fp, token, line, &pos);
    h = atoi(token);
	gettoken(fp, token, line, &pos);
	maxval = atoi(token);

    p = pcreate(FRAME, RGB, YUV_444, w, h);
    if(maxval <= 255)
		p->bits = 8;
    else if(maxval <= 1023)
		p->bits = 10;
    else if(maxval <= 4095)
		p->bits = 12;
	else if(maxval <= 16383)
		p->bits = 14;
    else if(maxval <= 65535)
		p->bits = 16;
    else
    {
		printf("PPM read error, maxval = %d\n", maxval);
		pdestroy(p);
		return(NULL);
    }

    if (magicnum[1] == '2')
        for (i = 0; i < h; i++)
            for (j = 0; j < w; j++)
            {
                if (fscanf(fp, "%d", &g) != 1)  // Gray value in PGM
					Err("Error reading PPM (PGM) file\n");
                p->data.rgb.r[i][j] = g;
                p->data.rgb.g[i][j] = g;
                p->data.rgb.b[i][j] = g;
            }
    else if (magicnum[1] == '3')
	{
		int c, v;
		i = 0; j = 0; c = 0;
		while( i < h )
		{
			char *rest;
			do
			{
				int sz = -1;
				do
				{
					sz++;
					line[sz] = fgetc(fp);
					if(feof(fp))
						line[++sz] = '\n';
				} while((line[sz] != '\n') && ((sz < 900) || ((line[sz] >= '0') && (line[sz] <= '9'))));
				line[sz+1] = '\0';
			} while (line[0] == '#');
			rest = line;
			while((rest[0] != '\0') && ((rest[0] < '0') || (rest[0] > '9'))) rest++;
			while(rest[0] != '\0')
			{
				v = 0;
				while((rest[0] >= '0') && (rest[0] <= '9'))
				{
					v = 10*v + (rest[0] - '0');
					rest++;
				}
				if(c==0)      p->data.rgb.r[i][j] = v;
				else if(c==1) p->data.rgb.g[i][j] = v;
				else if(c==2) p->data.rgb.b[i][j] = v;
				c++;
				if(c>2)
				{
					c = 0; j++;
					if(j>=w)
					{
						j = 0; i++;
					}
				}
				while((rest[0] != '\0') && ((rest[0] < '0') || (rest[0] > '9'))) rest++;
			}
		}
    }
    else if (magicnum[1] == '5') // PGM binary
        for (i = 0; i < h; i++)
            for (j = 0; j < w; j++)
            {
		g = (unsigned char)fgetc(fp);  // Gray value
		if(maxval > 255)
		    g = (g<<8) + (unsigned char)fgetc(fp);
                p->data.rgb.r[i][j] = g;
                p->data.rgb.g[i][j] = g;
                p->data.rgb.b[i][j] = g;
            }
    else // P6
	{
        for (i = 0; i < h; i++)
		{
            for (j = 0; j < w; j++)
            {
                p->data.rgb.r[i][j] = (unsigned char) fgetc(fp);
				if(maxval > 255)
					p->data.rgb.r[i][j] = (p->data.rgb.r[i][j] << 8) + (unsigned char)fgetc(fp);
				p->data.rgb.g[i][j] = (unsigned char) fgetc(fp);
				if(maxval > 255)
					p->data.rgb.g[i][j] = (p->data.rgb.g[i][j] << 8) + (unsigned char)fgetc(fp);
				p->data.rgb.b[i][j] = (unsigned char) fgetc(fp);
				if(maxval > 255)
					p->data.rgb.b[i][j] = (p->data.rgb.b[i][j] << 8) + (unsigned char)fgetc(fp);
            }
		}
	}

    return p;
}

int ppm_read(char *fname, pic_t **pic)
{
	FILE *fp;

	fp = fopen(fname, "rb");

	if(fp == NULL)
		return(-1);  // Error condition

	*pic = readppm(fp);

	fclose(fp);
	return(0);
}

void writeppm(FILE *fp, pic_t *p)
{
    int i, j;

    fprintf(fp, "P6\n");
    fprintf(fp, "%d %d\n", p->w, p->h);
	fprintf(fp, "%d\n", (1<<p->bits)-1);

    for (i = 0; i < p->h; i++)
        for (j = 0; j < p->w; j++)
        {
			if(p->bits>8)
				fputc((unsigned char)(p->data.rgb.r[i][j] >> 8), fp);
            fputc((unsigned char) p->data.rgb.r[i][j] & 0xff, fp);
			if(p->bits>8)
				fputc((unsigned char)(p->data.rgb.g[i][j] >> 8), fp);
            fputc((unsigned char) p->data.rgb.g[i][j] & 0xff, fp);
			if(p->bits>8)
				fputc((unsigned char)(p->data.rgb.b[i][j] >> 8), fp);
            fputc((unsigned char) p->data.rgb.b[i][j] & 0xff, fp);
        }
}


int ppm_write(char *fname, pic_t *pic)
{
	FILE *fp;
	pic_t *temp_pic, *temp_pic2;

	fp = fopen(fname, "wb");

	if(fp == NULL)
		return(-1);  // Error condition

	if(pic->chroma == YUV_420)
	{
		temp_pic = pcreate(0, pic->color, YUV_422, pic->w, pic->h);
		temp_pic->bits = pic->bits;
		yuv_420_422(pic, temp_pic);
		temp_pic2 = pcreate(0, pic->color, YUV_444, pic->w, pic->h);
		temp_pic2->bits = pic->bits;
		yuv_422_444(temp_pic, temp_pic2);
		pdestroy(temp_pic);
		temp_pic = pcreate(0, RGB, YUV_444, pic->w, pic->h);
		temp_pic->bits = pic->bits;
		yuv2rgb(temp_pic2, temp_pic);
		writeppm(fp, temp_pic);
		pdestroy(temp_pic);
		pdestroy(temp_pic2);
	}
	else if(pic->chroma == YUV_422)
	{
		temp_pic2 = pcreate(0, pic->color, YUV_444, pic->w, pic->h);
		temp_pic2->bits = pic->bits;
		yuv_422_444(pic, temp_pic2);
		temp_pic = pcreate(0, RGB, YUV_444, pic->w, pic->h);
		temp_pic->bits = pic->bits;
		yuv2rgb(temp_pic2, temp_pic);
		writeppm(fp, temp_pic);
		pdestroy(temp_pic);
		pdestroy(temp_pic2);
	}
	else if(pic->color != RGB)
	{
		temp_pic = pcreate(0, RGB, YUV_444, pic->w, pic->h);
		temp_pic->bits = pic->bits;
		yuv2rgb(pic, temp_pic);
		writeppm(fp, temp_pic);
		pdestroy(temp_pic);
	}
	else
		writeppm(fp, pic);

	fclose(fp);
	return(0);
}


int uyvy_read(pic_t *ip, FILE *fp)
{
	int i, j;
	int mode16bit;
	int shift;

	mode16bit = ip->bits > 8;
	shift = mode16bit ? (16-ip->bits) : 0;
	for(i=0; i<ip->h; ++i)
		for(j=0; j<ip->w/2; ++j)
		{
			ip->data.yuv.u[i][j] = fgetc(fp);
			if(mode16bit) ip->data.yuv.u[i][j] |= fgetc(fp)<<8;
			ip->data.yuv.y[i][2*j] = fgetc(fp);
			if(mode16bit) ip->data.yuv.y[i][2*j] |= fgetc(fp)<<8;
			ip->data.yuv.v[i][j] = fgetc(fp);
			if(mode16bit) ip->data.yuv.v[i][j] |= fgetc(fp)<<8;
			ip->data.yuv.y[i][2*j+1] = fgetc(fp);
			if(mode16bit) ip->data.yuv.y[i][2*j+1] |= fgetc(fp)<<8;
			ip->data.yuv.u[i][j] >>= shift;
			ip->data.yuv.y[i][2*j] >>= shift;
			ip->data.yuv.v[i][j] >>= shift;
			ip->data.yuv.y[i][2*j+1] >>= shift;
		}
	fclose(fp);
	return(0);
}


int yuv_read(char *fname, pic_t **ip, int width, int height, int framenum, int bpc, int *numframes, int filetype)
{
	FILE *fp;
	int i, j;
	int framesize;
	int mode16bit;
	int maxval;

	mode16bit = (bpc>8);
	maxval = (1<<bpc)-1;

	if((fp = fopen(fname, "rb")) == NULL)
	{
		printf("Could not read YUV file %s\n", fname);
		return(1);
	}

	*ip = pcreate(0, YUV_HD, (filetype == 1) ? YUV_422 : YUV_420, width, height);
	(*ip)->bits = bpc;  

	if(filetype == 1)
	{
		*numframes = 1;
		return(uyvy_read(*ip, fp));
	}

	framesize = width*height*3 / (mode16bit ? 1 : 2);  // number of bytes
#ifdef WIN32
	_fseeki64(fp, 0, SEEK_END);
	*numframes = (int)((_ftelli64(fp)+1) / (long long)framesize);
	if(*numframes == 0)
	{
		// for some reason the _ftelli64 fails sometimes
	}
	_fseeki64(fp, (__int64)framesize*framenum, SEEK_SET);
#else
	fseeko(fp, 0, SEEK_END);
	{
		//struct stat st;
		//*numframes = (int)((st.st_size+1) / framesize);
		*numframes = (int)((ftello(fp)+1) / framesize);
		fseeko(fp, (off_t)framesize*framenum, SEEK_SET);
	}
#endif

	for(i=0; i<height; ++i)
		for(j=0; j<width; ++j)
		{
			(*ip)->data.yuv.y[i][j] = fgetc(fp);
			if(mode16bit)
				(*ip)->data.yuv.y[i][j] |= (fgetc(fp) << 8);
			if ((*ip)->data.yuv.y[i][j] < 0 || (*ip)->data.yuv.y[i][j] > maxval)
			{
				printf("YUV read error: sample value did not match expected bit depth\n");
				return (1);
			}
		}
	for(i=0; i<height/2; ++i)
		for(j=0; j<width/2; ++j)
		{
			(*ip)->data.yuv.u[i][j] = fgetc(fp);
			if(mode16bit)
				(*ip)->data.yuv.u[i][j] |= (fgetc(fp) << 8);
		}
	for(i=0; i<height/2; ++i)
		for(j=0; j<width/2; ++j)
		{
			(*ip)->data.yuv.v[i][j] = fgetc(fp);
			if(mode16bit)
				(*ip)->data.yuv.v[i][j] |= (fgetc(fp) << 8);
		}

	fclose(fp);
	return(0);
}


int uyvy_write(pic_t *op, FILE *fp)
{
	int i, j;
	int mode16bit;
	int shift, c;

	mode16bit = op->bits > 8;
	shift = mode16bit ? (16 - op->bits) : 0;
	for(i=0; i<op->h; ++i)
		for(j=0; j<op->w/2; ++j)
		{
			c = op->data.yuv.u[i][j] << shift;
			fputc(c & 0xff, fp);
			if(mode16bit) fputc((c>>8) & 0xff, fp);
			c = op->data.yuv.y[i][2*j] << shift;
			fputc(c & 0xff, fp);
			if(mode16bit) fputc((c>>8) & 0xff, fp);
			c = op->data.yuv.v[i][j] << shift;
			fputc(c & 0xff, fp);
			if(mode16bit) fputc((c>>8) & 0xff, fp);
			c = op->data.yuv.y[i][2*j+1] << shift;
			fputc(c & 0xff, fp);
			if(mode16bit) fputc((c>>8) & 0xff, fp);
		}

	fclose(fp);
	return(0);
}


int yuv_write(char *fname, pic_t *op, int framenum, int filetype)
{
	FILE *fp;
	int i, j;
	int framesize;
	int width, height;
	int mode16bit;

	mode16bit = (op->bits>8);

	if(framenum==0)
	{
		if((fp = fopen(fname, "wb")) == NULL)
		{
			printf("Could not write YUV file %s\n", fname);
			return(1);
		}
	} else {
		if((fp = fopen(fname, "ab")) == NULL)
		{
			printf("Could not write YUV file %s\n", fname);
			return(1);
		}
	}

	if(filetype == 1)
		return(uyvy_write(op, fp));

	width = op->w;  height = op->h;
	framesize = width*height*3/(mode16bit ? 1 : 2);  // number of bytes
	fseek(fp, framesize*framenum, SEEK_SET);

	for(i=0; i<height; ++i)
		for(j=0; j<width; ++j)
		{
			if(mode16bit)
			{
				fputc(op->data.yuv.y[i][j] & 0xff, fp);
				fputc(op->data.yuv.y[i][j] >> 8, fp);
			} else
				fputc(op->data.yuv.y[i][j], fp);
		}
	for(i=0; i<height/2; ++i)
		for(j=0; j<width/2; ++j)
		{
			if(mode16bit)
			{
				fputc(op->data.yuv.u[i][j] & 0xff, fp);
				fputc(op->data.yuv.u[i][j] >> 8, fp);
			} else
				fputc(op->data.yuv.u[i][j], fp);
		}
	for(i=0; i<height/2; ++i)
		for(j=0; j<width/2; ++j)
		{
			if(mode16bit)
			{
				fputc(op->data.yuv.v[i][j] & 0xff, fp);
				fputc(op->data.yuv.v[i][j] >> 8, fp);
			} else
				fputc(op->data.yuv.v[i][j], fp);
		}
	fclose(fp);
	return(0);
}

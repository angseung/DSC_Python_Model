/***************************************************************************
*    Copyright (c) 2013-2016, Broadcom Ltd.
*    All rights reserved.
*
*  Statement regarding contribution of copyrighted materials to VESA:
*
*  This code is owned by Broadcom Limited and is contributed to VESA
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

/*! \file dsc_codec.c
 *    DSC codec
 *  \author Frederick Walls (fwalls@broadcom.com)
 *  \author Sandy MacInnis (macinnis@broadcom.com)
 *  \author and others at Broadcom
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>
#include <assert.h>
#include "dsc_utils.h"
#include "dsc_codec.h"
#include "dsc_types.h"
#include "multiplex.h"

//#define USE_MSE_FOR_ICH  1

//#define PRINTDEBUG
#define PRINT_DEBUG_VLC	  0
#define PRINT_DEBUG_RC    0
#define PRINT_DEBUG_RECON 0
int g_verbose;

// Prototypes
int MapQpToQlevel(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int qp, int CType);
int FindResidualSize(int eq);
int SampToLineBuf( dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int x, int CType);
int IsOrigFlatHIndex(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int hPos);
int PredictSize(dsc_cfg_t *dsc_cfg, int *req_size);

//-----------------------------------------------------------------------------
// debug dumping

FILE *g_fp_dbg = 0;

//-----------------------------------------------------------------------------
// The following are constants that are used in the code:
const int QuantDivisor[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536 };
const int QuantOffset[] =  {0, 0, 1, 3,  7, 15, 31,  63, 127, 255,  511, 1023, 2047, 4095,  8191, 16383, 32767 };
int qlevel_luma_8bpc[] = {0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 5, 6, 7 };
int qlevel_chroma_8bpc[] = {0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 8, 8, 8 };
int qlevel_luma_10bpc[] = {0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 7, 8, 9 };
int qlevel_chroma_10bpc[] = {0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 10, 10, 10 };
int qlevel_luma_12bpc[] = {0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 9, 10, 11 };
int qlevel_chroma_12bpc[] = {0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 12, 12, 12 };
int qlevel_luma_14bpc[] =   {0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,  9,  9,  10, 10, 11, 11, 11, 12, 13 };
int qlevel_chroma_14bpc[] = {0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 14, 14, 14 };
int qlevel_luma_16bpc[] =   {0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8,  9,  9,  10, 10, 11, 11, 12, 12, 13, 13, 13, 14, 15 };
int qlevel_chroma_16bpc[] = {0, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14, 15, 16, 16, 16 };


//! Fill out the rest of the slice with valid pixels when an error occurs. Note the standard does not specify what the pixels should be & this is just an example.
/*! \param dsc_cfg DSC parameters
    \param dsc_state DSC state structure */
void ErrorHandler(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int hPos, int vPos, pic_t *op)
{
	int cpnt;
	int px, py, w;

	while(vPos < dsc_cfg->slice_height)
	{
		while(hPos < dsc_state->sliceWidth)
		{
			for(cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
			{
				px = dsc_cfg->xstart + hPos;
				py = dsc_cfg->ystart + vPos;
				w = op->w;
				if (dsc_cfg->native_420 || dsc_cfg->native_422)
					px = (dsc_cfg->xstart>>1) + hPos;
				if(dsc_cfg->native_422)
					w = op->w/2;
				if((px < w) && (py < op->h))
				{
					switch(cpnt)
					{
					case 0:
						op->data.yuv.y[py][dsc_cfg->native_422 ? (px*2) : px] = dsc_state->prevLine[cpnt][PADDING_LEFT+px];
						break;
					case 1:
						op->data.yuv.u[py][px] = dsc_state->prevLine[cpnt][PADDING_LEFT+px];
						break;
					case 2:
						op->data.yuv.v[py][px] = dsc_state->prevLine[cpnt][PADDING_LEFT+px];
						break;
					case 3:  // 2nd luma for native 4:2:2
						op->data.yuv.y[py][px*2+1] = dsc_state->prevLine[cpnt][PADDING_LEFT+px];
						break;
					}
				}
			}
			hPos++;
		}
		hPos = 0;
		vPos++;
	}
}


//! Map QP to quantization level
/*! \param dsc_state DSC state structure
    \param qp      QP to map
    \param cpnt    Component to map
    \return        Corresponding qlevel */
int MapQpToQlevel(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int qp, int cpnt)
{
	int qlevel;

	// *MODEL NOTE* MN_MAP_QP_TO_QLEVEL
	///////////////////////////////////////////////////////////////////
	// Mapping QP to qLevel (6.8.6)
	// This 'qLevel' Will be Used to Full Data
	///////////////////////////////////////////////////////////////////
	// 3 pixels per cycle!
	if ((cpnt%3) == 0)			// i.e., cpnt = 0, or 3 in native 4:2:2 mode
		qlevel = dsc_state->quantTableLuma[qp];
	else if ((dsc_cfg->native_420) && (cpnt == 1))
		qlevel = dsc_state->quantTableLuma[qp];
	else
	{
		qlevel = dsc_state->quantTableChroma[qp];
		if((dsc_cfg->dsc_version_minor==2) && (dsc_state->cpntBitDepth[0] == dsc_state->cpntBitDepth[1]))  // QP adjustment for YCbCr mode, Default : YCgCo
			qlevel = MAX(0, qlevel-1);
	}

	return (qlevel);
}


//! Quantize a residual
/*! \param e       Raw residual
    \param qlevel  Quantization level
    \return        Quantized residual */
int QuantizeResidual(int e, int qlevel)
{
	int eq;

	// *MODEL NOTE* MN_ENC_QUANTIZATION
	//////////////////////////////////////////////////////////////////
	// Quantization (6.4.5)
	// Calculate Residual Using Predicted Pixel Value 
	// Residual = (Input Pixel) - (Predicted Pixel)
	// Then Encode This Residual Value
	//////////////////////////////////////////////////////////////////
	if (e>0)
		eq = (e + QuantOffset[qlevel]) >> qlevel;
	else
		eq = -((QuantOffset[qlevel]-e) >> qlevel);
	return eq;
}


//! Get maximum residual size for a given component & quantization level
/*! \param dsc_state DSC state structure
	\param cpnt      Which component
	\param qp        Quantization parameter 
	\return          Max residual size in bits */
int MaxResidualSize(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int cpnt, int qp)
{
	int qlevel;

	qlevel = MapQpToQlevel(dsc_cfg, dsc_state, qp, cpnt);

	return (dsc_state->cpntBitDepth[cpnt] - qlevel);
}


//! Get predicted size for unit (adjusted for QP changes)
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure
	\param unit      Which unit
    \return          Predicted size for unit */
int GetQpAdjPredSize(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int unit)
{
	int pred_size, max_size;
	int qlevel_old, qlevel_new;
	int cpnt;

	cpnt = dsc_state->unitCType[unit];
	qlevel_new = MapQpToQlevel(dsc_cfg, dsc_state, dsc_state->masterQp, cpnt);

	// *MODEL NOTE* MN_DSU_SIZE_PREDICTION
	pred_size = dsc_state->predictedSize[unit];

	qlevel_old = MapQpToQlevel(dsc_cfg, dsc_state, dsc_state->prevMasterQp, cpnt);

	pred_size += qlevel_old - qlevel_new;
	max_size = MaxResidualSize(dsc_cfg, dsc_state, cpnt, dsc_state->masterQp);
	pred_size = CLAMP(pred_size, 0, max_size-1);

	return (pred_size);
}


//! Get the predicted sample value
/*! \param prevLine  Array of samples from previous (reconstructed) line
	\param currLine  Array of samples from current (reconstructed) line
	\param hPos      Horizontal position within slice of sample to predict
	\param predType  Prediction mode to use (PT_MAP or one of PT_BLOCK)
	\param qLevel    Quantization level for current component
	\param cpnt      Which component
	\return          Predicted sample value */
int SamplePredict(
	dsc_state_t *dsc_state,
	int* prevLine,          // reconstructed samples for previous line
	int* currLine,          // reconstructed samples for current line
	int hPos,               // horizontal position for sample to predict
	PRED_TYPE predType,     // predictor to use
	int qLevel,
	int unit)
{
	// Values for MMAP Calculation
	int a, b, c, d, e;
	int filt_b, filt_c, filt_d, filt_e;
	int blend_b, blend_c, blend_d, blend_e;
	int p; //Predicted Pixel Value ; OUTPUT!
	int bp_offset;
	int diff = 0;
	int h_offset_array_idx;
	int cpnt;

	cpnt = dsc_state->unitCType[unit];
	h_offset_array_idx = (hPos / SAMPLES_PER_UNIT) * SAMPLES_PER_UNIT + PADDING_LEFT; 

	// organize samples into variable array defined in dsc spec
	c = prevLine[h_offset_array_idx-1];
	b = prevLine[h_offset_array_idx];
	d = prevLine[h_offset_array_idx+1];
	e = prevLine[h_offset_array_idx+2];
	a = currLine[h_offset_array_idx-1];

#define FILT3(a,b,c) (((a)+2*(b)+(c)+2)>>2)
	filt_c = FILT3(prevLine[h_offset_array_idx-2], prevLine[h_offset_array_idx-1], prevLine[h_offset_array_idx]);
	filt_b = FILT3(prevLine[h_offset_array_idx-1], prevLine[h_offset_array_idx], prevLine[h_offset_array_idx+1]);
	filt_d = FILT3(prevLine[h_offset_array_idx], prevLine[h_offset_array_idx+1], prevLine[h_offset_array_idx+2]);
	filt_e = FILT3(prevLine[h_offset_array_idx+1], prevLine[h_offset_array_idx+2], prevLine[h_offset_array_idx+3]);

	switch (predType) {
	case PT_MAP:	// MAP prediction
		// *MODEL NOTE* MN_MMAP
		////////////////////////////////////////////////////////////////////////////
		// Prediction Part 
		// Full "qLevel" bits provided from Rate Contol Part then Encode it
		// Modified Median-Adaptive Prediction (6.4.1, 6.4.1.1 ~ 6.4.1.2)
		////////////////////////////////////////////////////////////////////////////
		diff = CLAMP(filt_c - c, -(QuantDivisor[qLevel]/2), QuantDivisor[qLevel]/2);
		blend_c = c + diff;
		diff = CLAMP(filt_b - b, -(QuantDivisor[qLevel]/2), QuantDivisor[qLevel]/2);
		blend_b = b + diff;
		diff = CLAMP(filt_d - d, -(QuantDivisor[qLevel]/2), QuantDivisor[qLevel]/2);
		blend_d = d + diff;
		diff = CLAMP(filt_e - e, -(QuantDivisor[qLevel]/2), QuantDivisor[qLevel]/2);
		blend_e = e + diff;
		
		// Pixel on line above off the raster to the left gets same value as pixel below (ie., midpoint)
		if (hPos/SAMPLES_PER_UNIT == 0)
			blend_c = a; //WHY?
		
		if ((hPos % SAMPLES_PER_UNIT)==0)  // First pixel of group (P0)
			p = CLAMP(a + blend_b - blend_c, MIN(a, blend_b), MAX(a, blend_b));
		else if ((hPos % SAMPLES_PER_UNIT)==1)   // Second pixel of group (P1)
			p = CLAMP(a + blend_d - blend_c + (dsc_state->quantizedResidual[unit][0] * QuantDivisor[qLevel]),
				        MIN(MIN(a, blend_b), blend_d), MAX(MAX(a, blend_b), blend_d));
		else    // Third pixel of group (P2)
			p = CLAMP(a + blend_e - blend_c + (dsc_state->quantizedResidual[unit][0] + dsc_state->quantizedResidual[unit][1])*QuantDivisor[qLevel],
						MIN(MIN(a,blend_b), MIN(blend_d, blend_e)), MAX(MAX(a,blend_b), MAX(blend_d, blend_e)));
		break;

	case PT_LEFT:
		// First Line of Slice!
		p = a;    // First pixel of group
		if ((hPos % SAMPLES_PER_UNIT)==1)   // Second pixel of group
			p = CLAMP(a + (dsc_state->quantizedResidual[unit][0] * QuantDivisor[qLevel]), 0, (1<<dsc_state->cpntBitDepth[cpnt])-1);
		else if((hPos % SAMPLES_PER_UNIT)==2)  // Third pixel of group
			p = CLAMP(a + (dsc_state->quantizedResidual[unit][0] + dsc_state->quantizedResidual[unit][1])*QuantDivisor[qLevel],
						0, (1<<dsc_state->cpntBitDepth[cpnt])-1);
		break;

	default:  // PT_BLOCK+ofs = BLOCK predictor, starts at -1
		// *MODEL NOTE* MN_BLOCK_PRED
		/////////////////////////////////////////
		// Block Prediction (6.4.2)
		/////////////////////////////////////////
		bp_offset = (int)predType - (int)PT_BLOCK;
		p = currLine[MAX(hPos + PADDING_LEFT - 1 - bp_offset,0)]; // Predicted Value is reconstructed pixel value at a few index ahead in current line.
		break;
	}

	return p;
}


//! Encoder function to estimate bits required to code original pixels
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure
	\return          Bits estimate to code original pixels */
int EstimateBitsForGroup(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state)
{
	int i, cpnt, unit;
	int max_size[MAX_UNITS_PER_GROUP];
	int total_size = 0;
	int size, qerr;
	int max_residual_size;
	int pred_size;
	int hPos;

	for (i=0; i<MAX_UNITS_PER_GROUP; ++i)
		max_size[i] = 0;

	for (unit = 0; unit < dsc_state->unitsPerGroup; ++unit)
	{
		for (i=0; i<SAMPLES_PER_UNIT; ++i)
		{
			hPos = dsc_state->hPos + i - (dsc_state->pixelsInGroup-1) + dsc_state->unitStartHPos[unit];
			if (hPos >= dsc_state->sliceWidth)
				continue;
			qerr = dsc_state->quantizedResidual[unit][i];		// use real P-mode residual sizes
			size = FindResidualSize(qerr);
			if (size > max_size[unit])
				max_size[unit] = size;
		}
		max_residual_size = MaxResidualSize(dsc_cfg, dsc_state, dsc_state->unitCType[unit], dsc_state->masterQp);
		if (max_size[unit] > max_residual_size)
			max_size[unit] = max_residual_size;
	}

	for (unit = 0; unit < dsc_state->unitsPerGroup; ++unit)
	{
		cpnt = dsc_state->unitCType[unit];
		pred_size = GetQpAdjPredSize(dsc_cfg, dsc_state, unit);
		if (max_size[unit] < pred_size)
			total_size += 1 + SAMPLES_PER_UNIT * pred_size;
		else if((max_size[unit] == MaxResidualSize(dsc_cfg, dsc_state, cpnt, dsc_state->masterQp)) && (unit != 0)) // not 1st unit
			total_size += (max_size[unit] - pred_size) + SAMPLES_PER_UNIT * max_size[unit];
		else        
			total_size += 1 + (max_size[unit] - pred_size) + SAMPLES_PER_UNIT * max_size[unit];
                                                                
	}
	//If previous group was ICH and luma is not maxsize, add another bit
	if( (max_size[0] < MaxResidualSize(dsc_cfg, dsc_state, 0, dsc_state->masterQp)) && dsc_state->prevIchSelected)
		total_size += 1;

	return (total_size);
}


//! Size to code that means "escape code"
/*! \param dsc_state DSC state structure 
	\param qp        Quantization parameter for group */
int EscapeCodeSize(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int qp)
{
	int qlevel, alt_size_to_generate;

	qlevel = MapQpToQlevel(dsc_cfg, dsc_state, qp, 0);    // map to luma quant
	alt_size_to_generate = dsc_state->cpntBitDepth[0] + 1 - qlevel;

	return (alt_size_to_generate);
}


//! Encoder function that returns 1 if midpoint prediction will be used for the specified component
/*! \param dsc_state DSC state structure 
	\param cpnt      Component to check */
int UsingMidpoint(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int unit, int cpnt)
{
	int qlevel, max_size, req_size[SAMPLES_PER_UNIT];
	int i;

	// Determine required size for unit
	qlevel = MapQpToQlevel(dsc_cfg, dsc_state, dsc_state->masterQp, cpnt);
	max_size = 0;
	for ( i=0; i<SAMPLES_PER_UNIT; i++ ) 
	{
		req_size[i] = FindResidualSize( dsc_state->quantizedResidual[unit][i] );
		max_size = MAX( req_size[i], max_size );
	}

	// Check if required size is bpc-qlevel ...
	if ( max_size >= dsc_state->cpntBitDepth[cpnt] - qlevel )  // ...if so, just use midpoint predictor
		return (1);
	else
		return (0);
}


//! Make ICH/P-mode decision (normal, not RDO)
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure
	\param adj_predicted_size  Adjusted predicted size
	\param alt_pfx             Alt */
int IchDecision(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int adj_predicted_size,	int alt_pfx, int alt_size_to_generate)
{
	int p_mode_cost, ich_mode_cost;
	int bits_ich_mode, bits_p_mode;
	int log_err_ich_mode, log_err_p_mode;
	int max_err_p_mode[MAX_UNITS_PER_GROUP];
	int i;

	for (i=0; i<dsc_state->unitsPerGroup; ++i)
		max_err_p_mode[i] = 0;
	for (i=0; i<dsc_state->unitsPerGroup; ++i)
	{
		if(UsingMidpoint(dsc_cfg, dsc_state, i, dsc_state->unitCType[i]))
			max_err_p_mode[i] = MAX(max_err_p_mode[i], dsc_state->maxMidError[i]);
		else
			max_err_p_mode[i] = MAX(max_err_p_mode[i], dsc_state->maxError[i]);
	}

	// *MODEL NOTE* MN_ENC_ICH_MODE_SELECT
	if (dsc_state->prevIchSelected)
		bits_ich_mode = 1;
	else  // For escape code, no need to send trailing one for prefix
		bits_ich_mode = alt_size_to_generate - adj_predicted_size;
	bits_ich_mode += ICH_BITS * dsc_state->ichIndicesInGroup;  // ICH indices

	log_err_p_mode = 0;
	log_err_ich_mode = 0;
	for (i=0; i<dsc_state->unitsPerGroup; ++i)
	{
		log_err_p_mode += ceil_log2(max_err_p_mode[i]);
		log_err_ich_mode += ceil_log2(dsc_state->maxIchError[i]);
		if((dsc_cfg->dsc_version_minor == 1) && i==0)
		{
			log_err_p_mode *= 2;
			log_err_ich_mode *= 2;
		}
	}

	bits_p_mode = EstimateBitsForGroup(dsc_cfg, dsc_state);
	p_mode_cost = bits_p_mode + ICH_LAMBDA * log_err_p_mode;
	ich_mode_cost = bits_ich_mode + ICH_LAMBDA * log_err_ich_mode;

	if (dsc_cfg->dsc_version_minor == 2) {   // Original DSC 1.2 mode
		if(IsOrigFlatHIndex(dsc_cfg, dsc_state, dsc_state->hPos) == 2)
			return (log_err_ich_mode <= log_err_p_mode) && (ich_mode_cost < p_mode_cost);
		else
			return (ich_mode_cost < p_mode_cost);
	} else			// DSC 1.1 mode
		return (log_err_ich_mode <= log_err_p_mode) && (ich_mode_cost < p_mode_cost);
}


//! Look up the pixel values for a given ICH index
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure
	\param entry     History index (0-31)
	\param p         Returned pixel value
	\param hPos      Horizontal position in slice (to determine upper pixels, if applicable)
	\param first_line_flag Set to 1 for first line (ie., disable upper pixels) */
void HistoryLookup(dsc_cfg_t *dsc_cfg, dsc_state_t* dsc_state, int entry, unsigned int *p, int hPos, int first_line_flag, int is_odd_line)
{
	int reserved, i;

	memset(p, 0, sizeof(unsigned int) * NUM_COMPONENTS); // Fix compiler warning

	reserved = ICH_SIZE-ICH_PIXELS_ABOVE;

	hPos = (hPos/dsc_state->pixelsInGroup)*dsc_state->pixelsInGroup + (dsc_state->pixelsInGroup/2);  // Center pixel of group as reference
	if (dsc_cfg->native_420 || dsc_cfg->native_422)
		hPos = CLAMP(hPos, 2, dsc_state->sliceWidth-1-2);  // Keeps upper line history entries unique at left & right edge
	else
		hPos = CLAMP(hPos, ICH_PIXELS_ABOVE/2, dsc_state->sliceWidth-1-(ICH_PIXELS_ABOVE/2));  // Keeps upper line history entries unique at left & right edge
	if (!first_line_flag && (entry >= reserved))
	{
		if (dsc_cfg->native_420)
		{
			int offset;
			offset = (entry - reserved+1)>>1;
			if((entry-reserved) & 1)
			{
				p[0] = dsc_state->prevLine[0][hPos+offset - 2 +PADDING_LEFT];
				p[1] = dsc_state->prevLine[1][hPos+offset - 2  +PADDING_LEFT];
				p[2] = dsc_state->prevLine[2+is_odd_line][(hPos+offset-2)  +PADDING_LEFT];
			} else {
				p[0] = dsc_state->prevLine[1][hPos+offset - 2 +PADDING_LEFT];
				p[1] = dsc_state->prevLine[0][hPos+offset - 1 +PADDING_LEFT];
				p[2] = dsc_state->prevLine[2+is_odd_line][(hPos+offset) - 1 +PADDING_LEFT];
			}
		}
		else if (dsc_cfg->native_422)
		{
			int offset;
			offset = (entry - reserved+1)>>1;
			if((entry-reserved) & 1)
			{
				p[0] = dsc_state->prevLine[0][hPos+offset - 2 +PADDING_LEFT];
				p[1] = dsc_state->prevLine[1][hPos+offset-2  +PADDING_LEFT];
				p[2] = dsc_state->prevLine[2][hPos+offset-2  +PADDING_LEFT];
				p[3] = dsc_state->prevLine[3][hPos+offset - 2  +PADDING_LEFT];
			} else {
				p[0] = dsc_state->prevLine[3][hPos+offset - 2 +PADDING_LEFT];
				p[1] = dsc_state->prevLine[1][hPos+offset - 1 +PADDING_LEFT];
				p[2] = dsc_state->prevLine[2][hPos+offset - 1 +PADDING_LEFT];
				p[3] = dsc_state->prevLine[0][hPos+offset - 1 +PADDING_LEFT];
			}
		} 
		else
		{
			p[0] = dsc_state->prevLine[0][hPos+(entry-reserved) - (ICH_PIXELS_ABOVE/2)  +PADDING_LEFT];
			p[1] = dsc_state->prevLine[1][hPos+(entry-reserved) - (ICH_PIXELS_ABOVE/2)  +PADDING_LEFT];
			p[2] = dsc_state->prevLine[2][hPos+(entry-reserved) - (ICH_PIXELS_ABOVE/2)  +PADDING_LEFT];
		}
	} else {
		for(i=0; i<dsc_state->numComponents; ++i)
			p[i] = dsc_state->history.pixels[i][entry];
	}
}


//! Encoder function to determine whether or not the current sample is within the quantization error of any ICH entry 
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param hPos      Current horizontal position within slice
	\param vPos      Current vertical position within slice
	\param qp        Quantization parameter for current group
	\param sampModCnt Index of current pixel within group 
	\return          1 indicates that the sample is within the quantization error */
int IsOrigWithinQerr(dsc_cfg_t* dsc_cfg, dsc_state_t *dsc_state, int hPos, int vPos, int qp, int sampModCnt)
{
	int max_qerr[NUM_COMPONENTS];
	int hit;
	int diff, absdiff;
	unsigned int orig;
	int cpnt;
	unsigned int ich_pixel[NUM_COMPONENTS];
	int modified_qp;
	int i, j;

	// *MODEL NOTE* MN_ENC_ICH_PIXEL_CHECK_QERR
	dsc_state->origWithinQerr[sampModCnt] = 0;  // Assume for now that pixel is not within QErr

	if (ICH_BITS==0)		// ICH disabled
		return(0);
	if ((hPos == 0) && (vPos == 0))
		return(0);		// 1st group of slice can't use ICH since it's empty

	modified_qp = MIN(2*dsc_cfg->bits_per_component-1, qp+2);

	for(i=0; i<dsc_state->numComponents; ++i)
		max_qerr[i] = QuantDivisor[MapQpToQlevel(dsc_cfg, dsc_state, modified_qp, i)]/2;

	if ((!dsc_cfg->native_420 && dsc_state->vPos>0) || (dsc_cfg->native_420 && dsc_state->vPos>1))
	{
		// UL/U/UR always valid for non-first-lines
		for (i=ICH_SIZE-ICH_PIXELS_ABOVE; i<ICH_SIZE; ++i)
			dsc_state->history.valid[i] = 1;
	}

	hit = 0;
	for (j=0; j<ICH_SIZE; ++j)
	{
		if (!dsc_state->history.valid[j])
			continue;
		hit = 1;
		HistoryLookup(dsc_cfg, dsc_state, j, ich_pixel, hPos, 
                     (vPos==0) || (dsc_cfg->native_420 && vPos==1), vPos%2);  // CLAMP is to minimize duplicates among UL/U/UR

		for (cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
		{
			orig = dsc_state->origLine[cpnt][(hPos)+PADDING_LEFT];
			diff = (int)ich_pixel[cpnt] - (int)orig;
			absdiff = abs(diff);
			if (absdiff > max_qerr[cpnt])
				hit = 0;
		}
		if (hit)
			break;  // Found one
	}
	if (!hit)
		return(0);  // Can't use, one pixel was a total miss

	dsc_state->origWithinQerr[sampModCnt] = 1;
	return (1);
}


//! Update the ICH state based on a reconstructed pixel
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param recon     A 3-element array containing the components of the reconstructed pixel */

/////////////////////////////////////////////////////////
//Indexed Color History Updates (6.5.2)
/////////////////////////////////////////////////////////
void UpdateHistoryElement(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, unsigned int *recon)
{
	int hit, j;
	int loc;
	int cpnt;
	unsigned int ich_pixel[NUM_COMPONENTS];
	int reserved;
	int first_line_flag;

	first_line_flag = (dsc_state->vPos==0) || (dsc_cfg->native_420 && dsc_state->vPos==1);

	// *MODEL NOTE* MN_ICH_UPDATE
	reserved = first_line_flag ? ICH_SIZE : (ICH_SIZE-ICH_PIXELS_ABOVE);  // space for UL, U, UR

	// Update the ICH with recon as the MRU
	hit = 0;
	loc = reserved-1;  // If no match, delete LRU
	for (j=0; j<reserved; ++j)
	{
		if (!dsc_state->history.valid[j])  // Can replace any empty entry
		{
			loc = j;
			break;
		}
		HistoryLookup(dsc_cfg, dsc_state, j, ich_pixel, dsc_state->hPos, first_line_flag, dsc_state->vPos%2);  // Specific hPos within group is not critical 
																					// since hits against UL/U/UR don't have specific detection
		hit = 1;
		for (cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
		{
			if (ich_pixel[cpnt] != recon[cpnt])
				hit = 0;
		}
		if (hit && ((dsc_state->isEncoder && dsc_state->ichSelected) || (!dsc_state->isEncoder && dsc_state->prevIchSelected)))
		{
			loc = j;
			break;  // Found one
		}
	}

	for (cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
	{
		// Delete from current position (or delete LRU)
		for (j=loc; j>0; --j)
		{
			dsc_state->history.pixels[cpnt][j] = dsc_state->history.pixels[cpnt][j-1];	
		}
		dsc_state->history.valid[loc] = 1;

		// Insert as most recent
		dsc_state->history.pixels[cpnt][0] = recon[cpnt];
		dsc_state->history.valid[0] = 1;
	}
}



//! Encoder function to select the best history entry & return the corresponding index
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param hPos      Horizontal position within slice
	\param orig      3-element array containing the component samples for the pixel to be matched
	\return          The ICH index that the encoder selects */
////////////////////////////////////////////////////////////////////////////
// Indexed Color History Candidate Index Selection (6.5.3.1, 6.5.3.2)
////////////////////////////////////////////////////////////////////////////
int PickBestHistoryValue(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int hPos, unsigned int *orig)
{
	int j, best, lowest_sad;
	int diff0, diff1, diff2, diff3, weighted_sad;
	unsigned int ich_pixel[NUM_COMPONENTS];
	int first_line_flag;

	// Initialize to large, illegal values
	lowest_sad = LARGE_INT;
	best = 99;

	first_line_flag = dsc_state->vPos==0 || (dsc_cfg->native_420 && dsc_state->vPos==1);

	// *MODEL NOTE* MN_ENC_ICH_IDX_SELECT
	for (j=0; j<ICH_SIZE; ++j)
	{
		if (!dsc_state->history.valid[j])
			continue;
		// Calculate 'weightedSad'
		// Let Find the Minimum 'weightedSad' Value
		weighted_sad = 0;
		HistoryLookup(dsc_cfg, dsc_state, j, ich_pixel, hPos, first_line_flag, dsc_state->vPos%2);  // -2 to preserve U/UL/UR at right edge
		diff0 = (int)ich_pixel[0] - (int)orig[0];
		diff1 = (int)ich_pixel[1] - (int)orig[1];
		diff2 = (int)ich_pixel[2] - (int)orig[2];
		diff3 = (int)ich_pixel[3] - (int)orig[3];

		if(dsc_cfg->native_422)
			weighted_sad = 2 * abs(diff0) + abs(diff1) + abs(diff2) + 2 * abs(diff3);
		else if ((!dsc_cfg->native_420) || (dsc_cfg->dsc_version_minor==1))   // YCoCg chroma has an extra bit
			weighted_sad = 2 * abs(diff0) + abs(diff1) + abs(diff2);
		else	
			weighted_sad = abs(diff0) + abs(diff1) + abs(diff2);
		if (lowest_sad > weighted_sad)  // Find lowest SAD
		{
			lowest_sad = weighted_sad;
			best = j;
		}
	}

	if(best==99)
		printf("ICH search failed\n");
	return (best);
}


//! Updates the ICH state using final reconstructed value
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param currLine  Reconstructed samples for current line
	\param sampModCnt Index of which pixel within group
	\param hPos      Horizontal position within slice (note that update uses pixels from one group prior)
	\param vPos      Vertical position within slice */
void UpdateICHistory(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int **currLine, int hPos, int vPos)
{
	int i;
	int cpnt;
	unsigned int p[NUM_COMPONENTS];
	int hPos_prev_group;

	if (ICH_BITS==0)
		return;			// Nothing to do if ICH disabled

	// Reset ICH at beginning of each line if multiple slices per line
	if (hPos==0)
	{
		if (vPos == 0)            // Beginning of slice
		{
			for (i=0; i<ICH_SIZE; ++i)
				dsc_state->history.valid[i] = 0;
		}
		// End of Slice!
		else if (dsc_cfg->slice_width != dsc_cfg->pic_width)   // Multiple slices per line
															   // Picture width != Slice width
		{
			// padding with zero
			for (i=0; i<ICH_SIZE; ++i)
				dsc_state->history.valid[i] = 0;
		}
	}

	hPos_prev_group = hPos - dsc_state->pixelsInGroup;

	if (hPos_prev_group < 0)
		return;			// Current code doesn't update ICH for last group of each line -- probably not much correlation anyway

	// Get final reconstructed pixel value
	for (cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
	{
		p[cpnt] = currLine[cpnt][hPos_prev_group + PADDING_LEFT];
	}

	// Update ICH accordingly
	UpdateHistoryElement(dsc_cfg, dsc_state, p);
}


//! Encoder function to update the reconstructed samples & output picture if ICH is selected
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param currLine  Current line reconstructed samples (modified) */
void UseICHistory(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int **currLine)
{
	int i;
	int cpnt;
	unsigned int p[NUM_COMPONENTS];
	int hPos;

	if (ICH_BITS==0)
		return;   // If ICH is disabled, do nothing

	// Start at beginning of group
	hPos = dsc_state->hPos - dsc_state->pixelsInGroup + 1;
	
	if(g_verbose)
		printf("Using ICH values: \n");
	// Apply ICH decision to reconstructed line & update ICH values.
	for (i=0; i<dsc_state->ichIndicesInGroup; ++i)
	{
		if (dsc_state->ichSelected)
		{
			// We kept track of the history values the were selected when we checked if ICH was a good match for orig pixels
			p[0] = dsc_state->ichPixels[i][0];
			p[1] = dsc_state->ichPixels[i][1];
			p[2] = dsc_state->ichPixels[i][2];
			p[3] = dsc_state->ichPixels[i][3];

			if(g_verbose)
				printf("%d(%d %d %d %d), ", dsc_state->ichLookup[i], p[0], p[1], p[2], p[3]);
			// Update reconstructed line and output picture
			for (cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
				currLine[cpnt][(hPos+i)+PADDING_LEFT] = p[cpnt];
		}
	}
	if(g_verbose)
		printf("\n");
}


//! Encoder function to updated reconstructed pixels if midpoint prediction was selected
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param currLine  Current line reconstructed samples (modified)
	\param flag_first_luma For 4:2:2, flag indicating that only the first luma unit has been processed so far */
void UpdateMidpoint(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int **currLine, int hPos)
{
	int i;
	int cpnt, unit;
	int sampModCnt;
	int start_hPos;

	sampModCnt = (hPos % dsc_state->pixelsInGroup);
	
	start_hPos = hPos - sampModCnt;

	// Apply MPP decision to reconstructed line & update ICH values.  Let's do this in raster order.
	for (i=0; i<SAMPLES_PER_UNIT; ++i)
	{
		for(unit=0; unit<dsc_state->unitsPerGroup; ++unit)
		{
			cpnt = dsc_state->unitCType[unit];

			if (dsc_state->midpointSelected[unit])
				currLine[cpnt][start_hPos+PADDING_LEFT+dsc_state->unitStartHPos[unit]+i] = dsc_state->midpointRecon[unit][i];
		}
		if (hPos+i>=dsc_state->sliceWidth)
			return;
	}
}


//! Returns midpoint prediction predictor
/*! \param dsc_state DSC state structure 
	\param cpnt      Component index
	\param qlevel    Current quantization level */
int FindMidpoint(dsc_state_t *dsc_state, int cpnt, int qlevel)
{
	int range;

	// *MODEL NOTE* MN_MIDPOINT_PRED
	///////////////////////////////////////
	// Midpoint Prediction (6.4.3)
	///////////////////////////////////////
	range = 1<<dsc_state->cpntBitDepth[cpnt];

	return (range/2 + (dsc_state->leftRecon[cpnt]%(1<<qlevel)));
}


//! Function to decide block vs. MAP & which block prediction vector to use for the next line
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure
	\param cpnt      Component index
	\param currLine  Current line's reconstructed samples
	\param hPos      Horizontal position in slice
	\param recon_x   Current reconstructed sample value */
void BlockPredSearch(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int cpnt, int **currLine, int hPos)
{
	int i, j;
	int candidate_vector;  // a value of 0 maps to -1, 2 maps to -3, etc.
	// MAPED to... 0  1  2  3  4  5  6  7  8   9
	// THIS VALUE -1 -2 -3 -4 -5 -6 -7 -8 -9 -10
	int pred_x;
	int pixel_mod_cnt;
	int min_err;
	PRED_TYPE min_pred;
	int cursamp;
	int pixdiff;
	int bp_sads[BP_RANGE];
	int modified_abs_diff;
	int max_cpnt;
	int recon_x;
	int min_bp_vector, max_bp_vector;

	// This function runs after a reconstructed value is determined and computes the best predictor to use for the NEXT line.
	// An implementation could run the block prediction search at any time after that up until the point at which the selection is needed.
	// *MODEL NOTE* MN_BP_SEARCH
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Prediction Method Decision (6.4.4, 6.4.4.1)
	// Make Decision between MMAP and BP method Using Residual Value of Previous Line
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(cpnt >= NUM_COMPONENTS)
		return;

	max_cpnt = dsc_state->numComponents - 1;
	if (dsc_cfg->native_420)
	{
		if (cpnt > 1)
			return;
		max_cpnt = 1;   // BP only for luma
	}

	if (hPos == 0)
	{
		// Reset prediction accumulators every line
		dsc_state->bpCount = 0;
		dsc_state->lastEdgeCount = 10;  // Arbitrary large value as initial condition
		for (i=0; i<dsc_state->numComponents; ++i)
		{
			for (j=0; j<BP_SIZE; ++j)
			{
				for (candidate_vector=0; candidate_vector<BP_RANGE; ++candidate_vector)
					dsc_state->lastErr[i][j][candidate_vector] = 0;
					//lastErr[NUM_COMPONENTS][BP_SIZE][BP_RANGE] : < 3-pixel SAD's for each of the past 3 3-pixel-wide prediction blocks for each BP offset
			}
		}
	}

	// Last edge count check - looks at absolute differences between adjacent pixels
	//   - Don't use block prediction if the content is basically flat
	//pixdiff = SampToLineBuf(dsc_cfg, dsc_state, recon_x, cpnt) - 
	//	      SampToLineBuf(dsc_cfg, dsc_state, currLine[cpnt][hPos+PADDING_LEFT-1], cpnt);
	recon_x = currLine[cpnt][hPos+PADDING_LEFT];
	if(hPos > 0)
		pixdiff = recon_x - currLine[cpnt][hPos+PADDING_LEFT-1]; //CurrentSample - LeftSample
	else
		pixdiff = recon_x - (1<<(dsc_state->cpntBitDepth[cpnt]-1)); // midpoint pixel value
	pixdiff = ABS(pixdiff);
	if (cpnt == 0)
		dsc_state->edgeDetected = 0;
	if (pixdiff > (BP_EDGE_STRENGTH << (dsc_cfg->bits_per_component-8))) //Edge Occur Condition
		dsc_state->edgeDetected = 1;
	if (cpnt == max_cpnt)
	{
		if (dsc_state->edgeDetected)
			dsc_state->lastEdgeCount = 0;
		else
			dsc_state->lastEdgeCount++;
	}

	// The BP
	cursamp = (hPos/PRED_BLK_SIZE) % BP_SIZE;

	pixel_mod_cnt = hPos % PRED_BLK_SIZE; // [0, 1, 2]
	for ( candidate_vector = 0; candidate_vector < BP_RANGE; candidate_vector++ ) {
		// MAPED to... 0  1  2  3  4  5  6  7  8   9  10  11  12
		// THIS VALUE -1 -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 -13
		if ( pixel_mod_cnt == 0 ) {			// predErr is summed over PRED_BLK_SIZE pixels
			dsc_state->predErr[cpnt][candidate_vector] = 0;
		}

		// need to update
		if(hPos>candidate_vector)
			pred_x = SamplePredict( dsc_state, currLine[cpnt], currLine[cpnt], hPos, (PRED_TYPE)(candidate_vector+PT_BLOCK), 0, cpnt );
		else
			pred_x = 1<<(dsc_state->cpntBitDepth[cpnt]-1);

		// HW uses previous line's reconstructed samples, which may be bit-reduced
		//pred_x = SampToLineBuf(dsc_cfg, dsc_state, pred_x, cpnt);
		//recon_x = SampToLineBuf(dsc_cfg, dsc_state, recon_x, cpnt);

		pixdiff = recon_x - pred_x;
		pixdiff = ABS(pixdiff);
		modified_abs_diff = MIN(pixdiff>>(dsc_state->cpntBitDepth[cpnt] - 7), 0x3f);
		// ABS differences are clamped to 6 bits each, predErr for 3 pixels is 8 bits
		dsc_state->predErr[cpnt][candidate_vector] += modified_abs_diff;
	}

	if ( pixel_mod_cnt == PRED_BLK_SIZE - 1 ) // Last Component
	{
		// Track last 3 3-pixel SADs for each component (each is 7 bit)
		for (candidate_vector=0; candidate_vector<BP_RANGE; ++candidate_vector)
			dsc_state->lastErr[cpnt][cursamp][candidate_vector] = dsc_state->predErr[cpnt][candidate_vector];

		if (cpnt<max_cpnt)
			return;   // SAD is across all 3 components -- wait until we've processed all 3

		for (candidate_vector=0; candidate_vector<BP_RANGE; ++candidate_vector)
		{
			bp_sads[candidate_vector] = 0;

			for (i=0; i<BP_SIZE; ++i)
			{
				int sad3x1 = 0;

				// Use all 3 components
				for(j=0; j<dsc_state->numComponents; ++j)
					 sad3x1 += dsc_state->lastErr[j][i][candidate_vector];
				// sad3x1 is 9 bits
				sad3x1 = MIN(511, sad3x1); //3 Pixel SAD

				bp_sads[candidate_vector] += sad3x1;  // 11-bit SAD
			}
			// Each bp_sad can have a max value of 63*9 pixels * 3 components = 1701 or 11 bits
			bp_sads[candidate_vector] >>= 3;  // SAD is truncated to 8-bit for comparison
		}

		min_err = bp_sads[0];
		min_pred = PT_MAP; // 0
		min_bp_vector = 3;
		max_bp_vector = 10;
		// Could have moved search range from 6-13 for high throughput mode, but not much performance difference, so going with simpler approach
		for (candidate_vector=min_bp_vector-1; candidate_vector<=max_bp_vector-1; ++candidate_vector)
		{
			// Ties favor smallest vector
			if (min_err > bp_sads[candidate_vector])
			{
				min_err = bp_sads[candidate_vector];
				min_pred = (PRED_TYPE)(candidate_vector + PT_BLOCK);
			} 
		}

		if (dsc_cfg->block_pred_enable && (hPos>=9))  // Don't start algorithm until 10th pixel
		{
			if (min_pred > PT_BLOCK)
				dsc_state->bpCount++;
			else
				dsc_state->bpCount = 0;
		}
		if ((dsc_state->bpCount>=3) && (dsc_state->lastEdgeCount < BP_EDGE_COUNT)) // BP will be choosen in this condition
			dsc_state->prevLinePred[hPos/PRED_BLK_SIZE] = (PRED_TYPE)min_pred;
		else
			dsc_state->prevLinePred[hPos/PRED_BLK_SIZE] = (PRED_TYPE)PT_MAP;
	}
}


//! Function to compute number of bits required to code a given residual
/*! \param eq        Residual  */
int FindResidualSize(int eq)
{
	int size_e;

	// Find the size in bits of e
	if(eq == 0) size_e = 0;
	else if (eq >= -1 && eq <= 0) size_e = 1;
	else if (eq >= -2 && eq <= 1) size_e = 2;
	else if (eq >= -4 && eq <= 3) size_e = 3;
	else if (eq >= -8 && eq <= 7) size_e = 4;
	else if (eq >= -16 && eq <= 15) size_e = 5;	
	else if (eq >= -32 && eq <= 31) size_e = 6;
	else if (eq >= -64 && eq <= 63) size_e = 7;
	else if (eq >= -128 && eq <= 127) size_e = 8;
	else if (eq >= -256 && eq <= 255) size_e = 9;
	else if (eq >= -512 && eq <= 511) size_e = 10;
	else if (eq >= -1024 && eq <= 1023) size_e = 11;
	else if (eq >= -2048 && eq <= 2047) size_e = 12;
	else if (eq >= -4096 && eq <= 4095) size_e = 13;
	else if (eq >= -8192 && eq <= 8191) size_e = 14;
	else if (eq >= -16384 && eq <= 16383) size_e = 15;
	else if (eq >= -32768 && eq <= 32767) size_e = 16;
	else if (eq >= -65536 && eq <= 65535) size_e = 17;
	else if (eq >= -131702 && eq <= 131701) size_e = 18;
	else
	{
		printf("unexpectedly large residual size\n");
		return 0;
	}

	return size_e;
}


//! Map QP to quantization level
/*! \param dsc_cfg   DSC configuration structure
	\param qp      QP to map
    \return        Flag if flatness information sent for the current supergroup */
int IsFlatnessInfoSent(dsc_cfg_t *dsc_cfg, int qp)
{
	return ((qp>=dsc_cfg->flatness_min_qp) && (qp<=dsc_cfg->flatness_max_qp));
}


//! Check if original pixels are flat at a specified location
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param hPos    Set to the horizontal location of the group we're testing - 1 */
int IsOrigFlatHIndex(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int hPos)
{
	int i, cpnt;
	int max, min;
	int p;
	int thresh[NUM_COMPONENTS], qp;
	int somewhat_flat = 1, very_flat = 1;
	int vf_thresh;
	int fc1_start, fc1_end, fc2_start, fc2_end;

	fc1_start = 0; fc1_end = 4; fc2_start = 1; fc2_end = 7;

	qp = MAX(dsc_state->masterQp - SOMEWHAT_FLAT_QP_DELTA, 0);
	for(i=0; i<dsc_state->numComponents; ++i)
		thresh[i] = MapQpToQlevel(dsc_cfg, dsc_state, qp, i);

	// If group starts past the end of the slice, it can't be flat
	if (hPos+1 >= dsc_state->sliceWidth)
	{
		return(0);
	}

	for (cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
	{
		vf_thresh = dsc_cfg->flatness_det_thresh;
		max = -1; min = 99999;
		for (i=fc1_start; i < fc1_end; ++i)
		{
			p = dsc_state->origLine[cpnt][PADDING_LEFT + (hPos+i)];
			if (max < p) max = p;
			if (min > p) min = p;
		}
		if (max - min > MAX(vf_thresh, QuantDivisor[thresh[cpnt]]))
			somewhat_flat = 0;
		if (max - min > vf_thresh)
			very_flat = 0;
	}
	if (very_flat)
		return (2);
	else if (somewhat_flat)
		return (1);

	// Skip flatness check 2 if it only contains a single pixel
	if (hPos+2 >= dsc_state->sliceWidth)
		return (0);

	somewhat_flat = very_flat = 1;
	// Left adjacent isn't flat, but current group & group to the right is flat
	for (cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
	{
		vf_thresh = dsc_cfg->flatness_det_thresh;

		max = -1; min = 99999;
		for (i=fc2_start; i< fc2_end; ++i)
		{
			p = dsc_state->origLine[cpnt][PADDING_LEFT + (hPos+i)];
			if (max < p) max = p;
			if (min > p) min = p;
		}
		if (max - min > MAX(vf_thresh,QuantDivisor[thresh[cpnt]]))
			somewhat_flat = 0;
		if (max - min > vf_thresh)
			very_flat = 0;
	}
	if (very_flat)
		return (2);
	else if (somewhat_flat)
		return (1);
	return(0);
}


//! Function to remove one pixel's worth of bits from the encoder buffer model
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure */
void RemoveBitsEncoderBuffer(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state)
{
	dsc_state->bpgFracAccum += (dsc_cfg->bits_per_pixel & 0xf);
	dsc_state->bufferFullness -= (dsc_cfg->bits_per_pixel >> 4) + (dsc_state->bpgFracAccum>>4);
	dsc_state->numBitsChunk += (dsc_cfg->bits_per_pixel >> 4) + (dsc_state->bpgFracAccum>>4);
	dsc_state->bpgFracAccum &= 0xf;
	dsc_state->chunkPixelTimes++;
	if (dsc_state->chunkPixelTimes >= dsc_state->sliceWidth)
	{
		int adjustment_bits;
		if (dsc_cfg->vbr_enable)
		{
			int size;
			size = (dsc_state->numBitsChunk - dsc_state->bitsClamped + 7) / 8;
			adjustment_bits = size * 8 - (dsc_state->numBitsChunk - dsc_state->bitsClamped);
			dsc_state->bufferFullness -= adjustment_bits;
			dsc_state->bitsClamped = 0;
			if (dsc_state->isEncoder)
				dsc_state->chunkSizes[dsc_state->chunkCount] = size;
		}
		else   // CBR mode
		{
			adjustment_bits = dsc_cfg->chunk_size * 8 - dsc_state->numBitsChunk;
			dsc_state->bufferFullness -= adjustment_bits;
		}
		dsc_state->bpgFracAccum = 0;
		dsc_state->numBitsChunk = 0;
		dsc_state->chunkCount++;
		dsc_state->chunkPixelTimes = 0;
	}
}


//! Rate control function
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param throttle_offset  Offset to apply to buffer fullness before doing threshold comparisons
	\param bpg_offset  Bits per group adjustment for current group
	\param group_count  Current group within slice
	\param scale     Scale factor to apply to buffer fullness before doing threshold comparisons
	\param group_size Number of pixels actually in group (could be smaller than nominal group size for partial groups) */

//////////////////////////////////////////////////////////////////
// Rate Control Algorithm (6.8)
//////////////////////////////////////////////////////////////////
void RateControl( dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int throttle_offset, int bpg_offset, int group_count, int scale, int group_size )
{
	int i;
	int prev_fullness;
	int rcSizeGroup;
	int rcTgtBitsGroup;
	int overflowAvoid;
	int min_QP;
	int max_QP;
	int tgtMinusOffset;
	int tgtPlusOffset;
	dsc_range_cfg_t *range_cfg;
	int rcSizeGroupPrev  = dsc_state->rcSizeGroup;
	int prevQp            = dsc_state->stQp;
	int prev2Qp           = dsc_state->prevQp;
	int rcModelBufferFullness;
	int bpg;
	int incr_amount;
	int selected_range;
	int stQp;
	int curQp;
	int mpsel;
	int predActivity, bitSaveThresh;

	prev_fullness = dsc_state->bufferFullness;
	if(dsc_state->isEncoder)
		mpsel = dsc_state->midpointSelected[0] + dsc_state->midpointSelected[1] + dsc_state->midpointSelected[2] + dsc_state->midpointSelected[3];
	else
		mpsel = dsc_state->useMidpoint[0] + dsc_state->useMidpoint[1] + dsc_state->useMidpoint[2] + dsc_state->useMidpoint[3];
	
	for (i=0; i<group_size; ++i)
	{
		dsc_state->pixelCount++;
		if (dsc_state->pixelCount >= dsc_cfg->initial_xmit_delay)
			RemoveBitsEncoderBuffer(dsc_cfg, dsc_state);
	}

	// Add up estimated bits for the Group, i.e. as if VLC sample size matched max sample size
	rcSizeGroup = 0;
	for (i = 0; i < dsc_state->unitsPerGroup; i++)
		rcSizeGroup += dsc_state->rcSizeUnit[i];

	if (PRINT_DEBUG_RC)
	{
		fprintf(g_fp_dbg, "RC group #%d, scale=%d, offset=%d, bpg_offset=%d, ", group_count, scale, throttle_offset, bpg_offset);
		fprintf(g_fp_dbg, "RCSizeGroup=%d, coded_group_size=%d\n", rcSizeGroup, dsc_state->codedGroupSize);
		fprintf(g_fp_dbg, "Buffer fullness=%d, ", dsc_state->bufferFullness);
	}

	// Set target number of bits per Group according to buffer fullness
	range_cfg = dsc_cfg->rc_range_parameters;
	throttle_offset -= dsc_cfg->rc_model_size;

	// *MODEL NOTE* MN_RC_XFORM
	///////////////////////////////////////////////////////////////////
	// Linear Transformation (6.8.2)
	///////////////////////////////////////////////////////////////////
	rcModelBufferFullness = (scale * (dsc_state->bufferFullness + throttle_offset)) >> RC_SCALE_BINARY_POINT;
	if (PRINT_DEBUG_RC)
		fprintf(g_fp_dbg, "RC model buffer fullness=%d\n", rcModelBufferFullness);

	// Pick the correct range
	// *MODEL NOTE* MN_RC_LONG_TERM
	///////////////////////////////////////////////////////////////////
	// Long-term Parameter Selection (6.8.3)
	// Let Find Best Quantization Param (QP) at a Given Buffer Status
	///////////////////////////////////////////////////////////////////
	for (i=NUM_BUF_RANGES-1; i>0; --i)
	{
		overflowAvoid = (dsc_state->bufferFullness + throttle_offset > OVERFLOW_AVOID_THRESHOLD);
		if ((rcModelBufferFullness > dsc_cfg->rc_buf_thresh[i-1]-dsc_cfg->rc_model_size) )
			break;
	}

	if (rcModelBufferFullness > 0)
	{
		printf("The RC model has overflowed.  To address this issue, please adjust the\n");
		printf("min_QP and max_QP higher for the top-most ranges, or decrease the rc_buf_thresh\n");
		printf("for those ranges.\n");
		if(dsc_state->isEncoder)
			exit(1);
		else
			dsc_state->errorOccurred = 1;
	}

	// Add a group time of delay to RC calculation
	selected_range = dsc_state->prevRange;
	dsc_state->prevRange = i;

	// *MODEL NOTE* MN_RC_SHORT_TERM
	///////////////////////////////////////////////////////////////////
	// Short-term Quantization Parameter Adjustment (6.8.4)
	// Modify QP
	///////////////////////////////////////////////////////////////////
	bpg = (dsc_cfg->bits_per_pixel * group_size);
	bpg = (bpg + 8) >> 4;
	rcTgtBitsGroup = MAX( 0, bpg + range_cfg[selected_range].range_bpg_offset + bpg_offset );
	min_QP = range_cfg[selected_range].range_min_qp;
	max_QP = range_cfg[selected_range].range_max_qp;

	if (PRINT_DEBUG_RC)
		fprintf(g_fp_dbg, "Range %d: MIN=%d, MAX=%d, QuantPrev=%d, ", selected_range, min_QP, max_QP, dsc_state->stQp);

	tgtMinusOffset = MAX( 0, rcTgtBitsGroup - dsc_cfg->rc_tgt_offset_lo );
	tgtPlusOffset  = MAX( 0, rcTgtBitsGroup + dsc_cfg->rc_tgt_offset_hi );
	incr_amount = (dsc_state->codedGroupSize - rcTgtBitsGroup) >> 1;

	if (dsc_cfg->native_420)			// 420 mode
		predActivity = dsc_state->prevQp + MAX(dsc_state->predictedSize[0], dsc_state->predictedSize[1]) + dsc_state->predictedSize[2];
	else if (!dsc_cfg->native_422)		// 444 mode
	    predActivity = dsc_state->prevQp + dsc_state->predictedSize[0] + MAX(dsc_state->predictedSize[1], dsc_state->predictedSize[2]);
	else								// 422 mode
		predActivity = dsc_state->prevQp + ((dsc_state->predictedSize[0] + dsc_state->predictedSize[3] + dsc_state->predictedSize[1] + dsc_state->predictedSize[2])>>1);
	bitSaveThresh = dsc_state->cpntBitDepth[0] + dsc_state->cpntBitDepth[1] - 2;
	if((dsc_cfg->dsc_version_minor == 2) && dsc_state->vPos > 0 && dsc_state->firstFlat == -1)
	{
		if(!dsc_state->ichSelected && (mpsel >= 3))
		{
			dsc_state->mppState++;
			if(dsc_state->mppState >= 2)
				dsc_state->bitSaveMode = 2;
		}
		else if (!dsc_state->ichSelected && predActivity >= bitSaveThresh)
			dsc_state->bitSaveMode = dsc_state->bitSaveMode;	// Don't reset
		else if (dsc_state->ichSelected)
			dsc_state->bitSaveMode = MAX(1, dsc_state->bitSaveMode); 
		else
		{
			dsc_state->mppState = 0;
			dsc_state->bitSaveMode = 0;
		}
	}
	else
	{
		dsc_state->bitSaveMode = 0;
		dsc_state->mppState = 0;
	}

	if ((dsc_cfg->dsc_version_minor == 2) && dsc_state->bufferFullness < 192)
	{
		stQp = min_QP;
	}
	else if(dsc_state->bitSaveMode)
	{
		max_QP = MIN(dsc_cfg->bits_per_component*2 - 1, max_QP + 1);
		if (dsc_state->bitSaveMode == 1)
			stQp = prevQp;
		else
			stQp = prevQp+2;
	}	
	else if (rcSizeGroup==dsc_state->unitsPerGroup)
	{
		if(dsc_cfg->dsc_version_minor == 2)
		{
			min_QP = MAX(min_QP-4, 0);  // was min_QP / 2
			stQp = prevQp - 1;
		} else
			stQp = MAX(min_QP/2, prevQp-1);
	} 
	else if ( ((dsc_cfg->dsc_version_minor == 1) && (dsc_state->codedGroupSize < tgtMinusOffset) && (rcSizeGroup < tgtMinusOffset) ) ||
			  ((dsc_cfg->dsc_version_minor == 2) && (rcSizeGroup < tgtMinusOffset) ) ) 
	{
		if(dsc_cfg->dsc_version_minor == 2)
			stQp = prevQp - 1;
		else
			stQp = MAX(min_QP, (prevQp-1));
	}
	// avoid increasing QP immediately after edge
	else if (  (dsc_state->bufferFullness >= 64) &&  dsc_state->codedGroupSize > tgtPlusOffset )
	{ 
		curQp = MAX(prevQp, min_QP);
		if (prev2Qp == curQp)   // 2nd prev grp == prev grp 
		{   
			if ( (rcSizeGroup*2) < (rcSizeGroupPrev*dsc_cfg->rc_edge_factor) )
			{
				if (dsc_cfg->dsc_version_minor == 2)
					stQp = curQp + incr_amount;
				else
					stQp = MIN(max_QP, (curQp+incr_amount));
			} else
				stQp = curQp;
		}
		else if (prev2Qp < curQp) 
		{
			if ( ( (rcSizeGroup*2) < (rcSizeGroupPrev*dsc_cfg->rc_edge_factor) && (curQp < dsc_cfg->rc_quant_incr_limit0)) ) 
			{
				if (dsc_cfg->dsc_version_minor == 2)
					stQp = curQp + incr_amount;
				else
					stQp = MIN(max_QP, (curQp+incr_amount));
			} else
				stQp = curQp;
		}
		else if ( curQp < dsc_cfg->rc_quant_incr_limit1 ) 
		{
			if (dsc_cfg->dsc_version_minor == 2)
				stQp = curQp + incr_amount;
			else
				stQp = MIN(max_QP, (curQp+incr_amount));
		}
		else
		{
			stQp = curQp;
		}
	} else
		stQp = prevQp;

	if (dsc_cfg->dsc_version_minor == 2)
		stQp = CLAMP(stQp, min_QP, max_QP);  // MIN(stQp, max_QP) would probably be fine

	if ( overflowAvoid )
		stQp = range_cfg[NUM_BUF_RANGES-1].range_max_qp;

	rcSizeGroupPrev = rcSizeGroup;

	dsc_state->rcSizeGroup  = rcSizeGroup;
	dsc_state->stQp         = stQp;
	dsc_state->prevQp       = prevQp;

	if (PRINT_DEBUG_RC)
		fprintf(g_fp_dbg, "New quant value=%d\n", stQp);

	if ( dsc_state->bufferFullness > dsc_cfg->rcb_bits ) {
		printf("The buffer model has overflowed.  This probably occurred due to an error in the\n");
		printf("rate control parameter programming or an attempt to decode an invalid DSC stream.\n\n");
		printf( "ERROR: RCB overflow; size is %d, tried filling to %d\n", dsc_cfg->rcb_bits, dsc_state->bufferFullness );
		printf( "  previous level: %5d\n", prev_fullness );
		printf( "  target BPP:     %5d\n", dsc_cfg->bits_per_pixel >> 4 );
		printf( "  new level:      %5d\n", dsc_state->bufferFullness );
		printf( "Range info\n" );
		printf( "  range min_qp:     %d\n", range_cfg->range_min_qp );
		printf( "  range max_qp:     %d\n", range_cfg->range_max_qp );
		printf( "  range bpg_offset: %d\n", range_cfg->range_bpg_offset );
		if(dsc_state->isEncoder)
			exit(1);
		else
			dsc_state->errorOccurred = 1;
	}
}


//! Predict size for next unit
/*! \param dsc_cfg   DSC configuration structure
    \param req_size  Array of required sizes for the samples in the unit
	\return          Predicted size for next unit */
int PredictSize(dsc_cfg_t *dsc_cfg, int *req_size)
{
	int pred_size;

	pred_size = 2;  // +0.5 for rounding
	pred_size += req_size[0]+req_size[1];
	pred_size += 2*req_size[2];
	pred_size >>= 2; 

	return (pred_size);
}


//! Code one unit
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param cpnt      Component to code
	\param quantized_residuals Quantized residuals to code */
void VLCUnit(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int unit, int *quantized_residuals, int force_p1_ich2)
{
	int required_size[SAMPLES_PER_UNIT];
	int size;
	int max_size;
	int prefix_value;
	int adj_predicted_size;
	int i;
	int qlevel;
	int max_pfx_size;
	int qp;
	int unit_to_send_fflag = 0, unit_to_send_fpos = 0;
	int all_orig_within_qerr;
	int cpnt;
	int ssp;
	int force_mpp;
	int ich_disallow = 0;

	assert(unit<=MAX(MAX_UNITS_PER_GROUP,NUM_COMPONENTS));
	// *MODEL NOTE* MN_ENC_VLC_UNIT
	force_mpp = dsc_state->forceMpp;
	qp = dsc_state->masterQp;
	cpnt = dsc_state->unitCType[unit];
	ssp = dsc_state->unitSspMap[unit];

	qlevel = MapQpToQlevel(dsc_cfg, dsc_state, dsc_state->masterQp, cpnt);

	// Get predicted size for unit (adjusted for QP changes)
	adj_predicted_size = GetQpAdjPredSize(dsc_cfg, dsc_state, unit);
	if(dsc_cfg->bits_per_component==16 && unit==0 && 3*qlevel<=3-adj_predicted_size)
		ich_disallow = 1;		// No ICH allowed for special case
	
	if (unit==0) // Luma Unit
		// All Units follows First Luma Unit coding method
		dsc_state->prevIchSelected = dsc_state->ichSelected;

	if ((unit == unit_to_send_fflag) && ((dsc_state->groupCount % GROUPS_PER_SUPERGROUP) == 3) && 
		IsFlatnessInfoSent(dsc_cfg, qp))
	{
		if (dsc_state->prevFirstFlat<0) // None of units are flat
		{
			if (PRINT_DEBUG_VLC)
				fprintf(g_fp_dbg, "Flatness flag=0\n");
			AddBits(dsc_cfg, dsc_state, ssp, 0, 1); // add bit 0
		}
		else
		{
			if (PRINT_DEBUG_VLC)
				fprintf(g_fp_dbg, "Flatness flag=1\n");
			AddBits(dsc_cfg, dsc_state, ssp, 1, 1); // add bit 1
		}
	}
	//
	// unit_to_send_fpos ??????
	//
	if ((unit == unit_to_send_fpos) && ((dsc_state->groupCount % GROUPS_PER_SUPERGROUP) == 0) && 
		(dsc_state->firstFlat >= 0)) // first group and flat condition
	{
		if (dsc_state->masterQp >= SOMEWHAT_FLAT_QP_THRESH)
			AddBits(dsc_cfg, dsc_state, ssp, dsc_state->flatnessType, 1);
		else
			dsc_state->flatnessType = 0;
		// insert flat conditional bit
		AddBits(dsc_cfg, dsc_state, ssp, dsc_state->firstFlat, 2);
		if (PRINT_DEBUG_VLC)
			fprintf(g_fp_dbg, "First flat: %d, type: %d\n", dsc_state->firstFlat, dsc_state->flatnessType);
	}
	if ((unit>0) && dsc_state->ichSelected)	// OTHER UNITS IN H-MODE
	{
		for(i=0; i<dsc_state->ichIndicesInGroup; ++i)
			if(dsc_state->ichIndexUnitMap[i] == unit) // Unit associated with each ICH index (pixel) within an ICH grouping
				AddBits(dsc_cfg, dsc_state, ssp, dsc_state->ichLookup[i], ICH_BITS); // ICH indices for current group
		return;			// Nothing to do for other components once ICH selected
	}

	// figure out required size for unit
	max_size = 0;
	for ( i=0; i<SAMPLES_PER_UNIT; i++ ) {
		required_size[i] = FindResidualSize( quantized_residuals[i] );
		max_size = MAX( required_size[i], max_size ); // Find max required size from all units
	}

	if (PRINT_DEBUG_VLC)
	{
		fprintf(g_fp_dbg, "Component %d : ", cpnt);
		if (unit == 0)
			fprintf(g_fp_dbg, "uq=%d, ", dsc_state->masterQp);
	}
	//
	// ForceMpp Condition (6.6.1)
	//
	// Check if required size is bpc-qlevel ...
	if ( force_mpp || (max_size >= dsc_state->cpntBitDepth[cpnt] - qlevel ))  // ...if so, just use midpoint predictor
	{
		max_size = dsc_state->cpntBitDepth[cpnt] - qlevel; // Max size in Mpp Case
		for (i=0; i<SAMPLES_PER_UNIT; ++ i)
			required_size[i] = max_size;
	}
	// Sufficient Condition
	if (adj_predicted_size < max_size )
	{
		prefix_value = max_size - adj_predicted_size;
		size = max_size;
	}
	// Insufficient Condition?
	else 
	{
		prefix_value = 0; // size did not grow
		size = adj_predicted_size;
	}

	if (unit==0) // Escape code becomes size=0 if previous group was ICH mode, all other values get boosted by 1
		prefix_value += (dsc_state->prevIchSelected && !ich_disallow); // prev should be P-Mode

	// Determine if all 3 pixels are withing the quantization error
	dsc_state->ichSelected = 0;
	all_orig_within_qerr = 1;
	for (i=0; i<dsc_state->ichIndicesInGroup; ++i)
	{
		if (!dsc_state->origWithinQerr[i])
			all_orig_within_qerr = 0;
	}

	// ICH mode selection
	if ((force_p1_ich2!=1) && (unit==0) && all_orig_within_qerr && !dsc_state->forceMpp && !ich_disallow)
	{
		int alt_pfx, alt_size_to_generate;
		
		// Returns Escape Code
		alt_size_to_generate = EscapeCodeSize(dsc_cfg, dsc_state, dsc_state->masterQp);
		if (dsc_state->prevIchSelected)
			alt_pfx = 0;
		else  // For escape code, no need to send trailing one for prefix
			alt_pfx = alt_size_to_generate - adj_predicted_size;

		// IchDecision : Make ICH/P-mode decision (normal, not RDO)
		if((force_p1_ich2==2) || IchDecision(dsc_cfg, dsc_state, adj_predicted_size, alt_pfx, alt_size_to_generate))
		{
			dsc_state->ichSelected = 1;
			if (PRINT_DEBUG_VLC)
			{
				fprintf(g_fp_dbg, "size = pred (%d) + pfx (%d) = %d\n", adj_predicted_size, alt_pfx, adj_predicted_size + alt_pfx - dsc_state->prevIchSelected);
				fprintf(g_fp_dbg, "ICH mode (ecs=%d); indices: ", alt_size_to_generate); 
				for(i=0; i<dsc_state->ichIndicesInGroup; ++i)
					fprintf(g_fp_dbg, "%d ", dsc_state->ichLookup[i]);
				fprintf(g_fp_dbg, "\n");
			}

			if (dsc_state->prevIchSelected) // ICH -> ICH
				AddBits(dsc_cfg, dsc_state, ssp, 1, alt_pfx+1); // prefix is just bit "1"
			else // P -> ICH
				AddBits(dsc_cfg, dsc_state, ssp, 0, alt_pfx); 

			// Put IC$ samples
			for(i=0; i<dsc_state->ichIndicesInGroup; ++i)
				if(dsc_state->ichIndexUnitMap[i] == unit)
					AddBits(dsc_cfg, dsc_state, ssp, dsc_state->ichLookup[i], ICH_BITS); // insert suffix

			dsc_state->rcSizeUnit[0] = dsc_state->ichIndicesInGroup * ICH_BITS + 1;
			for(i=1; i<MAX_UNITS_PER_GROUP; ++i)
				dsc_state->rcSizeUnit[i] = 0;
			return;
		}	
	}

	// No need to send prefix if predicted size is max
	max_pfx_size = MaxResidualSize(dsc_cfg, dsc_state, cpnt, dsc_state->masterQp) + ((unit==0) && !ich_disallow) - adj_predicted_size;

	// Syntax modified to limit SE size
	//
	// ???????????????????????????????????????????
	//
	if(dsc_cfg->bits_per_component==16 && unit==0 && qlevel==0 && ich_disallow)
	{ 
		if(max_pfx_size + 16 * SAMPLES_PER_UNIT > 61)
		{
			max_pfx_size = prefix_value = 61 - 16 * SAMPLES_PER_UNIT;  // i.e., 13
			if(prefix_value >= max_pfx_size)  // Select MPP
			{
				prefix_value = max_pfx_size;
				size = max_size = dsc_state->cpntBitDepth[cpnt] - qlevel;  // i.e., 16 to select MPP
				for (i=0; i<SAMPLES_PER_UNIT; ++ i)
					required_size[i] = max_size;
			}
		}
	}

	if (PRINT_DEBUG_VLC)
		fprintf(g_fp_dbg, "size = pred (%d) + pfx (%d) = %d\n", adj_predicted_size, prefix_value, size);
	
	// Write bits to output bitstream
	if (prefix_value == max_pfx_size)  // Trailing "1" can be omitted
		AddBits(dsc_cfg, dsc_state, ssp, 0, max_pfx_size);
	else
		AddBits(dsc_cfg, dsc_state, ssp, 1, prefix_value+1);

	// *MODEL NOTE* MN_ENC_MPP_SELECT
	//
	// Selection between Block/Modified Median-Adaptive and Midpoint Prediction (6.4.4.2)
	//
	for ( i=0; i<SAMPLES_PER_UNIT; i++ ) 
	{
		if (max_size == dsc_state->cpntBitDepth[cpnt] - qlevel) // Select MPP
		{
			AddBits(dsc_cfg, dsc_state, ssp, dsc_state->quantizedResidualMid[unit][i], size);
			if (PRINT_DEBUG_VLC)
				fprintf(g_fp_dbg, "Sample delta %d = %d\n", i, dsc_state->quantizedResidualMid[unit][i]);
			dsc_state->midpointSelected[unit] = 1;
		}
		else
		{
			AddBits(dsc_cfg, dsc_state, ssp, quantized_residuals[i], size);
			if (PRINT_DEBUG_VLC)
				fprintf(g_fp_dbg, "Sample delta %d = %d\n", i, quantized_residuals[i]);
			dsc_state->midpointSelected[unit] = 0;
		}
	}

	// rate control size uses max required size plus 1 (for prefix value of 0)
	dsc_state->rcSizeUnit[unit] = max_size*SAMPLES_PER_UNIT + 1;

	//
	// Predict size for next unit for this component
	// & store predicted size to state structure
	dsc_state->predictedSize[unit] = PredictSize(dsc_cfg, required_size);
}


//! Code one unit
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param byte_out_p  Pointer to compressed bits buffer (modified) */
void VLCGroup(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, unsigned char **byte_out_p, int force_p1_ich2)
{
	int i;
	int start_fullness[MAX_NUM_SSPS];
	int adjFullness;
	int maxBitsPerGroup;
	int bugFixCondition;
	int gc;
	
	for (i=0; i<dsc_state->unitsPerGroup; ++i)
		dsc_state->midpointSelected[i] = 0;

	for (i=0; i<dsc_state->numSsps; ++i)
		start_fullness[i] = dsc_state->encBalanceFifo[i].fullness;

	// 444; Unit is same as CType
	dsc_state->prevNumBits = dsc_state->numBits;

	// Check stuffing condition
	dsc_state->forceMpp = 0;
	// Force MPP mode if buffer fullness is low
	//  Buffer threshold is ceil(bpp * 3) - 3, the first term is how many
	//   bits are removed from the model, the second term (3) is the minimum
	//   number of bits that a group can be coded with		
	//
	// Buffer Level Tracker (6.8.1)
	//
	maxBitsPerGroup = (dsc_state->pixelsInGroup * dsc_cfg->bits_per_pixel + 15) >> 4;
	adjFullness = dsc_state->bufferFullness;
	// Bug fix for DSC 1.1:  check if fractional bpp programmed; if so, change the check to >=
	bugFixCondition = (dsc_cfg->bits_per_pixel * dsc_state->sliceWidth) & 0xf;   // Fractional bit left at end of slice
	if( (bugFixCondition && (dsc_state->numBitsChunk + maxBitsPerGroup + 8 == dsc_cfg->chunk_size * 8)) ||
		(dsc_state->numBitsChunk + maxBitsPerGroup + 8 > dsc_cfg->chunk_size * 8))
	{
		// Bit Suffering Detection 1
		// End of chunk check to see if there is a potential to underflow 
		// assuming adjustment bits are sent.
		adjFullness -= 8;
		if (adjFullness  < maxBitsPerGroup - dsc_state->unitsPerGroup)  // Force MPP is possible in VBR only at end of line to pad chunks to byte boundaries
			dsc_state->forceMpp = 1;
	} 
	// Bit Suffering Detection 2
	else if((!dsc_cfg->vbr_enable) && (dsc_state->pixelCount >= dsc_cfg->initial_xmit_delay))  // underflow isn't possible if we're not removing bits
	{
		if (adjFullness  < maxBitsPerGroup - dsc_state->unitsPerGroup)
			dsc_state->forceMpp = 1;
	}
	// get prefix and encode each units
	for(i=0; i<dsc_state->unitsPerGroup; ++i)
		VLCUnit(dsc_cfg, dsc_state, i, dsc_state->quantizedResidual[i], force_p1_ich2);

	// Keep track of fullness for each coded unit in the balance FIFO's
	for (i=0; i<dsc_state->numSsps; ++i)
	{
		fifo_put_bits(&(dsc_state->seSizeFifo[i]), dsc_state->encBalanceFifo[i].fullness - start_fullness[i], 8);
		if(dsc_state->encBalanceFifo[i].fullness - start_fullness[i] > dsc_state->maxSeSize[i])
			printf("SE Size FIFO too small\n");
	}

	gc = dsc_state->groupCount;  
	if (gc > dsc_cfg->mux_word_size + MAX_SE_SIZE - 3)
		ProcessGroupEnc(dsc_cfg, dsc_state, *byte_out_p);

	//if(dsc_cfg->native_422)
	if(0)				// Placeholder if there is a use case for 2 SSP implementation
	{
		for (i=0; i<dsc_state->numSsps; ++i)
			start_fullness[i] = dsc_state->encBalanceFifo[i].fullness;
		for(i=dsc_state->unitsPerGroup/2; i<dsc_state->unitsPerGroup; ++i)
			VLCUnit(dsc_cfg, dsc_state, i, dsc_state->quantizedResidual[i], force_p1_ich2);
		for (i=0; i<dsc_state->numSsps; ++i)
		{
			fifo_put_bits(&(dsc_state->seSizeFifo[i]), dsc_state->encBalanceFifo[i].fullness - start_fullness[i], 8);
			if(dsc_state->encBalanceFifo[i].fullness - start_fullness[i] > dsc_state->maxSeSize[i])
				printf("SE Size FIFO too small\n");
		}

		// 2nd mux time of 4:2:2 group
		if (dsc_state->groupCount*2+1 > dsc_cfg->mux_word_size + MAX_SE_SIZE - 3)
			ProcessGroupEnc(dsc_cfg, dsc_state, *byte_out_p);
	}

	dsc_state->bufferFullness += dsc_state->numBits - dsc_state->prevNumBits;
	if ( dsc_state->bufferFullness > dsc_cfg->rcb_bits ) {
		// This check may actually belong after tgt_bpg has been subtracted
		printf("The buffer model has overflowed.  This probably occurred due to an error in the\n");
		printf("rate control parameter programming.\n\n");
		printf( "ERROR: RCB overflow; size is %d, tried filling to %d\n", dsc_cfg->rcb_bits, dsc_state->bufferFullness );
		exit(1);
	}
	dsc_state->codedGroupSize = dsc_state->numBits - dsc_state->prevNumBits;

	dsc_state->prevMasterQp = dsc_state->masterQp;
	dsc_state->groupCountLine++;
}


//! Decode one unit
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure
	\param cpnt      Component to code
	\param quantized_residuals Quantized residuals
	\param byte_in_p Pointer to compressed bits buffer (modified) */
void VLDUnit(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int unit, int *quantized_residuals, unsigned char **byte_in_p)
{
	int required_size[NUM_COMPONENTS];
	int size;
	int max_size;
	int prefix_value;
	int adj_predicted_size;
	int i;
	int alt_size_to_generate;
	int use_ich;
	int qlevel;
	int max_prefix;
	int midpoint_selected;
	int qp;
	int unit_to_read_fflag, unit_to_read_fpos;
	int cpnt;
	int ssp;
	int ich_disallow = 0;

	qp = dsc_state->masterQp;
	cpnt = dsc_state->unitCType[unit];
	ssp = dsc_state->unitSspMap[unit];

	unit_to_read_fflag = 0;
	unit_to_read_fpos = 0;

	qlevel = MapQpToQlevel(dsc_cfg, dsc_state, dsc_state->masterQp, cpnt);

	adj_predicted_size = GetQpAdjPredSize(dsc_cfg, dsc_state, unit);

	if(dsc_cfg->bits_per_component==16 && unit==0 && 3*qlevel<=3-adj_predicted_size)
		ich_disallow = 1;		// No ICH allowed for special case

	if (unit==0)
	{
		dsc_state->prevIchSelected = dsc_state->ichSelected; //Refers Previously userd method
		dsc_state->ichSelected = 0;
	}
	if ((unit==unit_to_read_fflag) && ((dsc_state->groupCount % GROUPS_PER_SUPERGROUP) == 3))
	{
		if (IsFlatnessInfoSent(dsc_cfg, qp))
		{
			if (GetBits(dsc_cfg, dsc_state, ssp, 1, 0, *byte_in_p))
				dsc_state->prevFirstFlat = 0;
			else
				dsc_state->prevFirstFlat = -1;
			if (PRINT_DEBUG_VLC)
				fprintf(g_fp_dbg, "Flatness flag=%d\n", (dsc_state->prevFirstFlat==0));
		} else
			dsc_state->prevFirstFlat = -1;
	}
	if ((unit==unit_to_read_fpos) && ((dsc_state->groupCount % GROUPS_PER_SUPERGROUP) == 0))
	{
	    if (dsc_state->prevFirstFlat >= 0)
		{
			dsc_state->flatnessType = 0;
			if (dsc_state->masterQp >= SOMEWHAT_FLAT_QP_THRESH)
				dsc_state->flatnessType = GetBits(dsc_cfg, dsc_state, ssp, 1, 0, *byte_in_p);
			dsc_state->firstFlat = GetBits(dsc_cfg, dsc_state, ssp, 2, 0, *byte_in_p);
			if (PRINT_DEBUG_VLC)
				fprintf(g_fp_dbg, "First flat: %d, type: %d\n", dsc_state->firstFlat, dsc_state->flatnessType);
		}
		else
			dsc_state->firstFlat = -1;
	}
	if (dsc_state->ichSelected)
	{
		for(i=0; i<dsc_state->ichIndicesInGroup; ++i)
		{
			if(dsc_state->ichIndexUnitMap[i] == unit)
				dsc_state->ichLookup[i] = GetBits(dsc_cfg, dsc_state, ssp, ICH_BITS, 0, *byte_in_p);
		}
		if (PRINT_DEBUG_VLC && (unit == dsc_state->unitsPerGroup-1))
		{
			for(i=0; i<dsc_state->ichIndicesInGroup; ++i)
					fprintf(g_fp_dbg, "%d ", dsc_state->ichLookup[i]);
			fprintf(g_fp_dbg, "\n");
		}
		return;							// Don't read other 2 components if we're in ICH mode
	}

	max_prefix = MaxResidualSize(dsc_cfg, dsc_state, cpnt, dsc_state->masterQp) + ((unit==0) && !ich_disallow) - adj_predicted_size; // +(CType==0) for escape code
	prefix_value = 0;
	// Syntax modified to limit SE size
	if(dsc_cfg->bits_per_component==16 && unit==0 && qlevel==0 && ich_disallow)
	{ 
		int old_max_prefix;
		old_max_prefix = max_prefix;
		if(max_prefix + 16 * SAMPLES_PER_UNIT > 61)
		{
			max_prefix = 61 - 16 * SAMPLES_PER_UNIT;  // i.e., 13
			while ((prefix_value < max_prefix) && !GetBits(dsc_cfg, dsc_state, ssp, 1, 0, *byte_in_p))
				prefix_value++;
			if(prefix_value == max_prefix)  // Select MPP
				prefix_value = old_max_prefix;
		} else {
			while ((prefix_value < max_prefix) && !GetBits(dsc_cfg, dsc_state, ssp, 1, 0, *byte_in_p))
				prefix_value++;
		}
	} else {
		while ((prefix_value < max_prefix) && !GetBits(dsc_cfg, dsc_state, ssp, 1, 0, *byte_in_p))
			prefix_value++;
	}

	if (PRINT_DEBUG_VLC)
	{
		fprintf(g_fp_dbg, "Component %d : ", cpnt);
		if (unit == 0)
			fprintf(g_fp_dbg, "uq=%d, ", dsc_state->masterQp);
	}

	if ((unit==0) && dsc_state->prevIchSelected && !ich_disallow)
		size = adj_predicted_size + prefix_value - 1;
	else
		size = adj_predicted_size + prefix_value;
	
	if (PRINT_DEBUG_VLC)
		fprintf(g_fp_dbg, "size = pred (%d) + pfx (%d) = %d\n", adj_predicted_size, prefix_value, size);
	
	alt_size_to_generate = EscapeCodeSize(dsc_cfg, dsc_state, dsc_state->masterQp);  //escape code length for ich mode

	if (dsc_state->prevIchSelected)
		use_ich = !ich_disallow && (prefix_value==0); // None prefix? ICH Select
	else
		use_ich = !ich_disallow && (size >= alt_size_to_generate); // Overlength prefix? ICH Select

	if ((unit==0) && use_ich)  // ICH hit
	{
		dsc_state->ichSelected = 1;
		dsc_state->rcSizeUnit[0] = 1 + ICH_BITS * dsc_state->ichIndicesInGroup;
		for(i=1; i<dsc_state->unitsPerGroup; ++i)
			dsc_state->rcSizeUnit[i] = 0;

		for(i=0; i<dsc_state->ichIndicesInGroup; ++i)
		{
			if(dsc_state->ichIndexUnitMap[i] == unit)
				dsc_state->ichLookup[i] = GetBits(dsc_cfg, dsc_state, ssp, ICH_BITS, 0, *byte_in_p);
		}

		if (PRINT_DEBUG_VLC)
			fprintf(g_fp_dbg, "ICH mode (ecs=%d); indices: ", alt_size_to_generate);

		return;
	}

	// *MODEL NOTE* MN_DEC_MPP_SELECT
	midpoint_selected = (size==dsc_state->cpntBitDepth[cpnt] - qlevel); // MPP Condition
	dsc_state->useMidpoint[unit] = midpoint_selected;

	// Get bits from input bitstream
	max_size = 0;
	for ( i=0; i<SAMPLES_PER_UNIT; i++ ) {
		quantized_residuals[i] = GetBits(dsc_cfg, dsc_state, ssp, size, 1, *byte_in_p); // Get 3 QR Values for Current Component,  P0, P1 and P2;
		if (PRINT_DEBUG_VLC)
			fprintf(g_fp_dbg, "Sample delta %d = %d\n", i, quantized_residuals[i]);
		required_size[i] = FindResidualSize( quantized_residuals[i] );
		max_size = MAX( required_size[i], max_size ); // Find Max Residual Size for Rate Control Part later
	}

	if (midpoint_selected)
	{
		max_size = size;
		for (i=0; i<SAMPLES_PER_UNIT; ++i)
			required_size[i] = size;
	}

	// rate control size uses max required size plus 1 (for prefix value of 0)
	dsc_state->rcSizeUnit[unit] = max_size*SAMPLES_PER_UNIT + 1;

	//
	// Predict size for next unit for this component
	// and store predicted size to state structure
	dsc_state->predictedSize[unit] =  PredictSize(dsc_cfg, required_size);
}


//! Decode one group
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure
	\param byte_in_p Pointer to compressed data buffer (modified) */
void VLDGroup(dsc_cfg_t* dsc_cfg, dsc_state_t* dsc_state, unsigned char** byte_in_p)
{
	int prevNumBits = dsc_state->numBits;
	int i;

	ProcessGroupDec(dsc_cfg, dsc_state, *byte_in_p);

	// *MODEL NOTE* MN_DEC_ENTROPY
	// 444; Unit is same as CType
	//if(!dsc_cfg->native_422) {
	if(1) {
		// 444; Unit is same as CType
		for(i=0; i<dsc_state->unitsPerGroup; ++i)
			VLDUnit(dsc_cfg, dsc_state, i, dsc_state->quantizedResidual[i], byte_in_p);
	}
	else   // Placeholder in case 2 SSP use case is required
	{
		for(i=0; i<dsc_state->unitsPerGroup/2; ++i)
			VLDUnit(dsc_cfg, dsc_state, i, dsc_state->quantizedResidual[i], byte_in_p);
		ProcessGroupDec(dsc_cfg, dsc_state, *byte_in_p);
		for(i=dsc_state->unitsPerGroup/2; i<dsc_state->unitsPerGroup; ++i)
			VLDUnit(dsc_cfg, dsc_state, i, dsc_state->quantizedResidual[i], byte_in_p);
	}

	dsc_state->prevMasterQp = dsc_state->masterQp;
	dsc_state->codedGroupSize = dsc_state->numBits - prevNumBits;
	dsc_state->bufferFullness += dsc_state->codedGroupSize;

	if ( dsc_state->bufferFullness > dsc_cfg->rcb_bits ) {
		// This check may actually belong after tgt_bpg has been subtracted
		printf("The buffer model has overflowed.  This probably occurred due to an attempt to decode an invalid DSC stream.\n\n");
		printf( "ERROR: RCB overflow; size is %d, tried filling to %d\n", dsc_cfg->rcb_bits, dsc_state->bufferFullness );
		dsc_state->errorOccurred = 1;
	}
	
	if ((dsc_state->groupCount % GROUPS_PER_SUPERGROUP) == dsc_state->firstFlat)
		dsc_state->origIsFlat = 1; // Current Group in this Super Group was flaged as flat?
	else
		dsc_state->origIsFlat = 0;
	dsc_state->groupCountLine++;
}


//! Reduce the bits required for a previous reconstructed line sample (currently via rounding)
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure
	\param x         The sample
	\param cpnt      Component to process */
int SampToLineBuf( dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int x, int cpnt)
{
	int shift_amount, round, storedSample;

	// *MODEL NOTE* MN_LINE_STORAGE
	///////////////////////////////////////////
	// Line Storage (6.3)
	// Allocate Storage to Store Reconstructed Pixel Value
	///////////////////////////////////////////
	shift_amount = MAX(dsc_state->cpntBitDepth[cpnt] - dsc_cfg->linebuf_depth, 0);
	if (shift_amount > 0)
		round = 1<<(shift_amount-1);
	else
		round = 0;

	storedSample = MIN(((x+round)>>shift_amount), (1<<dsc_cfg->linebuf_depth) - 1);
	return (storedSample << shift_amount);	
}


//! Initialize the DSC state
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\return          Returns dsc_state that was passed in */
dsc_state_t *InitializeDSCState( dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state )
{
	int i, j, k;

	memset(dsc_state, 0, sizeof(dsc_state_t));

	// Initialize quantization table
	switch(dsc_cfg->bits_per_component)
	{
	case 8:
		dsc_state->quantTableLuma = qlevel_luma_8bpc;
		dsc_state->quantTableChroma = qlevel_chroma_8bpc;
		break;
	case 10:
		dsc_state->quantTableLuma = qlevel_luma_10bpc;
		dsc_state->quantTableChroma = qlevel_chroma_10bpc;
		break;
	case 12:
		dsc_state->quantTableLuma = qlevel_luma_12bpc;
		dsc_state->quantTableChroma = qlevel_chroma_12bpc;
		break;
	case 14:
		dsc_state->quantTableLuma = qlevel_luma_14bpc;
		dsc_state->quantTableChroma = qlevel_chroma_14bpc;
		break;
	case 16:
		dsc_state->quantTableLuma = qlevel_luma_16bpc;
		dsc_state->quantTableChroma = qlevel_chroma_16bpc;
	}

	// initialize dsc state
	dsc_state->sliceWidth         = (dsc_cfg->native_420 | dsc_cfg->native_422) ? (dsc_cfg->slice_width>>1) : dsc_cfg->slice_width;
	dsc_state->numBits            = 0;
	dsc_state->prevNumBits        = 0;
	dsc_state->bufferFullness     = 0;
	dsc_state->stQp               = 0;
	dsc_state->prevQp             = 0;
	dsc_state->masterQp           = 0;
	dsc_state->prevMasterQp       = 0;
	dsc_state->rcSizeGroup        = 0;
	dsc_state->nonFirstLineBpgTarget = 0;
	dsc_state->rcXformOffset     = dsc_cfg->initial_offset + dsc_cfg->second_line_ofs_adj;
	dsc_state->throttleInt       = 0;
	dsc_state->throttleFrac      = 0;
	dsc_state->firstFlat         = -1;
	dsc_state->prevFirstFlat     = -1;
	dsc_state->unitsPerGroup     = 3;
	for ( i=0; i<MAX_UNITS_PER_GROUP; i++ ) {
		dsc_state->unitCType[i] = i;
		dsc_state->unitSspMap[i] = i;
		dsc_state->unitStartHPos[i] = 0;
		dsc_state->predictedSize[i] = 0;
		for ( j=0; j<SAMPLES_PER_UNIT; j++ )
			dsc_state->quantizedResidual[i][j] = 0;
	}
	for (i=0; i<MAX_PIXELS_PER_GROUP; ++i)
		dsc_state->ichIndexUnitMap[i] = i%3;
	for ( i=0; i<MAX_UNITS_PER_GROUP; i++ ) {
		dsc_state->rcSizeUnit[i] = 0;
	}

	for (i=0; i<dsc_state->numComponents; ++i)
	{	
		for(j=0; j<BP_RANGE; ++j)
		{
			for(k=0; k<BP_SIZE; ++k)
				dsc_state->lastErr[i][k][j] = 0;
		}
	}

	dsc_state->prevLinePred = (PRED_TYPE *)malloc(sizeof(PRED_TYPE) * (dsc_state->sliceWidth+PRED_BLK_SIZE-1) / PRED_BLK_SIZE);
	assert(dsc_state->prevLinePred != NULL);
	// Sets last predictor of line to MAP, since BP search is not done for partial groups
	memset(dsc_state->prevLinePred, 0, sizeof(PRED_TYPE) * (dsc_state->sliceWidth+PRED_BLK_SIZE-1) / PRED_BLK_SIZE);  
	
	for(i=0; i<MAX_NUM_SSPS; ++i)
	{
		fifo_init(&(dsc_state->shifter[i]), (dsc_cfg->mux_word_size + MAX_SE_SIZE + 7) / 8);
		fifo_init(&(dsc_state->encBalanceFifo[i]), ((dsc_cfg->mux_word_size + MAX_SE_SIZE - 1) * (MAX_SE_SIZE) + 7)/8);
		fifo_init(&(dsc_state->seSizeFifo[i]), (8 * (dsc_cfg->mux_word_size + MAX_SE_SIZE - 1) + 7)/8);
	}

	if(dsc_cfg->bits_per_component == 16)
	{
		dsc_state->maxSeSize[0] = dsc_state->maxSeSize[1] = dsc_state->maxSeSize[2] = dsc_state->maxSeSize[3] = 64;
	} else {
		dsc_state->maxSeSize[0] = dsc_cfg->bits_per_component * 4 + 4;  // prefix + code + up to 3 bits for flatness
		dsc_state->maxSeSize[1] = (dsc_cfg->bits_per_component + dsc_cfg->convert_rgb) * 4;  // prefix omitted
		dsc_state->maxSeSize[2] = (dsc_cfg->bits_per_component + dsc_cfg->convert_rgb) * 4;
		dsc_state->maxSeSize[3] = (dsc_cfg->bits_per_component) * 4;
	}

	dsc_state->pixelsInGroup = 3;
	if (dsc_cfg->native_422)
	{
		dsc_state->unitsPerGroup = 4;
		dsc_state->unitCType[3] = 3;
		dsc_state->numSsps = 4;
		dsc_state->numComponents = 4;
		dsc_state->ichIndexUnitMap[0] = 3;  // put first ICH index with 2nd luma unit
		dsc_state->ichIndexUnitMap[1] = 1;
		dsc_state->ichIndexUnitMap[2] = 2;
	} else {
		dsc_state->unitsPerGroup = 3;
		dsc_state->numComponents = 3;
		dsc_state->numSsps = 3;
	}
	dsc_state->native420 = dsc_cfg->native_420;
	dsc_state->ichIndicesInGroup = dsc_state->pixelsInGroup;

	dsc_state->history.valid = (int *)malloc(sizeof(int)*ICH_SIZE);
	assert(dsc_state->history.valid != NULL);
	for(i=0; i<ICH_SIZE; ++i)
		dsc_state->history.valid[i] = 0;
	for(i=0; i<dsc_state->numComponents; ++i)
	{
		dsc_state->history.pixels[i] = (unsigned int*)malloc(sizeof(unsigned int)*ICH_SIZE);
	}
	dsc_state->ichSelected = 0;


	return dsc_state;
}


//! Calculate offset & scale values for rate control
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param vPos    Current vertical position in slice
	\param group_count Group counter
	\param scale     Scale factor for RC (returned)
	\param bpgOffset BPG Offset value for RC (returned)  */
void CalcFullnessOffset(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int vPos, int group_count, int *scale, int *bpg_offset)
{
	// positive offset takes bits away, negative offset provides more bits
	int current_bpg_target;
	int unity_scale;
	int increment;

	// *MODEL NOTE* MN_CALC_SCALE_OFFSET
	unity_scale = (1<<RC_SCALE_BINARY_POINT);

	if (group_count == 0)
	{
		dsc_state->currentScale = dsc_cfg->initial_scale_value;
		dsc_state->scaleAdjustCounter = 1;
	} 
	else if ((vPos == 0) && (dsc_state->currentScale > unity_scale))  // Reduce scale at beginning of slice
	{
		dsc_state->scaleAdjustCounter++;
		if(dsc_state->scaleAdjustCounter >= dsc_cfg->scale_decrement_interval)
		{
			dsc_state->scaleAdjustCounter = 0;
			dsc_state->currentScale--;
		}
	}
	else if (dsc_state->scaleIncrementStart)
	{
		dsc_state->scaleAdjustCounter++;
		if (dsc_state->scaleAdjustCounter >= dsc_cfg->scale_increment_interval)
		{
			dsc_state->scaleAdjustCounter = 0;
			dsc_state->currentScale++;
		}
	}

	// Account for first line boost
	if (vPos == 0) 
	{
		current_bpg_target = dsc_cfg->first_line_bpg_ofs;
		increment = -(dsc_cfg->first_line_bpg_ofs << OFFSET_FRACTIONAL_BITS);
	} else {
		current_bpg_target = -(dsc_cfg->nfl_bpg_offset >> OFFSET_FRACTIONAL_BITS);
		increment = dsc_cfg->nfl_bpg_offset;
	}

	// Account for 2nd line boost
	if (vPos == 1) 
	{
		current_bpg_target += dsc_cfg->second_line_bpg_ofs;
		increment += -(dsc_cfg->second_line_bpg_ofs << OFFSET_FRACTIONAL_BITS);
		if(!dsc_state->secondOffsetApplied)
		{
			dsc_state->secondOffsetApplied = 1;
			dsc_state->rcXformOffset -= dsc_cfg->second_line_ofs_adj;
		}
	} else {
		current_bpg_target += -(dsc_cfg->nsl_bpg_offset >> OFFSET_FRACTIONAL_BITS);
		increment += dsc_cfg->nsl_bpg_offset;
	}

	// Account for initial delay boost
	if (dsc_state->pixelCount < dsc_cfg->initial_xmit_delay)
	{
		int num_pixels;

		if(dsc_state->pixelCount == 0)
			num_pixels = dsc_state->pixelsInGroup;
		else
			num_pixels = dsc_state->pixelCount - dsc_state->prevPixelCount;
		num_pixels = MIN(dsc_cfg->initial_xmit_delay - dsc_state->pixelCount, num_pixels);
		increment -= (dsc_cfg->bits_per_pixel * num_pixels) << (OFFSET_FRACTIONAL_BITS - 4);
	} 
	else
	{
		if (dsc_cfg->scale_increment_interval && !dsc_state->scaleIncrementStart && (vPos > 0) && (dsc_state->rcXformOffset > 0))
		{
			dsc_state->currentScale = 9;
			dsc_state->scaleAdjustCounter = 0;
			dsc_state->scaleIncrementStart = 1;
		}
	}
		
	dsc_state->prevPixelCount = dsc_state->pixelCount;

	current_bpg_target -= (dsc_cfg->slice_bpg_offset>>OFFSET_FRACTIONAL_BITS); 
	increment += dsc_cfg->slice_bpg_offset;

	dsc_state->throttleFrac += increment;
	dsc_state->rcXformOffset += dsc_state->throttleFrac >> OFFSET_FRACTIONAL_BITS;
	dsc_state->throttleFrac = dsc_state->throttleFrac & ((1<<OFFSET_FRACTIONAL_BITS)-1);
	
	if(dsc_state->rcXformOffset < dsc_cfg->final_offset)
		dsc_state->rcOffsetClampEnable = 1;

	if(dsc_state->rcOffsetClampEnable)
		dsc_state->rcXformOffset = MIN(dsc_state->rcXformOffset, dsc_cfg->final_offset);

	*scale = dsc_state->currentScale;
	*bpg_offset = current_bpg_target;
}


//! Convert original pixels in pic_t format to an array of unsigned int for easy access/
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param ip        Input picture
	\param vPos      Which line of slice to use */
void PopulateOrigLine(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, pic_t *ip, int vPos)
{
	int i, cpnt, pic_w, yidx;
	int xstart, lastpos;

	xstart = dsc_cfg->xstart >> (dsc_cfg->native_420 || dsc_cfg->native_422);
	for (cpnt = 0; cpnt < dsc_state->numComponents; ++cpnt)
	{
		pic_w = ip->w;
		if (dsc_cfg->native_422 && (cpnt>=1) && (cpnt<=2))
			pic_w >>= 1;
		for (i=0; i<dsc_state->sliceWidth+PADDING_RIGHT; ++i)
		{
			if (dsc_cfg->native_422 && (cpnt==0 || cpnt==3))
				lastpos = MIN(pic_w, (xstart + dsc_state->sliceWidth)*2) - 1;
			else
				lastpos = MIN(pic_w, xstart + dsc_state->sliceWidth) - 1;

			yidx = MIN(dsc_cfg->ystart+vPos, ip->h-1);
			// Padding for lines that fall off the bottom of the raster uses midpoint value
			if(dsc_cfg->ystart+vPos >= ip->h)
				dsc_state->origLine[cpnt][i+PADDING_LEFT] = 1<<(dsc_state->cpntBitDepth[cpnt]-1);
			else if (dsc_cfg->native_422)
			{
				switch (cpnt)
				{
				case 0:
					dsc_state->origLine[cpnt][i+PADDING_LEFT] = ip->data.yuv.y[yidx][MIN((xstart+i)*2, lastpos)];
					break;
				case 1:
					dsc_state->origLine[cpnt][i+PADDING_LEFT] = ip->data.yuv.u[yidx][MIN(xstart+i, lastpos)];
					break;
				case 2:
					dsc_state->origLine[cpnt][i+PADDING_LEFT] = ip->data.yuv.v[yidx][MIN(xstart+i, lastpos)];
					break;
				case 3:
					dsc_state->origLine[cpnt][i+PADDING_LEFT] = ip->data.yuv.y[yidx][MIN((xstart+i)*2+1, lastpos)];
					break;
				}			
			}
			else
			{
				switch (cpnt)
				{
				case 0:
					dsc_state->origLine[cpnt][i+PADDING_LEFT] = ip->data.yuv.y[yidx][MIN(xstart+i, lastpos)];
					break;
				case 1:
					dsc_state->origLine[cpnt][i+PADDING_LEFT] = ip->data.yuv.u[yidx][MIN(xstart+i, lastpos)];
					break;
				case 2:
					dsc_state->origLine[cpnt][i+PADDING_LEFT] = ip->data.yuv.v[yidx][MIN(xstart+i, lastpos)];
					break;
				}
			}
		}
	}
}


//! Main prediction loop
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param hPos		 Horizontal position of pixel being processed within slice
	\param vPos      Vertical position of pixel being processed within slice
	\param sampModCnt Sample counter within group
	\param qp		 Quantization parameter for current group */
void PredictionLoop(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int hPos, int vPos, int sampModCnt, int qp)
{
	int cpnt;
	int unit, qlevel;
	// WHICH METHOD TO PREDICT??
	PRED_TYPE pred2use; //enum { PT_MAP=0, PT_LEFT, PT_BLOCK } PRED_TYPE ; Determined at end of the current line
	int pred_x = 0, recon_x, actual_x = 0;
	int err_raw, err_q;
	int maxval;
	int residual_index;

	for ( unit = 0; unit < dsc_state->unitsPerGroup; unit++ ) { // NUM_Unit == 3 or 4 
		cpnt = dsc_state->unitCType[unit];
		
		// get QP Value from Rate Control Algorithm
		// HOW MANY FULL DATA?
		qlevel = MapQpToQlevel(dsc_cfg, dsc_state, qp, cpnt);

		if (vPos==0) // First Line!
		{
			// Use left predictor.  Modified MAP doesn't make sense since there is no previous line.
			pred2use = PT_LEFT; // PT_LEFT == 1
		}
		else
		{
			pred2use = dsc_state->prevLinePred[hPos/PRED_BLK_SIZE]; // from MMAP/BP Selection
		}
		residual_index = (sampModCnt-dsc_state->unitStartHPos[unit]);
			
		if (dsc_cfg->native_420) // ONLY FOR 4:2:0 MODE
		{
			if (cpnt == 2)  // chroma
			{
				if(vPos <= 1) // First Line
   		  			pred_x = SamplePredict( dsc_state, dsc_state->prevLine[cpnt+(vPos%2)], dsc_state->currLine[cpnt], hPos, 
											PT_LEFT, qlevel, unit);
				else
   		            pred_x = SamplePredict( dsc_state, dsc_state->prevLine[cpnt+(vPos%2)], dsc_state->currLine[cpnt], hPos, 
										PT_MAP, qlevel, unit);
			}
			else
				pred_x = SamplePredict( dsc_state, dsc_state->prevLine[cpnt], dsc_state->currLine[cpnt], hPos, 
											pred2use, qlevel, unit);
		} else
			pred_x = SamplePredict( dsc_state, dsc_state->prevLine[cpnt], dsc_state->currLine[cpnt], hPos, 
									pred2use, qlevel, unit);

		// Compute residual and quantize:
		if ( dsc_state->isEncoder ) {
			//
			// Calculate Residual and Quantize it (6.4.5)
			//
			actual_x = dsc_state->origLine[cpnt][hPos+PADDING_LEFT];

			err_raw = actual_x - pred_x;

			// First Line
			if (sampModCnt==0)
				dsc_state->masterQp = qp;

			// get Quantization Param
			qlevel = MapQpToQlevel(dsc_cfg, dsc_state, qp, cpnt);

			// get Quantized Residual
			err_q = QuantizeResidual( err_raw, qlevel );

			// MPP quantized residual check
			err_raw = actual_x - FindMidpoint(dsc_state, cpnt, qlevel);

			if(g_verbose)
				printf("Cpnt %d: ql=%d, pred=%d, act=%d, qr=%d, mp=%d, mqr=%d\n", cpnt, qlevel, pred_x, actual_x, err_q, actual_x - err_raw, QuantizeResidual(err_raw, qlevel));

			assert (residual_index>=0 && residual_index<SAMPLES_PER_UNIT);
			// store to array
			dsc_state->quantizedResidual[unit][residual_index] = err_q;

			// Calculate midpoint prediction error:
			dsc_state->quantizedResidualMid[unit][residual_index] = QuantizeResidual(err_raw, qlevel);
					
			// Midpoint residuals need to be bounded to BPC-QP in size, this is for some corner cases:
			// If an MPP residual exceeds this size, the residual is changed to the nearest residual with a size of cpntBitDepth - qLevel.
			//
			// FIND NEAREST Q_RESIDUAL (6.4.5)
			//
			if (dsc_state->quantizedResidualMid[unit][residual_index] > 0)
				while (FindResidualSize(dsc_state->quantizedResidualMid[unit][residual_index]) > MaxResidualSize(dsc_cfg, dsc_state, cpnt, qp))
					dsc_state->quantizedResidualMid[unit][residual_index]--;
			else
				while (FindResidualSize(dsc_state->quantizedResidualMid[unit][residual_index]) > MaxResidualSize(dsc_cfg, dsc_state, cpnt, qp))
					dsc_state->quantizedResidualMid[unit][residual_index]++;
							
		}
		else  // DECODER:
		{
			// Decoder takes error from bitstream
			err_q = dsc_state->quantizedResidual[unit][residual_index];

			qlevel = MapQpToQlevel(dsc_cfg, dsc_state, qp, cpnt);

			// Use midpoint prediction if selected
			if (dsc_state->useMidpoint[unit])
			{
				pred_x = FindMidpoint(dsc_state, cpnt, qlevel);
			}
			if(g_verbose)
				printf("Cpnt %d: ql=%d, pred=%d, qr=%d, usemp=%d\n", cpnt, qlevel, pred_x, err_q, dsc_state->useMidpoint[unit]);
		}

		//-----------------------------------------------------------------
		// reconstruct
		// *MODEL NOTE* MN_IQ_RECON
		///////////////////////////////////////////////////////////////////
		// Inverse Quantization and Reconstruction (6.4.6)
		// Reconstruct Pixel to Use it for Next Group Encoding
		///////////////////////////////////////////////////////////////////
		maxval = (1<<dsc_state->cpntBitDepth[cpnt]) - 1;
		recon_x = CLAMP(pred_x + (err_q << qlevel), 0, maxval);

		if (dsc_state->isEncoder)
		{
			int absErr;

			if (dsc_cfg->full_ich_err_precision)
				absErr = abs(actual_x - recon_x);
			else
				absErr = abs(actual_x - recon_x) >> (dsc_cfg->bits_per_component - 8);
#ifdef USE_MSE_FOR_ICH
			dsc_state->maxError[unit] += absErr * absErr;
#else
			dsc_state->maxError[unit] = MAX(dsc_state->maxError[unit], absErr);
#endif
		}
		if (dsc_state->isEncoder)
		{
			int midpoint_pred, midpoint_recon, absErr;

			// Encoder always computes midpoint value in case any residual size >= BPC - QP
			midpoint_pred = FindMidpoint(dsc_state, cpnt, qlevel);
			midpoint_recon = midpoint_pred + (dsc_state->quantizedResidualMid[unit][residual_index] << qlevel);
			dsc_state->midpointRecon[unit][residual_index] = CLAMP(midpoint_recon, 0, maxval);
			if (dsc_cfg->full_ich_err_precision)
				absErr = abs(actual_x - dsc_state->midpointRecon[unit][residual_index]);
			else
				absErr = abs(actual_x - dsc_state->midpointRecon[unit][residual_index]) >> (dsc_cfg->bits_per_component - 8);
#ifdef USE_MSE_FOR_ICH
			dsc_state->maxMidError[unit] += absErr * absErr;
#else
			dsc_state->maxMidError[unit] = MAX(dsc_state->maxMidError[unit], absErr);
#endif
		}

		// Save reconstructed value in line store
		dsc_state->currLine[cpnt][hPos+PADDING_LEFT] = recon_x;
		if(g_verbose)
			printf("Recon=%d\n", recon_x);
	}
}


//! Convert original pixels in pic_t format to an array of unsigned int for easy access/
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state DSC state structure 
	\param ip        Input picture
	\param vPos      Which line of slice to use */
void FlatnessAdjustment(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int hPos, int qp, int new_quant)
{
	int i;

	if ( dsc_state->isEncoder ) {
		// *MODEL NOTE* MN_ENC_FLATNESS_DECISION
		/////////////////////////////////////////////////////
		// Flatness Quantization Parameter Overrides (6.8.5, 6.8.5.1)
		// Make Decision whether Input is Flat or Not
		/////////////////////////////////////////////////////
		// IsFlatnessInfoSent : Checks masterQP is within MIN/MAX Val
		if (IsFlatnessInfoSent(dsc_cfg, qp) && ((dsc_state->groupCount % GROUPS_PER_SUPERGROUP) == 3))
		{
			if (dsc_state->firstFlat >= 0)
				dsc_state->prevIsFlat = 1;
			else
				dsc_state->prevIsFlat = 0;
			dsc_state->prevFirstFlat = -1;

			for (i=0; i<GROUPS_PER_SUPERGROUP; ++i)
			{
				int flatness_type;
						
				flatness_type = IsOrigFlatHIndex(dsc_cfg, dsc_state, hPos + (i+1)*dsc_state->pixelsInGroup);
				if (!dsc_state->prevIsFlat && flatness_type)
				{
					dsc_state->prevFirstFlat = i;
					dsc_state->prevFlatnessType = flatness_type - 1;
					break;
				}
				dsc_state->prevIsFlat = flatness_type;
			}
		} else if (!IsFlatnessInfoSent(dsc_cfg, qp)
			&& ((dsc_state->groupCount % GROUPS_PER_SUPERGROUP)==3))
		{
			dsc_state->prevFirstFlat = -1;
		}
		else if ((dsc_state->groupCount % GROUPS_PER_SUPERGROUP)==0)
		{
			dsc_state->firstFlat = dsc_state->prevFirstFlat;
			dsc_state->flatnessType = dsc_state->prevFlatnessType;
		}
		dsc_state->origIsFlat = 0;
		if ((dsc_state->firstFlat >= 0) &&
			((dsc_state->groupCount % GROUPS_PER_SUPERGROUP) == dsc_state->firstFlat))
			dsc_state->origIsFlat = 1;
	}

	// *MODEL NOTE* MN_FLAT_QP_ADJ
	/////////////////////////////////////////////////////////////////
	// Encoder and Decoder Flatness QP Adjustment (6.8.5.2)
	// Drops QP Quickly When Input is Flat or Flat-Like
	/////////////////////////////////////////////////////////////////
	if (dsc_cfg->dsc_version_minor == 1)
	{
		if (dsc_state->origIsFlat && (dsc_state->masterQp < dsc_cfg->rc_range_parameters[NUM_BUF_RANGES-1].range_max_qp))
		{
			if ((dsc_state->flatnessType==0) || (dsc_state->masterQp<SOMEWHAT_FLAT_QP_THRESH)) // Somewhat flat
			{
				dsc_state->stQp = MAX(dsc_state->stQp - SOMEWHAT_FLAT_QP_DELTA, 0);
			} else {		// very flat
				dsc_state->stQp = dsc_cfg->very_flat_qp; 
			}
		}
	} else {
		if (hPos >= dsc_state->sliceWidth-1)
		{
			dsc_state->origIsFlat = 1;
			dsc_state->flatnessType = 1; //0;
		}
		if (dsc_state->origIsFlat && (dsc_state->masterQp < dsc_cfg->rc_range_parameters[NUM_BUF_RANGES-1].range_max_qp))
		{
			if ((dsc_state->flatnessType==0) || (dsc_state->masterQp<SOMEWHAT_FLAT_QP_THRESH)) // Somewhat flat
			{
				dsc_state->stQp = MAX(dsc_state->stQp - SOMEWHAT_FLAT_QP_DELTA, 0);
				dsc_state->prevQp = MAX(dsc_state->prevQp - SOMEWHAT_FLAT_QP_DELTA, 0);
			} else {		// very flat
				dsc_state->stQp = dsc_cfg->very_flat_qp; 
				dsc_state->prevQp = dsc_cfg->very_flat_qp;
			}
		}
	}
}


//! Main DSC encoding and decoding algorithm
/*! \param isEncoder Flag indicating whether to do an encode (1) or decode (0)
    \param dsc_cfg   DSC configuration structure
	\param ip        Input picutre
	\param op        Output picture (modified, only affects area of current slice)
	\param cmpr_buf  Compressed data buffer (modified for encoder)
	\param temp_pic  Array of two temporary pictures to use for RGB-YCoCg conversion and back 
	\param chunk_sizes Array to hold sizes in bytes for each slice multiplexed chunk (modified for encoder) */
int DSC_Algorithm(int isEncoder, dsc_cfg_t* dsc_cfg, pic_t* ip, pic_t* op, unsigned char* cmpr_buf, pic_t **temp_pic, int *chunk_sizes)
{
	dsc_state_t dsc_state_storage, *dsc_state;
	pic_t* pic, *orig_op, *opic;
	int i;
	int vPos;
	int hPos;
	int done;
	int sampModCnt; //Index of current pixel within group 
	int cpnt; 
	int *currLine[NUM_COMPONENTS];
	int *prevLine[NUM_COMPONENTS+1];
	int lbufWidth;
	int range[NUM_COMPONENTS];
	int qp;
	int group_count = 0;
	int throttle_offset, bpg_offset;
	int scale;
	int new_quant;
	int unit;

#ifdef PRINTDEBUG
	if(isEncoder)
		g_fp_dbg = fopen("log_encode.txt","wt");
	else
		g_fp_dbg = fopen("log_decode.txt","wt");
#endif

	orig_op = op; //OUTPUT PICTURE
	// dsc_cfg->convert_rgb "True" means "Input is a RGB format"
	//
	// STEP 1. CONVERT RGB to YCoCg-R Format
	//
	if ( dsc_cfg->convert_rgb ) {
		// convert rgb to ycocg
		pic = temp_pic[0];
		opic = temp_pic[1];
		op = opic;
		rgb2ycocg(ip, pic, dsc_cfg);
	} else {
		// no color conversion
		pic = ip;
	}
	
	if (isEncoder && dsc_cfg->native_422 && dsc_cfg->convert_rgb)
	{
		// Chroma 444 to 422 conversion.
		yuv_444_422_region(pic, dsc_cfg);
	}

	if ( pic->bits != dsc_cfg->bits_per_component )
	{
		printf("ERROR: Expect picture bit depth to match configuration\n");
		exit(1);
	}

	dsc_state = InitializeDSCState( dsc_cfg, &dsc_state_storage );
	dsc_state->isEncoder = isEncoder;
	dsc_state->chunkSizes = chunk_sizes; // Array to hold sizes in bytes for each slice multiplexed chunk

	// line buffers have padding to left and right
	lbufWidth = dsc_state->sliceWidth + PADDING_LEFT + PADDING_RIGHT; // pixels to left and right
	// PADDING_LEFT : Pixels to pad line arrays to the left
	// PADDING_RIGHT : Pixels to pad line arrays to the right

	for ( i=0; i<NUM_COMPONENTS; i++ ) // NUM_COMPONENTS == 4
	{
		range[i] = 1<<dsc_cfg->bits_per_component;
		dsc_state->cpntBitDepth[i] = dsc_cfg->bits_per_component; // Bit depth for each component
		if(dsc_cfg->convert_rgb && (i!=0) && (i!=3) && (dsc_cfg->bits_per_component!=16))
		{
			range[i] *= 2; //WHY
			dsc_state->cpntBitDepth[i]++; //WHY
		}
	}

	// initialize DSC variables
	for ( cpnt = 0; cpnt<dsc_state->numComponents; cpnt++ )
	{
		int initValue;
		initValue = 1 << (dsc_state->cpntBitDepth[cpnt]-1);

		dsc_state->currLine[cpnt] = currLine[cpnt] = (int*) malloc(lbufWidth*sizeof(int));
		dsc_state->prevLine[cpnt] = prevLine[cpnt] = (int*) malloc(lbufWidth*sizeof(int));
		//SBR:OPTION 5
        if (dsc_cfg->native_420 && cpnt == 2)		// Extra chroma buffer
            dsc_state->prevLine[cpnt+1] = prevLine[cpnt+1] = (int*) malloc(lbufWidth*sizeof(int));
		  	 
		dsc_state->origLine[cpnt] = (int*) malloc(lbufWidth*sizeof(int));
		for ( i=0; i<lbufWidth; i++ ) {
			currLine[cpnt][i] = initValue;
			prevLine[cpnt][i] = initValue;
	  	    //SBR:OPTION 5
			if (dsc_cfg->native_420 && cpnt == 2)		// Extra chroma buffer
    	        prevLine[cpnt+1][i] = initValue;
	  	    
		}
		for (i=0; i<ICH_SIZE; ++i)
			dsc_state->history.pixels[cpnt][i] = initValue;  // Needed for 4:2:0 chroma
	}

	//--------------------------------------------------------------------------
	// sample range handling
	//
	dsc_state->groupCountLine = 0;
	
	// If decoder, read first group's worth of data
	if ( !isEncoder )
		VLDGroup( dsc_cfg, dsc_state, &cmpr_buf ); // Get QP from bitstream

	dsc_state->hPos = 0;
	dsc_state->vPos = 0;

	if (isEncoder)
		// Convert original pixels in pic_t format to an array of unsigned int for easy access
		PopulateOrigLine(dsc_cfg, dsc_state, pic, 0);
		
	vPos = 0; // Vertical position of pixel being processed within slice
	hPos = 0; // Horizontal position of pixel being processed within slice
	sampModCnt = 0; // Leftmost Pixel in Group
	done = 0; // end flag
	new_quant = 0;
	qp = 0;

	while ( !done ) {
		dsc_state->vPos = vPos;
		// init QP
		// Most Recent QP Value
		if(sampModCnt==0)
			dsc_state->masterQp = qp=dsc_state->prevQp;
		
		if ((hPos % dsc_state->pixelsInGroup)==0) // First Pixel in a Group
			// dsc_state->pixelsInGroup : Number of pixels in current group
		{
			// Note that UpdateICHistory works on the group to the left
			for (i=0; i<dsc_state->pixelsInGroup; ++i)
				UpdateICHistory(dsc_cfg, dsc_state, currLine, hPos+i, vPos);
			// Clear ICH errors:
			for (i=0; i<dsc_state->unitsPerGroup; ++i)
			{
				dsc_state->maxError[i] = 0;
				dsc_state->maxMidError[i] = 0;
				dsc_state->maxIchError[i] = 0;
			} // Values for P-Mode / ICH-Mode Selection
		}
		// Calculate Prediction Pixel P and Quantize, and Reconstruct it
		PredictionLoop(dsc_cfg, dsc_state, hPos, vPos, sampModCnt, qp);

		// Update QP per group
		// Last Pixel in a Group
		if ((sampModCnt==dsc_state->pixelsInGroup-1) || (hPos==dsc_state->sliceWidth-1))
		{
			// Check Flatness of Current Group
			FlatnessAdjustment(dsc_cfg, dsc_state, hPos, qp, new_quant);
		}
				
		if(isEncoder)
		{
			int ichhit, scnt;

			scnt = sampModCnt;
			if (sampModCnt == 0)
			{
				for (i=0; i<dsc_state->pixelsInGroup; ++i)
					dsc_state->origWithinQerr[i] = 1;
			}

			//Encoder function to determine whether or not the current sample is within the quantization error of any ICH entry 
			ichhit = IsOrigWithinQerr(dsc_cfg, dsc_state, hPos, vPos, dsc_state->masterQp, sampModCnt);
			if (ichhit) // WHEN ICH MODE IS A VALID OPTION FOR THE GROUP
			{
				unsigned int orig[NUM_COMPONENTS];
				int u;

				for(i=0; i<dsc_state->numComponents; ++i)
					orig[i] = dsc_state->origLine[i][PADDING_LEFT+hPos];

				//Encoder function to select the best history entry & return the corresponding index
				dsc_state->ichLookup[scnt] = PickBestHistoryValue(dsc_cfg, dsc_state, hPos, orig);

				// Have to do ICH lookup here & remember pixel values
				if(g_verbose)
					printf("hPos = %d\n", hPos);
				// SET PREDICTED VALUE "P" USING HISTORY
				HistoryLookup(dsc_cfg, dsc_state, dsc_state->ichLookup[scnt], dsc_state->ichPixels[scnt], hPos,
                              (vPos==0) || (vPos==1 && dsc_cfg->native_420), vPos%2);

				for (u=0; u < dsc_state->unitsPerGroup; ++u)
				{
					int absErr;

					// ichPixels : ICH pixel samples selected for current group (for encoder)
					cpnt = dsc_state->unitCType[u];
					if (dsc_cfg->full_ich_err_precision) // Full Error Precision for ICH Decision
						// Section 6.5.3.3
						absErr = abs((int)dsc_state->ichPixels[scnt][cpnt] - (int)orig[cpnt]);
					else
					absErr = abs((int)dsc_state->ichPixels[scnt][cpnt] - (int)orig[cpnt]) >> (dsc_cfg->bits_per_component - 8);

#ifdef USE_MSE_FOR_ICH
					dsc_state->maxIchError[u] += absErr * absErr;
#else
					dsc_state->maxIchError[u] = MAX(dsc_state->maxIchError[u], absErr);
#endif
				}
			}
		}

		sampModCnt++; // Next Pixel in current Group [0, 1, 2]

		// AFTER 3 Pixels Encoded or Decoded
		if ( (sampModCnt >= dsc_state->pixelsInGroup ) || (hPos+1 == dsc_state->sliceWidth)) { // Last Pixel in a Group "OR" Outbound pixel outside of slice
			// Pad partial group at the end of the line
			if (sampModCnt < dsc_state->pixelsInGroup) // A Pixel in a group is located in outside of a slice.
			{
				for (i = sampModCnt; i<dsc_state->pixelsInGroup; ++i)
				{
					// Set ICH values to the rightmost value
					dsc_state->ichLookup[i] = dsc_state->ichLookup[sampModCnt-1];
					hPos++;
				}
			}

			dsc_state->hPos = hPos;

			if ( isEncoder ) {
				VLCGroup( dsc_cfg, dsc_state, &cmpr_buf, 0);

				// If it turned out we needed midpoint prediction, change the reconstructed pixels to use midpoint results
				UpdateMidpoint(dsc_cfg, dsc_state, currLine, hPos - dsc_state->pixelsInGroup + 1);

				// If it turned out that IC$ was selected, change the reconstructed pixels to use IC$ values
				if (dsc_state->ichSelected)
					UseICHistory(dsc_cfg, dsc_state, currLine);

				for (cpnt=0; cpnt < dsc_state->numComponents; ++cpnt)
				{
					dsc_state->leftRecon[cpnt] = currLine[cpnt][(MIN(dsc_state->sliceWidth-1, hPos))+PADDING_LEFT];
				}
				
				// Calculate scale & offset for RC
				CalcFullnessOffset(dsc_cfg, dsc_state, vPos, group_count, &scale, &bpg_offset);
				group_count++;
				dsc_state->groupCount = group_count;
				throttle_offset = dsc_state->rcXformOffset;

				// Do rate control
				RateControl( dsc_cfg, dsc_state, throttle_offset, bpg_offset, group_count, scale, sampModCnt );  // Group is finished
				new_quant = dsc_state->stQp;

				if (dsc_state->bufferFullness < 0)
				{
					if (dsc_cfg->vbr_enable)
					{
						dsc_state->bitsClamped += -dsc_state->bufferFullness;
						dsc_state->bufferFullness = 0;
					}
					else
					{
						printf("The buffer model encountered an underflow.  This may have occurred due to\n");
						printf("an excessively high programmed constant bit rate\n");
						exit(1);
					}
				}
				// Reset residuals to 0
				for(unit=0; unit<dsc_state->unitsPerGroup; ++unit)
				{
					dsc_state->midpointSelected[unit] = 0;
					for(i=0; i<SAMPLES_PER_UNIT; ++i)
						dsc_state->quantizedResidual[unit][i] = dsc_state->quantizedResidualMid[unit][i] = 0;
				}
			}
			else  //DECODER
			{  
				if(dsc_state->ichSelected)
				{
					if(g_verbose)
						printf("Using ICH %d ", hPos-dsc_state->pixelsInGroup+1);
					for(i=0; i<dsc_state->pixelsInGroup; ++i)
					{
						HistoryLookup(dsc_cfg, dsc_state, dsc_state->ichLookup[i], dsc_state->ichPixels[i], 
                                      hPos-dsc_state->pixelsInGroup+1, 
                                      (vPos==0) || (vPos==1 && dsc_cfg->native_420), vPos%2);
						if(g_verbose)
							printf("%d %d %d  ", dsc_state->ichPixels[i][0], dsc_state->ichPixels[i][1], dsc_state->ichPixels[i][2]);
					}				
					UseICHistory(dsc_cfg, dsc_state, dsc_state->currLine);
				}

				if(g_verbose)
					printf("sample=%d\n", dsc_state->currLine[2][5+0]);
				for (cpnt=0; cpnt < dsc_state->numComponents; ++cpnt)
				{
					dsc_state->leftRecon[cpnt] = currLine[cpnt][(MIN(dsc_state->sliceWidth-1, hPos))+PADDING_LEFT];
				}

				// Calculate scale & offset for RC
				CalcFullnessOffset(dsc_cfg, dsc_state, vPos, group_count, &scale, &bpg_offset);
				group_count++;
				dsc_state->groupCount = group_count;
				throttle_offset = dsc_state->rcXformOffset;

				// Do rate control
				RateControl( dsc_cfg, dsc_state, throttle_offset, bpg_offset, group_count, scale, sampModCnt );
				new_quant = dsc_state->stQp;

				if (dsc_state->bufferFullness < 0)
				{
					if (dsc_cfg->vbr_enable)
					{
						dsc_state->bitsClamped += -dsc_state->bufferFullness;
						dsc_state->bufferFullness = 0;
					}
					else
					{
						printf("The buffer model encountered an underflow.  This may have occurred due to\n");
						printf("an excessively high constant bit rate or due to an attempt to decode an\n");
						printf("invalid DSC stream.\n");
						dsc_state->errorOccurred = 1;
					}
				}

				dsc_state->masterQp = dsc_state->prevQp;
				if (hPos>=dsc_state->sliceWidth-1)
				{
					dsc_state->groupCountLine = 0;
				}
				if ((hPos<dsc_state->sliceWidth-1) || (vPos<dsc_cfg->slice_height-1))  // Don't decode if we're done
					VLDGroup( dsc_cfg, dsc_state, &cmpr_buf );
			}

			sampModCnt = 0;
		}

#ifdef PRINTDEBUG
		fflush(g_fp_dbg);
#endif
		hPos++;
		if ( hPos >= dsc_state->sliceWidth ) {
			int mod_hPos, op_x, op_y;
			// end of line
			// Mapping Reconstructed Value to Out Picture *op
			for (mod_hPos=0; mod_hPos<dsc_state->sliceWidth; ++mod_hPos)
			{
				if (PRINT_DEBUG_RECON)
					fprintf(g_fp_dbg, "\n%d, %d: ", mod_hPos, vPos);
				for (cpnt = 0; cpnt < dsc_state->numComponents; ++cpnt)
				{
					int w;
					if (PRINT_DEBUG_RECON)
					{
						fprintf(g_fp_dbg, "%d ", currLine[cpnt][mod_hPos+PADDING_LEFT]);
					}

					//op_x = (mod_hPos + dsc_cfg->xstart)>>hsampling;
					op_x = dsc_cfg->xstart + mod_hPos;
					w = op->w;
					if(dsc_cfg->native_422)
					{
						op_x = (dsc_cfg->xstart >> 1) + mod_hPos;
						w = op->w>>1;
					}
					else if (dsc_cfg->native_420)
						op_x = (dsc_cfg->xstart >> 1) + mod_hPos;
					op_y = (vPos + dsc_cfg->ystart);
					if ((op_y < op->h) && (op_x < (w)))
					{
						if(dsc_cfg->native_422)
						{
							switch(cpnt)
							{
							case 0: op->data.yuv.y[op_y][op_x*2] = currLine[cpnt][PADDING_LEFT+mod_hPos]; break;
							case 1: op->data.yuv.u[op_y][op_x] = currLine[cpnt][PADDING_LEFT+mod_hPos]; break;
							case 2: op->data.yuv.v[op_y][op_x] = currLine[cpnt][PADDING_LEFT+mod_hPos]; break;
							case 3: op->data.yuv.y[op_y][op_x*2+1] = currLine[cpnt][PADDING_LEFT+mod_hPos]; break;
							}
						} else {
							switch(cpnt)
							{
							case 0: op->data.yuv.y[op_y][op_x] = currLine[cpnt][PADDING_LEFT+mod_hPos]; break;
							case 1: op->data.yuv.u[op_y][op_x] = currLine[cpnt][PADDING_LEFT+mod_hPos]; break;
							case 2: op->data.yuv.v[op_y][op_x] = currLine[cpnt][PADDING_LEFT+mod_hPos]; break;
							}
						}
					}
				}
#ifdef PRINTDEBUG
				fflush(g_fp_dbg);
#endif
			}

			// reduce number of bits per sample in line buffer (replicate pixels in left/right padding)
			for ( cpnt = 0; cpnt<dsc_state->numComponents; cpnt++ )
				for ( i=0; i<lbufWidth; i++ )
				{
					int width;
					width = dsc_state->sliceWidth;
					if ((dsc_cfg->native_420) && (cpnt == dsc_state->numComponents-1))
                    //SBR:OPTION 5
					     prevLine[cpnt+(vPos%2)][i] = SampToLineBuf( dsc_cfg, dsc_state, currLine[cpnt][CLAMP(i,PADDING_LEFT,PADDING_LEFT+width-1)], cpnt );
					else
						prevLine[cpnt][i] = SampToLineBuf( dsc_cfg, dsc_state, currLine[cpnt][CLAMP(i,PADDING_LEFT,PADDING_LEFT+width-1)], cpnt );
				}

			// BP search
			// Calculate BP/MMAP ERROR respectively and determine Prediction Method
			for (mod_hPos=0; mod_hPos<dsc_state->sliceWidth; ++mod_hPos)
			{
				for (cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
					BlockPredSearch(dsc_cfg, dsc_state, cpnt, prevLine, mod_hPos);
			}

			hPos = 0;
			vPos++;
			if (dsc_state->isEncoder)
				dsc_state->groupCountLine = 0;

			if ( vPos >= dsc_cfg->slice_height )
				done = 1;
			else if (isEncoder)
				PopulateOrigLine(dsc_cfg, dsc_state, pic, vPos);
		}
		//if((dsc_cfg->xstart + hPos == 1792) && (vPos + dsc_cfg->ystart == 540+25))
		//{
		//	printf("Debug\n");
		//	g_verbose = 1;
		//} else
		//	g_verbose = 0;		

		if(dsc_state->errorOccurred)
		{
			// Note: DSC requires that pixels are output for errored bitstreams but does not mandate what those pixels are
			ErrorHandler(dsc_cfg, dsc_state, hPos, vPos, op);
			done = 1;
		}
	}

	if ( sampModCnt != 0 && isEncoder ) {  // Pad last unit wih 0's if needed
		for (i = sampModCnt; i<MAX_PIXELS_PER_GROUP; ++i)
			for (cpnt = 0; cpnt<dsc_state->numComponents; cpnt++)
				dsc_state->quantizedResidual[cpnt][i] = 0;
		VLCGroup( dsc_cfg, dsc_state, &cmpr_buf, 0);
	}

	if (dsc_state->isEncoder)
	{
		while (dsc_state->seSizeFifo[0].fullness > 0)
			ProcessGroupEnc(dsc_cfg, dsc_state, cmpr_buf);
	}

	if (dsc_state->isEncoder && dsc_cfg->vbr_enable)
	{
		int accum_bytes = 0;

		if(dsc_state->chunkCount != dsc_cfg->slice_height-1)
		{
			while(dsc_state->chunkCount < dsc_cfg->slice_height-1)
				RemoveBitsEncoderBuffer(dsc_cfg, dsc_state);
			if(dsc_state->chunkCount > dsc_cfg->slice_height - 1)
			{
				printf("Chunk count was greater than the number of slice lines. This is an unexpected\n");
				printf("fatal error.\n");
				exit(1);
			}
		}

		for(i=0; i<dsc_cfg->slice_height-1; ++i)
			accum_bytes += dsc_state->chunkSizes[i];

		dsc_state->chunkSizes[dsc_state->chunkCount] = MAX(0, dsc_state->postMuxNumBits / 8 - accum_bytes);
	}

	if (dsc_cfg->native_422 && dsc_cfg->convert_rgb)
	{
		yuv_422_444_region(op, dsc_cfg);
	}

	if ( dsc_cfg->convert_rgb ) {
		// Convert YCoCg back to RGB again
		ycocg2rgb(op, orig_op, dsc_cfg);
	}

	for(cpnt=0; cpnt<dsc_state->numComponents; ++cpnt)
	{
		free(currLine[cpnt]);
		free(prevLine[cpnt]);
		//SBR:OPTION 5
		if (dsc_cfg->native_420 && cpnt == 2)		// Extra chroma buffer
		     free(prevLine[cpnt+1]);
					
		free(dsc_state->origLine[cpnt]);
		free(dsc_state->history.pixels[cpnt]);
	}
	for(cpnt=0; cpnt<MAX_NUM_SSPS; ++cpnt)
	{
		fifo_free(&(dsc_state->shifter[cpnt]));
		fifo_free(&(dsc_state->encBalanceFifo[cpnt]));
		fifo_free(&(dsc_state->seSizeFifo[cpnt]));
	}

	free(dsc_state->prevLinePred);
	free(dsc_state->history.valid);

	if (isEncoder && (dsc_state->bufferFullness > ((dsc_cfg->initial_xmit_delay * dsc_cfg->bits_per_pixel) >> 4)))
	{
		printf("Too many bits are left in the rate buffer at the end of the slice.  This is most likely\n");
		printf("due to an invalid RC configuration.\n");
		exit(1);
	}
#ifdef PRINTDEBUG
	fclose(g_fp_dbg);
#endif
	return dsc_state->postMuxNumBits;

}


//! Wrapper function for encode
/*! \param dsc_cfg   DSC configuration structure
    \param p_in      Input picture
	\param p_out     Output picture
	\param cmpr_buf  Pointer to empty buffer to hold compressed bitstream
	\param temp_pic  Array of two pictures to use as temporary storage for YCoCg conversions
	\return          Number of bits in the resulting compressed bitstream */
int DSC_Encode(dsc_cfg_t *dsc_cfg, pic_t *p_in, pic_t *p_out, unsigned char *cmpr_buf, pic_t **temp_pic, int *chunk_sizes)
{
	return DSC_Algorithm(1, dsc_cfg, p_in, p_out, cmpr_buf, temp_pic, chunk_sizes);
}


//! Wrapper function for decode
/*! \param dsc_cfg   DSC configuration structure
	\param p_out     Output picture
	\param cmpr_buf  Pointer to buffer containing compressed bitstream
	\param temp_pic  Array of two pictures to use as temporary storage for YCoCg conversions */
void DSC_Decode(dsc_cfg_t *dsc_cfg, pic_t *p_out, unsigned char *cmpr_buf, pic_t **temp_pic)
{
	DSC_Algorithm(0, dsc_cfg, p_out, p_out, cmpr_buf, temp_pic, NULL);
}


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
#include <stdlib.h>

#include "fifo.h"
#include "dsc_types.h"
#include "dsc_utils.h"

/*! \file multiplex.c
 *    Substream multiplex support functions */

//! In non-substream mode, this function writes the contents of the shifter to the output bitstream
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state Current DSC state
	\param buf       Pointer to output buffer */
void WriteEntryToBitstream(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, unsigned char *buf)
{
	int i;
	int sz;
	unsigned int d;

	for (i=0; i<dsc_state->numSsps; ++i)
	{
		sz = fifo_get_bits(&(dsc_state->seSizeFifo[i]), 8, 0);
		if (sz>32)
		{
			d = fifo_get_bits(&(dsc_state->encBalanceFifo[i]), 32, 0);
			putbits(d, 32, buf, &dsc_state->postMuxNumBits);
			sz -= 32;
		}
		d = fifo_get_bits(&(dsc_state->encBalanceFifo[i]), sz, 0);
		if (dsc_state->encBalanceFifo[i].fullness!=0)
			printf("Error: shifter underflow\n");
		putbits(d, sz, buf, &dsc_state->postMuxNumBits);
	}
}


//! Add bits to one of the encoder FIFO's
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state Current DSC state
	\param CType	 Which component FIFO to add to
	\param d		 Data to add
	\param nbits     Number of bits to add */
void AddBits(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int CType, int d, int nbits)
{
	fifo_put_bits(&(dsc_state->encBalanceFifo[CType]), d, nbits);
	dsc_state->numBits += nbits;
}


//! Get bits from the input bitstream (in non-susbtream mode) or from one of the funnel shifters (in substream mode)
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state Current DSC state
	\param unit      Which substream (component) shifter to get data from
	\param nbits     Number of bits to retrieve 
	\param sign_extend Flag indicating whether return value should have sign bit extended
	\param buf       Input buffer (for non-substream mode)
	\return          Data from bitstream or shifter */
int GetBits(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, int unit, int nbits, int sign_extend, unsigned char *buf)
{
	dsc_state->numBits += nbits;

	return (fifo_get_bits(&(dsc_state->shifter[unit]), nbits, sign_extend));
}


//! Process a single group through the FIFO's, and generate 0 - 3 mux words
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state Current DSC state
	\param buf       Pointer to output buffer */
void ProcessGroupEnc(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, unsigned char *buf)
{
	int i, j;
	unsigned char d;
	int sz;


	// Check to see if mux words are needed and feed the FIFO's
	for (i=0; i<dsc_state->numSsps; ++i)
	{
		if (dsc_state->shifter[i].fullness < dsc_state->maxSeSize[i])
		{
			for (j=0; j<(dsc_cfg->mux_word_size / 8); ++j)
			{
				if (dsc_state->isEncoder)
				{
					sz = dsc_state->encBalanceFifo[i].fullness;
					if (sz >= 8)
						d = fifo_get_bits(&(dsc_state->encBalanceFifo[i]), 8, 0);
					else if (sz > 0)
						d = fifo_get_bits(&(dsc_state->encBalanceFifo[i]), sz, 0) << (8-sz);
					else
						d = 0;
					putbits(d, 8, buf, &dsc_state->postMuxNumBits);
				}
				else  // Decoder
				{
					d = getbits(8, buf, &dsc_state->numBits, 0);
				}
				fifo_put_bits(&dsc_state->shifter[i], d, 8);				
			}
		}
		// Virtual decoder
		sz = fifo_get_bits(&(dsc_state->seSizeFifo[i]), 8, 0);
		fifo_get_bits(&(dsc_state->shifter[i]), sz, 0);        // Remove one SE
	}
}


//! Process a single group through the FIFO's, and insert 0 - 3 mux words in shifters
/*! \param dsc_cfg   DSC configuration structure
    \param dsc_state Current DSC state
	\param buf       Pointer to input buffer */
void ProcessGroupDec(dsc_cfg_t *dsc_cfg, dsc_state_t *dsc_state, unsigned char *buf)
{
	int i, j;
	unsigned char d;

	// Check to see if mux words are needed and feed the FIFO's
	for (i=0; i<dsc_state->numSsps; ++i)
	{
		int k = dsc_state->shifter[i].fullness < dsc_state->maxSeSize[i];
		if (k) 
		{
			for (j=0; j<(dsc_cfg->mux_word_size / 8); ++j)
			{
				d = getbits(8, buf, &dsc_state->postMuxNumBits, 0);
				fifo_put_bits(&dsc_state->shifter[i], d, 8);				
			}
		}
	}
}

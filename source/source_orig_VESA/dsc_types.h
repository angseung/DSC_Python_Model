/***************************************************************************
*    Copyright (c) 2013-2015, Broadcom Corporation
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

#ifndef _DSC_TYPES_H_
#define _DSC_TYPES_H_

#include "fifo.h"

#define NUM_BUF_RANGES        15
#define NUM_COMPONENTS        4
#define MAX_UNITS_PER_GROUP   4
#define MAX_NUM_SSPS		  4   // Number of substreams (ie., substream processors)
#define SAMPLES_PER_UNIT      3
#define MAX_PIXELS_PER_GROUP  6
#define GROUPS_PER_SUPERGROUP 4
#define BP_RANGE              13 ///  was 10, modified for high throughput test modes
#define BP_SIZE				  3
#define PRED_BLK_SIZE		  3
#define ICH_BITS			  5
#define ICH_SIZE			  (1<<ICH_BITS)
#define ICH_PIXELS_ABOVE      7
#define ICH_LAMBDA			  4
#define OFFSET_FRACTIONAL_BITS  11
#define MAX_SE_SIZE           (4*dsc_cfg->bits_per_component+4)  // Maximum possible syntax element size
#define PPS_SIZE			  128
#define BP_EDGE_COUNT		  3
#define BP_EDGE_STRENGTH      32
#define PADDING_LEFT          5  // Pixels to pad line arrays to the left
#define PADDING_RIGHT         10  // Pixels to pad line arrays to the right
#define RC_SCALE_BINARY_POINT   3
//#define SOMEWHAT_FLAT_QP_THRESH (7+(2*(dsc_cfg->bits_per_component-8)))
//#define SOMEWHAT_FLAT_QP_THRESH (4+(2*(dsc_cfg->bits_per_component-8)))
//#define SOMEWHAT_FLAT_QP_DELTA  3
#define SOMEWHAT_FLAT_QP_THRESH (dsc_cfg->somewhat_flat_qp_thresh)
#define SOMEWHAT_FLAT_QP_DELTA  (dsc_cfg->somewhat_flat_qp_delta)
#define OVERFLOW_AVOID_THRESHOLD  (dsc_cfg->native_422 ? -224 : -172)
#define LARGE_INT			 (1<<30)

typedef enum { PT_MAP=0, PT_LEFT, PT_BLOCK } PRED_TYPE;

#define NUM_PRED_TYPES        (PT_BLOCK+BP_RANGE)

/// Configuration for a single RC model range
typedef struct dsc_range_cfg_s {
	int  range_min_qp;			///< Min QP allowed for this range
	int  range_max_qp;			///< Max QP allowed for this range
	int  range_bpg_offset;		///< Bits/group offset to apply to target for this group
} dsc_range_cfg_t;

/// Codec configuration
typedef struct dsc_cfg_s {
	int  linebuf_depth;		///< Bits / component for previous reconstructed line buffer
	int  rcb_bits;			///< Rate control buffer size (in bits); not in PPS, used only in C model for checking overflow
	int  bits_per_component; ///< Bits / component to code (must be 8, 10, or 12)
	int  convert_rgb;		///< Flag indicating to do RGB - YCoCg conversion and back (should be 1 for RGB input)
	int  slice_width;		///< Slice width
	int  slice_height;		///< Slice height
	int  simple_422;        ///< 4:2:2 simple mode (from PPS, 4:2:2 conversion happens outside of DSC encode/decode algorithm)
	int  native_422;		///< 4:2:2 native mode (no conversion is done)
	int  native_420;		///< 4:2:0 native mode (no conversion is done)
	int  pic_width;			///< Picture width
	int  pic_height;		///< Picture height
	int  rc_tgt_offset_hi;		///< Offset to bits/group used by RC to determine QP adjustment
	int  rc_tgt_offset_lo;		///< Offset to bits/group used by RC to determine QP adjustment
	int  bits_per_pixel;	///< Bits/pixel target << 4 (ie., 4 fractional bits)
	int  rc_edge_factor;	///< Factor to determine if an edge is present based on the bits produced
	int  rc_quant_incr_limit1; ///< Slow down incrementing once the range reaches this value
	int  rc_quant_incr_limit0; ///< Slow down incrementing once the range reaches this value
	int  initial_xmit_delay;	///< Number of pixels to delay the initial transmission
	int  initial_dec_delay;		///< Number of pixels to delay the VLD on the decoder, not including SSM
	int  block_pred_enable;	///< Block prediction range (in pixels)
	int  first_line_bpg_ofs; ///< Bits/group offset to use for first line of the slice
	int  second_line_bpg_ofs;  ///< For 4:2:0, bits/group offset to use for the 2nd line of the slice
	int  initial_offset;    ///< Value to use for RC model offset at slice start
	int  xstart;			///< X position in the picture of top-left corner of slice
	int  ystart;			///< Y position in the picture of top-left corner of slice
	int  rc_buf_thresh[NUM_BUF_RANGES-1];   ///< Thresholds defining each of the buffer ranges
	dsc_range_cfg_t rc_range_parameters[NUM_BUF_RANGES];  ///< Parameters for each of the RC ranges
	int  rc_model_size;     ///< Total size of RC model
	int  flatness_min_qp;	///< Minimum QP where flatness information is sent
	int  flatness_max_qp;   ///< Maximum QP where flatness information is sent
	int  flatness_det_thresh;  ///< MAX-MIN for all components is required to be <= this value for flatness to be used
	int  flatness_mod;         ///< Possible flatness modification
	int  initial_scale_value;  ///< Initial value for scale factor
	int  scale_decrement_interval;   ///< Decrement scale factor every scale_decrement_interval groups
	int  scale_increment_interval;   ///< Increment scale factor every scale_increment_interval groups
	int  nfl_bpg_offset;		///< Non-first line BPG offset to use
	int  nsl_bpg_offset;		///< Non-2nd line BPG offset to use
	int  slice_bpg_offset;		///< BPG offset used to enforce slice bit constraint
	int  final_offset;          ///< Final RC linear transformation offset value
	int  vbr_enable;			///< Enable on-off VBR (ie., disable stuffing bits)
	int  mux_word_size;         ///< Mux word size (in bits) for SSM mode
	int  chunk_size;            ///< The (max) size in bytes of the "chunks" that are used in slice multiplexing
	int  pps_identifier;		///< Placeholder for PPS identifier
	int  very_flat_qp;          ///< (TEST ONLY) QP to use when "very flat" is indicated
	int  somewhat_flat_qp_delta; ///< (TEST ONLY) QP delta for detecting "somewhat flat"
	int  somewhat_flat_qp_thresh; ///< (TEST ONLY) QP threshold for "somewhat flat" indication
	int  second_line_ofs_adj;    ///< Adjustment to offset for 2nd line in 4:2:0 (since chroma has no prediction)
	int  dsc_version_minor;			///< DSC minor version
	int	 full_ich_err_precision;	///< Use full error precision for ICH selection (1.2 only)
} dsc_cfg_t;

/// The ICH state
typedef struct dsc_history_s {
	unsigned int *pixels[NUM_COMPONENTS];  ///< Array of pixels that corresponds to the shift register (position 0 is MRU)
	int *valid;				  ///< Valid bits for each pixel in the history (position 0 is MRU)
} dsc_history_t;

/// The DSC state
typedef struct dsc_state_s {
	int isEncoder;			///< 1=encode, 0=decode
	int numBits;			///< Number of bits read/written
	int prevNumBits;		///< Number of bits at end of previous group
	int postMuxNumBits;     ///< Number of bits read/written post-mux
	int bufferSize;			///< Buffer size in bits
	int bufferFullness;		///< Current buffer fullness
	int bpgFracAccum;		///< Accumulator for fractional bits/group
	int rcXformOffset;		///< Offset used for RC model (to get rcXformOffset from spec, subtract rc_model_size from this value)
	int throttleInt;		///< Integer portion of RC offset
	int throttleFrac;		///< Fractional portion of RC offset
	int nonFirstLineBpgTarget;  ///< Bits/group target for non-first-lines
	int currentScale;		///< Current scale factor used for RC model
	int scaleAdjustCounter;  ///< Counter used to compute when to adjust scale factor
	int predErr[NUM_COMPONENTS][BP_RANGE];   ///< Errors for all pred types for each component
	int stQp;				///< QP from RC
	int prevQp;		  		///< QP for previous group from RC
	int quantizedResidual[MAX_UNITS_PER_GROUP][SAMPLES_PER_UNIT];  ///< Quantized residuals for current group
	int quantizedResidualMid[MAX_UNITS_PER_GROUP][SAMPLES_PER_UNIT];  ///< Quantized residuals assuming midpoint prediction for current group
	int rcSizeUnit[MAX_UNITS_PER_GROUP];  ///< Size for each unit assuming size was perfectly predicted
	int rcSizeGroup;		///< Sum of RcSizeUnits for previous group
	int codedGroupSize;		///< Size of previous group in bits
	int predictedSize[MAX_UNITS_PER_GROUP];  ///< Predicted sizes for next DSU code 
	PRED_TYPE *prevLinePred;  ///< BP selection decsion buffer (since model calculates BP offset one line ahead of time)
	int bpCount;			///< Number of times in a row block prediction had the lowest cost
	int lastEdgeCount;		///< How long ago we saw the last edge (for BP)
	int edgeDetected;       ///< Was an edge detected for BP
	int lastErr[NUM_COMPONENTS][BP_SIZE][BP_RANGE];  ///< 3-pixel SAD's for each of the past 3 3-pixel-wide prediction blocks for each BP offset
	dsc_history_t history;	///< The ICH
	int hPos;				///< Current horizontal position within the slice
	int vPos;				///< Current vertical position within the slice
	int masterQp;			///< QP used for the current group
	int prevMasterQp;			///< QP used for the previous group
	int midpointRecon[MAX_UNITS_PER_GROUP][MAX_PIXELS_PER_GROUP];  ///< Reconstructed samples assuming midpoint prediction is selected
	int midpointSelected[MAX_UNITS_PER_GROUP];  ///< Encoder flags indicating for each unit whether midpoint prediction is selected
	int useMidpoint[MAX_UNITS_PER_GROUP];  ///< Decoder flags indicating for each unit whether midpoint prediction should be used 
	int ichSelected;		///< Flag indicating ICH is used for current group
	int prevIchSelected;	///< Flag indicating ICH is used for previous group
	int ichLookup[MAX_PIXELS_PER_GROUP];		///< ICH indices for current group
	//SBR OPTION 5
	int *prevLine[NUM_COMPONENTS+1];  ///< Previous line reconstructed samples 
	int *currLine[NUM_COMPONENTS];  ///< Current line reconstructed samples
	int *origLine[NUM_COMPONENTS];  ///< Current line original samples (for encoder)
	int origWithinQerr[MAX_PIXELS_PER_GROUP];   ///< Encoder flags indicating that original pixels are within the quantization error
	unsigned int ichPixels[MAX_PIXELS_PER_GROUP][NUM_COMPONENTS];  ///< ICH pixel samples selected for current group (for encoder)
	int leftRecon[NUM_COMPONENTS];		///< Previous group's rightmost reconstructed samples (used for midpoint prediction)
	int cpntBitDepth[NUM_COMPONENTS];   ///< Bit depth for each component
	int origIsFlat;			///< Flag indicating that original pixels are flat for this group
	int groupCount;			///< Current group count
	int firstFlat;			///< If -1, none of the 4 group set are flat, otherwise indicates which group is the one where flatness starts
	int prevFirstFlat;			///< If -1, none of the 4 group set are flat, otherwise indicates which group is the one where flatness starts
	int prevIsFlat;			///< Flag indicating the previous group is flat
	int flatnessType;		///< 0 = somewhat flat; 1 = very flat
	int prevFlatnessType;   ///< The flatnessType coded with the previous group
	int maxError[MAX_UNITS_PER_GROUP];  ///< Max error for each component using DPCM
	int maxMidError[MAX_UNITS_PER_GROUP];  ///< Max error for each component using MPP
	int maxIchError[MAX_UNITS_PER_GROUP];  ///< Max error for each unit using ICH
	int groupCountLine;    ///< Group count for current line
	int prevRange;         ///< RC range for previous group
	fifo_t shifter[MAX_NUM_SSPS];		///< Decoder funnel shifter
	fifo_t encBalanceFifo[MAX_NUM_SSPS];	///< Encoder balance FIFO's
	fifo_t seSizeFifo[MAX_NUM_SSPS];	///< Syntax element sizes
	int forceMpp;			///< Flag to force MPP mode to prevent underflow
	int *quantTableLuma;	///< Quantization table for luma
	int *quantTableChroma;	///< Quantization table for chroma
	int numBitsChunk;		///< Number of bits output for the current chunk
	int chunkCount;         ///< Chunk number currently being coded
	int bitsClamped;        ///< Number of bits clamped by buffer level tracker in VBR mode
	int *chunkSizes;        ///< For encoders, stores the chunk sizes for each chunk in slice multiplexing
	int pixelCount;         ///< Number of pixels that have been processed
	int prevPixelCount;		///< Number of pixels that have been processed as of the previous group
	int chunkPixelTimes;    ///< Number of pixels that have been generated for the current slice line
	int rcOffsetClampEnable; ///< Set to true after rcXformOffset falls below final_offset - rc_model_size
	int scaleIncrementStart; ///< Flag indicating that the scale increment has started
	int unitsPerGroup;       ///< Number of units in a group
	int unitStartHPos[MAX_UNITS_PER_GROUP];	 ///< Starting pixel offset for each unit
	int unitCType[MAX_UNITS_PER_GROUP];    ///< Component type for each unit
	int unitSspMap[MAX_UNITS_PER_GROUP];	///< Substream processor associated with each unit
	int maxSeSize[MAX_NUM_SSPS];	///< Max syntax element size for each SSP
	int ichIndexUnitMap[MAX_PIXELS_PER_GROUP];    ///< Unit associated with each ICH index (pixel) within an ICH grouping
	int numSsps;            ///< Number of SSP's
	int pixelsInGroup;		///< Number of pixels in current group
	int ichIndicesInGroup;  ///< Number of ICH indices to represent current group
	int useIch;				///< Indicates ICH/P mode decision
	int numComponents;      ///< Number of components (includes coefficient bands for each component)
	int rcIntegrator;       ///< LT integrator for PID control RC
	int secondOffsetApplied;  ///< Flag indicating the 2nd line offset was applied
	int errorOccurred;      ///< An error was detected, just output data until the end of the slice
	int native420;		    ///< Are we in 4:2:0 high throughput mode?
	int sliceWidth;			///< Slice_width or slice_width/2 if 4:2:0 mode selected
	int bitSaveMode;		///< Bit save state variable for RC
	int mppState;			///< MPP state variable for RC
} dsc_state_t;

#endif // __DSC_TYPES_H_

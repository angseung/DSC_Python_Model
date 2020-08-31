import numpy as np
from math import ceil

rc_range_parameters_table = [
    np.array([
        [0, 4, 2],
        [0, 4, 0],
        [1, 5, 0],
        [1, 6, -2],
        [3, 7, -4],
        [3, 7, -6],
        [3, 7, -8],
        [3, 8, -8],
        [3, 9, -8],
        [3, 10, -10],
        [5, 10, -10],
        [5, 11, -12],
        [5, 11, -12],
        [9, 12, -12],
        [12, 13, -12]
    ]),
    np.array([
        [0, 8, 2],
        [4, 8, 0],
        [5, 9, 0],
        [5, 10, -2],
        [7, 11, -4],
        [7, 11, -6],
        [7, 11, -8],
        [7, 12, -8],
        [7, 13, -8],
        [7, 14, -10],
        [9, 14, -10],
        [9, 15, -12],
        [9, 15, -12],
        [13, 16, -12],
        [16, 17, -12]
    ]),
    np.array([
        [0, 12, 2],
        [4, 12, 0],
        [9, 13, 0],
        [9, 14, -2],
        [11, 15, -4],
        [11, 15, -6],
        [11, 15, -8],
        [11, 16, -8],
        [11, 17, -8],
        [11, 18, -10],
        [13, 18, -10],
        [13, 19, -12],
        [13, 19, -12],
        [17, 20, -12],
        [20, 21, -12]
    ]),
    np.array([
        [0, 2, 0],
        [0, 4, 0],
        [1, 5, 0],
        [1, 6, -2],
        [3, 7, -4],
        [3, 7, -6],
        [3, 7, -8],
        [3, 8, -8],
        [3, 8, -8],
        [3, 9, -10],
        [5, 9, -10],
        [5, 9, -12],
        [5, 9, -12],
        [7, 10, -12],
        [10, 11, -12]
    ]),
    np.array([
        [0, 2, 0],
        [2, 5, 0],
        [3, 7, 0],
        [4, 8, -2],
        [6, 9, -4],
        [7, 10, -6],
        [7, 11, -8],
        [7, 12, -8],
        [7, 12, -8],
        [7, 13, -10],
        [9, 13, -10],
        [9, 13, -12],
        [9, 13, -12],
        [11, 14, -12],
        [14, 15, -12]
    ]),
    np.array([
        [0, 6, 2],
        [4, 9, 2],
        [7, 11, 0],
        [8, 12, -2],
        [10, 13, -4],
        [11, 14, -6],
        [11, 15, -8],
        [11, 16, -8],
        [11, 16, -8],
        [11, 17, -10],
        [13, 17, -10],
        [13, 17, -12],
        [13, 17, -12],
        [15, 18, -12],
        [18, 19, -12]
    ])]

initial_xmit_delay_table = [512, 512, 512, 341, 341, 341]
first_lie_bpg_offset_table = [15, 15, 15, 15, 15, 15]
initial_offset_table = [6144, 6144, 6144, 2048, 2048, 2048]
flatness_min_qp_table = [3, 7, 11, 3, 7, 11]
flatness_max_qp_table = [12, 16, 20, 12, 16, 20]
rc_quant_incr_limit_table = [11, 15, 19, 11, 15, 19]

class initPps:
    def __init__(self):
        self.RESERVED = 0
        self.dsc_version_major = 0
        self.dsc_version_minor = 0
        self.pps_identifier = 0
        self.bits_per_component = 0
        self.line_buf_depth = 0
        self.block_pred_enable = 0
        self.convert_rgb = 0
        self.simple_422 = 0
        self.native_420 = 0
        self.native_422 = 0
        self.vbr_enable = 0
        self.bits_per_pixel = 0
        self.pic_height = 0
        self.pic_width = 0
        self.slice_height = 0
        self.slice_width = 0
        self.chunk_size = 0
        self.initial_xmit_delay = 0
        self.initial_dec_delay = 0
        self.initial_scale_value = 0
        self.scale_increment_interval = 0
        self.scale_decrement_interval = 0
        self.first_line_bpg_ofs = 0
        self.nfl_bpg_offset = 0
        self.slice_bpg_offset = 0
        self.initlal_offset = 0
        self.final_offset = 0
        self.flatness_min_qp = 0
        self.flatness_max_qp = 0
        self.flatness_det_thresh = 0
        self.rc_model_size = 0
        self.rc_edge_factor = 0
        self.rc_quant_icnr_limit0 = 0
        self.rc_quant_icnr_limit1 = 0
        self.rc_tgt_offset_hi = 0
        self.rc_tgt_offset_lo = 0
        self.rc_buf_thresh = 0
        self.rc_range_parameters = 0
        self.second_line_bpg_offset = 0
        self.nsl_bpg_offset = 0
        self.second_line_offset_adj = 0

        ################################################
        #self.numSsp          = 0
        self.bitsPerPixel    = 0
        self.maxSeSize_Y     = 0
        self.maxSeSize_C     = 0
        self.numExtraMuxBits = 0
        self.pixelsPerGroup  = 0
        self.groupsPerLine   = 0
        self.groupsTotal     = 0
        self.rcb_bits        = 0
        self.hrdDelay        = 0

    def cal_params_enc(self, user_opt):
        self.dsc_version_major = user_opt['dsc_version_major']
        self.dsc_version_minor = user_opt['dsc_version_minor']
        self.pps_identifier = user_opt['pps_identifier']
        self.bits_per_component = user_opt['bits_per_component']
        self.line_buf_depth = user_opt['line_buf_depth']
        self.block_pred_enable = user_opt['block_pred_enable']
        self.convert_rgb = user_opt['convert_rgb']
        self.simple_422 = user_opt['simple_422']
        self.native_420 = user_opt['native_420']
        self.native_422 = user_opt['native_422']
        self.vbr_enable = user_opt['vbr_enable']
        self.bits_per_pixel = user_opt['bits_per_pixel']
        self.pic_height = user_opt["pic_height"]
        self.pic_width = user_opt["pic_width"]
        self.slice_height = user_opt["slice_height"]
        self.slice_width = user_opt["slice_width"]

        if self.bits_per_pixel == (8 << 4):
            if self.bits_per_component == 8:
                table_sel = 0
            elif self.bits_per_component == 10:
                table_sel = 1
            elif self.bits_per_component == 12:
                table_sel = 2
            muxWordSize = 48

        elif self.bits_per_pixel == (12 << 4):
            if self.bits_per_component == 8:
                table_sel = 3
            elif self.bits_per_component == 10:
                table_sel = 4
            elif self.bits_per_component == 12:
                table_sel = 5
            muxWordSize = 64

        chunk_size = int(ceil(self.slice_width * (self.bits_per_pixel >> 4) / 8.0))
        rc_model_size = 8192
        rc_edge_factor = 6
        rc_tgt_offset_hi = 3
        rc_tgt_offset_lo = 3
        initial_xmit_delay = initial_xmit_delay_table[table_sel]
        numSsps = 4 if self.native_422 else 3
        initial_fullness_offset = 6144

        maxSeSize_Y = 4 * self.bits_per_component + 4
        maxSeSize_C = 4 * self.bits_per_component

        if self.convert_rgb:
            numExtraMuxBits = numSsps * (muxWordSize + (4 * self.bits_per_component + 4) - 2)
        elif not self.native_422:
            numExtraMuxBits = numSsps * muxWordSize + maxSeSize_Y + 2 * maxSeSize_C - 2
        else:
            numExtraMuxBits = numSsps * muxWordSize + maxSeSize_Y + 3 * maxSeSize_C - 2

        slicebit = 8 * chunk_size * self.slice_height

        while ((numExtraMuxBits > 0) and ((slicebit - numExtraMuxBits) % muxWordSize)):
            numExtraMuxBits -= 1

        pixelsPerGroup = 3
        groupsPerLine = int(self.slice_width / pixelsPerGroup)
        groupsTotal = groupsPerLine * self.slice_height

        ######## FOR CALCULATED PPSs...

        # initial_dec_delay = []
        # initial_scale_value = []
        # scale_increment_interval = []
        # scale_decrement_interval = []
        first_line_bpg_offset = first_lie_bpg_offset_table[table_sel]
        nfl_bpg_offset = int(ceil(float((first_line_bpg_offset << 11) / (self.slice_height - 1))))
        # slice_bpg_offset = []
        initial_offset = initial_offset_table[table_sel]
        final_offset = rc_model_size - ((initial_xmit_delay * self.bits_per_pixel + 8) >> 4) + numExtraMuxBits
        flatness_min_qp = flatness_min_qp_table[table_sel]
        flatness_max_qp = flatness_max_qp_table[table_sel]
        rc_quant_incr_limit0 = rc_quant_incr_limit_table[table_sel]
        rc_quant_incr_limit1 = rc_quant_incr_limit_table[table_sel]
        rc_buf_thresh = np.array([896, 1792, 2688, 3584, 4480, 5376, 6272, 6720, 7168, 7616, 7744, 7872, 8000, 8064])
        rc_range_parameters = rc_range_parameters_table[table_sel]
        second_line_bpg_offset = self.native_420 * 12
        nsl_bpg_offset = int(ceil(float(second_line_bpg_offset << 11)) / (self.slice_height - 1))
        second_line_offset_adj = 512 * self.native_420

        if (self.native_422 or self.native_420):
            grpcnt = int(ceil(float(initial_xmit_delay) / pixelsPerGroup))
            maxOffset = compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay, self.bits_per_pixel,
                                          first_line_bpg_offset, second_line_bpg_offset, self.slice_bpg_offset,
                                          nfl_bpg_offset,
                                          nsl_bpg_offset, grpcnt, self.native_420)
            maxOffset = max(maxOffset,
                            compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay, self.bits_per_pixel,
                                              first_line_bpg_offset, second_line_bpg_offset, self.slice_bpg_offset,
                                              nfl_bpg_offset,
                                              nsl_bpg_offset, groupsPerLine, self.native_420))
            maxOffset = max(maxOffset,
                            compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay, self.bits_per_pixel,
                                              first_line_bpg_offset, second_line_bpg_offset, self.slice_bpg_offset,
                                              nfl_bpg_offset,
                                              nsl_bpg_offset, groupsPerLine * 2, self.native_420))
            rsbMin = rc_model_size - initial_fullness_offset + maxOffset

        else:
            rbsMin = int(rc_model_size - initial_fullness_offset + int(
                ceil(initial_xmit_delay * (self.bits_per_pixel >> 4)))) + groupsPerLine * first_line_bpg_offset

        hrdDelay = int((ceil(float(rbsMin) / (self.bits_per_pixel >> 4))))
        rcb_bits = int((ceil(float(hrdDelay) * (self.bits_per_pixel >> 4))))
        initial_dec_delay = hrdDelay - initial_xmit_delay
        initial_scale_value = int((8 * rc_model_size) / (rc_model_size - initial_offset))
        finalScaleValue = int(8 * rc_model_size / (rc_model_size - final_offset))
        slice_bpg_offset = int(
            ceil(float((1 << 11)) * (rc_model_size - initial_offset + numExtraMuxBits) / (groupsTotal)))

        if finalScaleValue > 9:
            # scale_increment_interval = int(float(1 << 11) * final_offset / (float(finalScaleValue - 9) * nfl_bpg_offset + slice_bpg_offset + nfl_bpg_offset))
            scale_increment_interval = (int)(float((1 << 11)) * final_offset / (
                        float((finalScaleValue - 9)) * (nfl_bpg_offset + slice_bpg_offset + nsl_bpg_offset)))

        else:
            scale_increment_interval = 0

        scale_decrement_interval = int(groupsPerLine / (initial_scale_value - 8))
        # nsl_bpg_offset = first_line_bpg_offset / (slice_height - 1)

        #### Insert ALL PPS to dict
        self.chunk_size               = chunk_size
        self.initial_xmit_delay       = initial_xmit_delay
        self.initial_dec_delay        = initial_dec_delay
        self.initial_scale_value      = initial_scale_value
        self.scale_increment_interval = scale_increment_interval
        self.scale_decrement_interval = scale_decrement_interval
        self.first_line_bpg_ofs       = first_line_bpg_offset
        self.nfl_bpg_offset           = nfl_bpg_offset
        self.slice_bpg_offset         = slice_bpg_offset
        self.initlal_offset           = initial_offset
        self.final_offset             = final_offset
        self.flatness_min_qp          = flatness_min_qp
        self.flatness_max_qp          = flatness_max_qp
        self.rc_model_size            = rc_model_size
        self.rc_edge_factor           = rc_edge_factor
        self.rc_quant_incr_limit0     = rc_quant_incr_limit0
        self.rc_quant_incr_limit1     = rc_quant_incr_limit1
        self.rc_tgt_offset_hi         = rc_tgt_offset_hi
        self.rc_tgt_offset_lo         = rc_tgt_offset_lo
        self.rc_buf_thresh            = rc_buf_thresh
        self.rc_range_parameters      = rc_range_parameters
        self.second_line_bpg_offset   = second_line_bpg_offset
        self.nsl_bpg_offset           = nsl_bpg_offset
        self.second_line_offset_adj   = second_line_offset_adj

        # Below parameters are constants calculated from PPS values
        self.numSsps         = numSsps
        self.bitsPerPixel    = self.bits_per_pixel >> 4 # Removed Fractional 4-bits, Original Value!
        self.maxSeSize_Y     = maxSeSize_Y
        self.maxSeSize_C     = maxSeSize_C
        self.numExtraMuxBits = numExtraMuxBits
        self.pixelsPerGroup  = pixelsPerGroup
        self.groupsPerLine   = groupsPerLine
        self.groupsTotal     = groupsTotal
        self.rcb_bits        = rcb_bits
        self.hrdDelay        = hrdDelay
        self.somewhat_flat_qp_thresh = 7 + (2 * (self.bits_per_component - 8))
        self.somewhat_flat_qp_delta = 4
        self.flatness_det_thresh = 2
        self.muxWordSize = muxWordSize ## 2020.07.27 added

    def cal_params_dec(self):
        # self.dsc_version_major = user_opt['dsc_version_major']
        # self.dsc_version_minor = user_opt['dsc_version_minor']
        # self.pps_identifier = user_opt['pps_identifier']
        # self.bits_per_component = user_opt['bits_per_component']
        # self.line_buf_depth = user_opt['line_buf_depth']
        # self.block_pred_enable = user_opt['block_pred_enable']
        # self.convert_rgb = user_opt['convert_rgb']
        # self.simple_422 = user_opt['simple_422']
        # self.native_420 = user_opt['native_420']
        # self.native_422 = user_opt['native_422']
        # self.vbr_enable = user_opt['vbr_enable']
        # self.bits_per_pixel = user_opt['bits_per_pixel']
        # self.pic_height = user_opt["pic_height"]
        # self.pic_width = user_opt["pic_width"]
        # self.slice_height = user_opt["slice_height"]
        # self.slice_width = user_opt["slice_width"]

        if self.bits_per_pixel == (8 << 4):
            if self.bits_per_component == 8:
                table_sel = 0
            elif self.bits_per_component == 10:
                table_sel = 1
            elif self.bits_per_component == 12:
                table_sel = 2
            muxWordSize = 48

        elif self.bits_per_pixel == (12 << 4):
            if self.bits_per_component == 8:
                table_sel = 3
            elif self.bits_per_component == 10:
                table_sel = 4
            elif self.bits_per_component == 12:
                table_sel = 5
            muxWordSize = 64

        chunk_size = int(ceil(self.slice_width * (self.bits_per_pixel >> 4) / 8.0))
        rc_model_size = 8192
        rc_edge_factor = 6
        rc_tgt_offset_hi = 3
        rc_tgt_offset_lo = 3
        initial_xmit_delay = initial_xmit_delay_table[table_sel]
        numSsps = 4 if self.native_422 else 3
        initial_fullness_offset = 6144

        maxSeSize_Y = 4 * self.bits_per_component + 4
        maxSeSize_C = 4 * self.bits_per_component

        if self.convert_rgb:
            numExtraMuxBits = numSsps * (muxWordSize + (4 * self.bits_per_component + 4) - 2)
        elif not self.native_422:
            numExtraMuxBits = numSsps * muxWordSize + maxSeSize_Y + 2 * maxSeSize_C - 2
        else:
            numExtraMuxBits = numSsps * muxWordSize + maxSeSize_Y + 3 * maxSeSize_C - 2

        slicebit = 8 * chunk_size * self.slice_height

        while ((numExtraMuxBits > 0) and ((slicebit - numExtraMuxBits) % muxWordSize)):
            numExtraMuxBits -= 1

        pixelsPerGroup = 3
        groupsPerLine = int(self.slice_width / pixelsPerGroup)
        groupsTotal = groupsPerLine * self.slice_height

        ######## FOR CALCULATED PPSs...

        # initial_dec_delay = []
        # initial_scale_value = []
        # scale_increment_interval = []
        # scale_decrement_interval = []
        first_line_bpg_offset = first_lie_bpg_offset_table[table_sel]
        nfl_bpg_offset = int(ceil(float((first_line_bpg_offset << 11) / (self.slice_height - 1))))
        # slice_bpg_offset = []
        initial_offset = initial_offset_table[table_sel]
        final_offset = rc_model_size - ((initial_xmit_delay * self.bits_per_pixel + 8) >> 4) + numExtraMuxBits
        flatness_min_qp = flatness_min_qp_table[table_sel]
        flatness_max_qp = flatness_max_qp_table[table_sel]
        rc_quant_incr_limit0 = rc_quant_incr_limit_table[table_sel]
        rc_quant_incr_limit1 = rc_quant_incr_limit_table[table_sel]
        rc_buf_thresh = np.array(
            [896, 1792, 2688, 3584, 4480, 5376, 6272, 6720, 7168, 7616, 7744, 7872, 8000, 8064])
        rc_range_parameters = rc_range_parameters_table[table_sel]
        second_line_bpg_offset = self.native_420 * 12
        nsl_bpg_offset = int(ceil(float(second_line_bpg_offset << 11)) / (self.slice_height - 1))
        second_line_offset_adj = 512 * self.native_420

        if (self.native_422 or self.native_420):
            grpcnt = int(ceil(float(initial_xmit_delay) / pixelsPerGroup))
            maxOffset = compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay, self.bits_per_pixel,
                                          first_line_bpg_offset, second_line_bpg_offset, self.slice_bpg_offset,
                                          nfl_bpg_offset,
                                          nsl_bpg_offset, grpcnt, self.native_420)
            maxOffset = max(maxOffset,
                            compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay,
                                              self.bits_per_pixel,
                                              first_line_bpg_offset, second_line_bpg_offset, self.slice_bpg_offset,
                                              nfl_bpg_offset,
                                              nsl_bpg_offset, groupsPerLine, self.native_420))
            maxOffset = max(maxOffset,
                            compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay,
                                              self.bits_per_pixel,
                                              first_line_bpg_offset, second_line_bpg_offset, self.slice_bpg_offset,
                                              nfl_bpg_offset,
                                              nsl_bpg_offset, groupsPerLine * 2, self.native_420))
            rsbMin = rc_model_size - initial_fullness_offset + maxOffset

        else:
            rbsMin = int(rc_model_size - initial_fullness_offset + int(
                ceil(initial_xmit_delay * (self.bits_per_pixel >> 4)))) + groupsPerLine * first_line_bpg_offset

        hrdDelay = int((ceil(float(rbsMin) / (self.bits_per_pixel >> 4))))
        rcb_bits = int((ceil(float(hrdDelay) * (self.bits_per_pixel >> 4))))
        initial_dec_delay = hrdDelay - initial_xmit_delay
        initial_scale_value = int((8 * rc_model_size) / (rc_model_size - initial_offset))
        finalScaleValue = int(8 * rc_model_size / (rc_model_size - final_offset))
        slice_bpg_offset = int(
            ceil(float((1 << 11)) * (rc_model_size - initial_offset + numExtraMuxBits) / (groupsTotal)))

        if finalScaleValue > 9:
            # scale_increment_interval = int(float(1 << 11) * final_offset / (float(finalScaleValue - 9) * nfl_bpg_offset + slice_bpg_offset + nfl_bpg_offset))
            scale_increment_interval = (int)(float((1 << 11)) * final_offset / (
                    float((finalScaleValue - 9)) * (nfl_bpg_offset + slice_bpg_offset + nsl_bpg_offset)))

        else:
            scale_increment_interval = 0

        scale_decrement_interval = int(groupsPerLine / (initial_scale_value - 8))
        # nsl_bpg_offset = first_line_bpg_offset / (slice_height - 1)

        #### Insert ALL PPS to dict
        self.chunk_size = chunk_size
        self.initial_xmit_delay = initial_xmit_delay
        self.initial_dec_delay = initial_dec_delay
        self.initial_scale_value = initial_scale_value
        self.scale_increment_interval = scale_increment_interval
        self.scale_decrement_interval = scale_decrement_interval
        self.first_line_bpg_ofs = first_line_bpg_offset
        self.nfl_bpg_offset = nfl_bpg_offset
        self.slice_bpg_offset = slice_bpg_offset
        self.initlal_offset = initial_offset
        self.final_offset = final_offset
        self.flatness_min_qp = flatness_min_qp
        self.flatness_max_qp = flatness_max_qp
        self.rc_model_size = rc_model_size
        self.rc_edge_factor = rc_edge_factor
        self.rc_quant_icnr_limit0 = rc_quant_incr_limit0
        self.rc_quant_icnr_limit1 = rc_quant_incr_limit1
        self.rc_tgt_offset_hi = rc_tgt_offset_hi
        self.rc_tgt_offset_lo = rc_tgt_offset_lo
        self.rc_buf_thresh = rc_buf_thresh
        self.rc_range_parameters = rc_range_parameters
        self.second_line_bpg_offset = second_line_bpg_offset
        self.nsl_bpg_offset = nsl_bpg_offset
        self.second_line_offset_adj = second_line_offset_adj

        # Below parameters are constants calculated from PPS values
        self.numSsps = numSsps
        self.bitsPerPixel = self.bits_per_pixel >> 4  # Removed Fractional 4-bits, Original Value!
        self.maxSeSize_Y = maxSeSize_Y
        self.maxSeSize_C = maxSeSize_C
        self.numExtraMuxBits = numExtraMuxBits
        self.pixelsPerGroup = pixelsPerGroup
        self.groupsPerLine = groupsPerLine
        self.groupsTotal = groupsTotal
        self.rcb_bits = rcb_bits
        self.hrdDelay = hrdDelay
        self.somewhat_flat_qp_thresh = 7 + (2 * (self.bits_per_component - 8))
        self.somewhat_flat_qp_delta = 4
        self.flatness_det_thresh = 2
        self.muxWordSize = muxWordSize ## 2020.07.27 added


def compute_maxoffset(pixelsPerGroup, groupsPerLine, initial_xmit_delay, bits_per_pixel, first_line_bpg_offset, second_line_bpg_offset, slice_bpg_offset, nfl_bpg_offset, nsl_bpg_offset, groupcount, native_420):
    #grpcnt = int(ceil(float(initial_xmit_delay) / pixelsPerGroup))
    grpcnt = groupcount
    grpcnt_id = int(ceil(float(initial_xmit_delay) / pixelsPerGroup))

    if grpcnt <= grpcnt_id:
        offset = int(ceil(grpcnt * pixelsPerGroup * bits_per_pixel))
    else:
        offset = int(ceil(grpcnt_id * pixelsPerGroup * bits_per_pixel)) - (((grpcnt - grpcnt_id) * slice_bpg_offset) >> 11)

    if grpcnt <= groupsPerLine:
        offset += grpcnt * first_line_bpg_offset
    else:
        offset += groupsPerLine * first_line_bpg_offset - (((grpcnt - groupsPerLine) * nfl_bpg_offset) >> 11)

    if native_420:
        if grpcnt <= grpcnt_id:
            offset -= (grpcnt * nsl_bpg_offset) >> 11
        elif grpcnt <= 2 * groupsPerLine:
            offset += (grpcnt - groupsPerLine) * second_line_bpg_offset - ((groupsPerLine * nsl_bpg_offset) >> 11)
        else:
            offset += (grpcnt - groupsPerLine) * second_line_bpg_offset - (((grpcnt - groupsPerLine) * nsl_bpg_offset) >> 11)

    return offset
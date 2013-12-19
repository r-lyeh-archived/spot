/** this is an amalgamated file. do not edit.
 */

#include "deps/webp/src/webp/decode.h"
#include "deps/webp/src/webp/demux.h"
#include "deps/webp/src/webp/encode.h"
#include "deps/webp/src/webp/format_constants.h"
#include "deps/webp/src/webp/mux.h"
#include "deps/webp/src/webp/mux_types.h"
#include "deps/webp/src/webp/types.h"

#include "deps/webp/src/dec/alphai.h"
#include "deps/webp/src/dec/decode_vp8.h"
#include "deps/webp/src/dec/vp8i.h"
#include "deps/webp/src/dec/vp8li.h"
#include "deps/webp/src/dec/webpi.h"

#include "deps/webp/src/dsp/dsp.h"
#include "deps/webp/src/dsp/lossless.h"
#include "deps/webp/src/dsp/yuv.h"

#include "deps/webp/src/mux/muxi.h"

#include "deps/webp/src/utils/alpha_processing.h"
#include "deps/webp/src/utils/bit_reader.h"
#include "deps/webp/src/utils/bit_writer.h"
#include "deps/webp/src/utils/color_cache.h"
#include "deps/webp/src/utils/filters.h"
#include "deps/webp/src/utils/huffman.h"
#include "deps/webp/src/utils/huffman_encode.h"
#include "deps/webp/src/utils/quant_levels.h"
#include "deps/webp/src/utils/quant_levels_dec.h"
#include "deps/webp/src/utils/random.h"
#include "deps/webp/src/utils/rescaler.h"
#include "deps/webp/src/utils/thread.h"
#include "deps/webp/src/utils/utils.h"

#include "deps/webp/src/dec/frame.c"
#include "deps/webp/src/dec/alpha.c"
#include "deps/webp/src/dec/buffer.c"
#include "deps/webp/src/dec/idec.c"
#include "deps/webp/src/dec/io.c"
#include "deps/webp/src/dec/layer.c"
#include "deps/webp/src/dec/quant.c"
#include "deps/webp/src/dec/tree.c"
#include "deps/webp/src/dec/vp8.c"
#include "deps/webp/src/dec/vp8l.c"
#include "deps/webp/src/dec/webp.c"

#define InitMemBuffer InitMemBuffer_B
#define MemBuffer MemBuffer_B
#define MemDataSize MemDataSize_B
#define ParseVP8X ParseVP8X_B
#define RemapMemBuffer RemapMemBuffer_B
#include "deps/webp/src/demux/demux.c"
#undef InitMemBuffer
#undef MemBuffer
#undef MemDataSize
#undef ParseVP8X
#undef RemapMemBuffer

#include "deps/webp/src/mux/muxedit.c"
#include "deps/webp/src/mux/muxinternal.c"
#include "deps/webp/src/mux/muxread.c"

#include "deps/webp/src/utils/alpha_processing.c"
#include "deps/webp/src/utils/bit_reader.c"
#include "deps/webp/src/utils/bit_writer.c"
#include "deps/webp/src/utils/color_cache.c"
#include "deps/webp/src/utils/filters.c"
#include "deps/webp/src/utils/huffman.c"
#define HuffmanTree HuffmanTree_B
#include "deps/webp/src/utils/huffman_encode.c"
#undef HuffmanTree
#include "deps/webp/src/utils/quant_levels.c"
#include "deps/webp/src/utils/quant_levels_dec.c"
#include "deps/webp/src/utils/random.c"
#include "deps/webp/src/utils/rescaler.c"
#include "deps/webp/src/utils/thread.c"
#include "deps/webp/src/utils/utils.c"

#include "deps/webp/src/dsp/dec.c"
#define TransformOne TransformOne_D1
#define TransformTwo TransformTwo_D1
#define TransformDC TransformDC_D1
#define TransformWHT TransformWHT_D1
#define ITransformOne ITransformOne_D1
#define ITransform ITransform_D1
#define ITransformWHT ITransformWHT_D1
#define FTransform FTransform_D1
#define FTransformWHT FTransformWHT_D1
#define Disto4x4 Disto4x4_D1
#define Disto16x16 Disto16x16_D1
#include "deps/webp/src/dsp/dec_neon.c"
#undef TransformOne 
#undef TransformTwo 
#undef TransformDC 
#undef TransformWHT 
#undef ITransformOne 
#undef ITransform 
#undef ITransformWHT 
#undef FTransform 
#undef FTransformWHT 
#undef Disto4x4 
#undef Disto16x16 
#define TransformOne TransformOne_D2
#define TransformTwo TransformTwo_D2
#define TransformDC TransformDC_D2
#define TransformWHT TransformWHT_D2
#define ITransformOne ITransformOne_D2
#define ITransform ITransform_D2
#define ITransformWHT ITransformWHT_D2
#define FTransform FTransform_D2
#define FTransformWHT FTransformWHT_D2
#define Disto4x4 Disto4x4_D2
#define Disto16x16 Disto16x16_D2
#include "deps/webp/src/dsp/dec_sse2.c"
#undef TransformOne 
#undef TransformTwo 
#undef TransformDC 
#undef TransformWHT 
#undef ITransformOne 
#undef ITransform 
#undef ITransformWHT 
#undef FTransform 
#undef FTransformWHT 
#undef Disto4x4 
#undef Disto16x16 
#include "deps/webp/src/dsp/lossless.c"
#include "deps/webp/src/dsp/upsampling.c"
#include "deps/webp/src/dsp/upsampling_neon.c"
#include "deps/webp/src/dsp/upsampling_sse2.c"
#include "deps/webp/src/dsp/yuv.c"



#define clip_8b clip_8b_B
#define DC4 DC4_B
#define HD4 HD4_B
#define HE4 HE4_B
#define HU4 HU4_B
#define kC1 kC1_B
#define kC2 kC2_B
#define kZigzag kZigzag_B
#define LD4 LD4_B
#define RD4 RD4_B
#define tables_ok tables_ok_B
#define TM4 TM4_B
#define TrueMotion TrueMotion_B
#define VE4 VE4_B
#define VL4 VL4_B
#define VR4 VR4_B
#define VP8SegmentHeader VP8SegmentHeader1
#define VP8Proba VP8Proba1
#define VP8FilterHeader VP8FilterHeader1
#define B_DC_PRED B_DC_PRED1
#define B_TM_PRED B_TM_PRED1
#define B_VE_PRED B_VE_PRED1
#define B_HE_PRED B_HE_PRED1
#define B_RD_PRED B_RD_PRED1
#define B_VR_PRED B_VR_PRED1
#define B_LD_PRED B_LD_PRED1
#define B_VL_PRED B_VL_PRED1
#define B_HD_PRED B_HD_PRED1
#define B_HU_PRED B_HU_PRED1
#define DC_PRED DC_PRED1
#define H_PRED H_PRED1
#define NUM_BMODES NUM_BMODES1
#define V_PRED V_PRED1
#define TM_PRED TM_PRED1
#define NUM_MB_SEGMENTS NUM_MB_SEGMENTS1
#define MAX_NUM_PARTITIONS MAX_NUM_PARTITIONS1
#define NUM_TYPES NUM_TYPES1
#define NUM_BANDS NUM_BANDS1
#define NUM_CTX NUM_CTX1
#define NUM_PROBAS NUM_PROBAS1
#undef BPS
#undef Y_SIZE
#undef YUV_SIZE
#undef Y_OFF
#undef U_OFF
#undef V_OFF
#include "deps/webp/src/enc/backward_references.h"
#include "deps/webp/src/enc/cost.h"
#include "deps/webp/src/enc/histogram.h"
#include "deps/webp/src/enc/vp8enci.h"
#include "deps/webp/src/enc/vp8li.h"
#define TransformOne TransformOne_E1
#define TransformTwo TransformTwo_E1
#define TransformDC TransformDC_E1
#define TransformWHT TransformWHT_E1
#define ITransformOne ITransformOne_E1
#define ITransform ITransform_E1
#define ITransformWHT ITransformWHT_E1
#define FTransform FTransform_E1
#define FTransformWHT FTransformWHT_E1
#define Disto4x4 Disto4x4_E1
#define Disto16x16 Disto16x16_E1
#include "deps/webp/src/dsp/enc.c"
#undef TransformOne 
#undef TransformTwo 
#undef TransformDC 
#undef TransformWHT 
#undef ITransformOne 
#undef ITransform 
#undef ITransformWHT 
#undef FTransform 
#undef FTransformWHT 
#undef Disto4x4 
#undef Disto16x16 
#define TransformOne TransformOne_E2
#define TransformTwo TransformTwo_E2
#define TransformDC TransformDC_E2
#define TransformWHT TransformWHT_E2
#define ITransformOne ITransformOne_E2
#define ITransform ITransform_E2
#define ITransformWHT ITransformWHT_E2
#define FTransform FTransform_E2
#define FTransformWHT FTransformWHT_E2
#define Disto4x4 Disto4x4_E2
#define Disto16x16 Disto16x16_E2
#include "deps/webp/src/dsp/enc_neon.c"
#undef TransformOne 
#undef TransformTwo 
#undef TransformDC 
#undef TransformWHT 
#undef ITransformOne 
#undef ITransform 
#undef ITransformWHT 
#undef FTransform 
#undef FTransformWHT 
#undef Disto4x4 
#undef Disto16x16 
#define TransformOne TransformOne_E3
#define TransformTwo TransformTwo_E3
#define TransformDC TransformDC_E3
#define TransformWHT TransformWHT_E3
#define ITransformOne ITransformOne_E3
#define ITransform ITransform_E3
#define ITransformWHT ITransformWHT_E3
#define FTransform FTransform_E3
#define FTransformWHT FTransformWHT_E3
#define Disto4x4 Disto4x4_E3
#define Disto16x16 Disto16x16_E3
#include "deps/webp/src/dsp/enc_sse2.c"
#undef TransformOne 
#undef TransformTwo 
#undef TransformDC 
#undef TransformWHT 
#undef ITransformOne 
#undef ITransform 
#undef ITransformWHT 
#undef FTransform 
#undef FTransformWHT 
#undef Disto4x4 
#undef Disto16x16 
#include "deps/webp/src/dsp/cpu.c"

/* nope yet */
#define clip clip_C
#define CopyPlane CopyPlane_C
#define do_filter2 do_filter2_C
#define do_filter4 do_filter4_C
#define DoFilter DoFilter_C
#define FilterLoop24 FilterLoop24_C
#define hev hev_C
#define HFilter16i HFilter16i_C
#define HFilter8i HFilter8i_C
#define InitTables InitTables_C
#define needs_filter needs_filter_C
#define needs_filter2 needs_filter2_C
#define ResetSegmentHeader ResetSegmentHeader_C
#define SimpleHFilter16 SimpleHFilter16_C
#define SimpleHFilter16i SimpleHFilter16i_C
#define SimpleVFilter16 SimpleVFilter16_C
#define SimpleVFilter16i SimpleVFilter16i_C
#define VFilter16i VFilter16i_C
#define VFilter8i VFilter8i_C

#include "deps/webp/src/enc/alpha.c"
#include "deps/webp/src/enc/analysis.c"
#undef MAX_ALPHA
#include "deps/webp/src/enc/backward_references.c"
#include "deps/webp/src/enc/config.c"
#include "deps/webp/src/enc/cost.c"
#undef tables_ok
#define tables_ok tables_ok_C
#include "deps/webp/src/enc/filter.c"
#include "deps/webp/src/enc/frame.c"
#include "deps/webp/src/enc/histogram.c"
#include "deps/webp/src/enc/iterator.c"
#include "deps/webp/src/enc/layer.c"
#undef CopyPlane
#define CopyPlane CopyPlane_D
#define GetPSNR GetPSNR_D
#include "deps/webp/src/enc/picture.c"
#undef clip
#undef kZigzag
#undef kDcTable
#undef kAcTable
#undef kBModesProba
#define clip clip_D
#define kZigzag kZigzag_D
#define kDcTable kDcTable_D
#define kAcTable kAcTable_D
#define kBModesProba kBModesProba_D
#include "deps/webp/src/enc/quant.c"
#include "deps/webp/src/enc/syntax.c"
#include "deps/webp/src/enc/token.c"
#include "deps/webp/src/enc/tree.c"
#include "deps/webp/src/enc/vp8l.c"
#undef GetPSNR
#define GetPSNR GetPSNR_E
#include "deps/webp/src/enc/webpenc.c"

#undef VP8SegmentHeader
#undef VP8Proba
#undef VP8FilterHeader
#undef clip_8b
#undef DC4
#undef HD4
#undef HE4
#undef HU4
#undef kC1
#undef kC2
#undef kZigzag
#undef LD4
#undef RD4
#undef tables_ok
#undef TM4
#undef TrueMotion
#undef VE4
#undef VL4
#undef VR4

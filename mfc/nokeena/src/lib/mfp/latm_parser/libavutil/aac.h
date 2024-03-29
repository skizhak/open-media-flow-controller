#ifndef AVCODEC_AAC_H
#define AVCODEC_AAC_H

//#include "avcodec.h"
//#include "dsputil.h"
//#include "fft.h"
#include "mpeg4audio.h"
//#include "sbr.h"
//#include "fmtconvert.h"

#include <stdint.h>

#define MAX_CHANNELS 64
#define MAX_ELEM_ID 16

#define TNS_MAX_ORDER 20
#define MAX_LTP_LONG_SFB 40

enum RawDataBlockType {
    TYPE_SCE,
    TYPE_CPE,
    TYPE_CCE,
    TYPE_LFE,
    TYPE_DSE,
    TYPE_PCE,
    TYPE_FIL,
    TYPE_END,
};

enum ExtensionPayloadID {
    EXT_FILL,
    EXT_FILL_DATA,
    EXT_DATA_ELEMENT,
    EXT_DYNAMIC_RANGE = 0xb,
    EXT_SBR_DATA      = 0xd,
    EXT_SBR_DATA_CRC  = 0xe,
};

enum WindowSequence {
    ONLY_LONG_SEQUENCE,
    LONG_START_SEQUENCE,
    EIGHT_SHORT_SEQUENCE,
    LONG_STOP_SEQUENCE,
};

enum BandType {
    ZERO_BT        = 0,     ///< Scalefactors and spectral data are all zero.
    FIRST_PAIR_BT  = 5,     ///< This and later band types encode two values (rather than four) with one code word.
    ESC_BT         = 11,    ///< Spectral data are coded with an escape sequence.
    NOISE_BT       = 13,    ///< Spectral data are scaled white noise not coded in the bitstream.
    INTENSITY_BT2  = 14,    ///< Scalefactor data are intensity stereo positions.
    INTENSITY_BT   = 15,    ///< Scalefactor data are intensity stereo positions.
};

#define IS_CODEBOOK_UNSIGNED(x) ((x - 1) & 10)

enum ChannelPosition {
    AAC_CHANNEL_OFF   = 0,
    AAC_CHANNEL_FRONT = 1,
    AAC_CHANNEL_SIDE  = 2,
    AAC_CHANNEL_BACK  = 3,
    AAC_CHANNEL_LFE   = 4,
    AAC_CHANNEL_CC    = 5,
};

/**
 * The point during decoding at which channel coupling is applied.
 */
enum CouplingPoint {
    BEFORE_TNS,
    BETWEEN_TNS_AND_IMDCT,
    AFTER_IMDCT = 3,
};

/**
 * Output configuration status
 */
enum OCStatus {
    OC_NONE,        ///< Output unconfigured
    OC_TRIAL_PCE,   ///< Output configuration under trial specified by an inband PCE
    OC_TRIAL_FRAME, ///< Output configuration under trial specified by a frame header
    OC_GLOBAL_HDR,  ///< Output configuration set in a global header but not yet locked
    OC_LOCKED,      ///< Output configuration locked in place
};

/**
 * Predictor State
 */
typedef struct {
    float cor0;
    float cor1;
    float var0;
    float var1;
    float r0;
    float r1;
} PredictorState;

#define MAX_PREDICTORS 672

#define SCALE_DIV_512    36    ///< scalefactor difference that corresponds to scale difference in 512 times
#define SCALE_ONE_POS   140    ///< scalefactor index that corresponds to scale=1.0
#define SCALE_MAX_POS   255    ///< scalefactor index maximum value
#define SCALE_MAX_DIFF   60    ///< maximum scalefactor difference allowed by standard
#define SCALE_DIFF_ZERO  60    ///< codebook index corresponding to zero scalefactor indices difference
#define POW_SF2_ZERO    200    ///< ff_aac_pow2sf_tab index corresponding to pow(2, 0);

/**
 * Long Term Prediction
 */
typedef struct {
    int8_t present;
    int16_t lag;
    float coef;
    int8_t used[MAX_LTP_LONG_SFB];
} LongTermPrediction;

/**
 * Individual Channel Stream
 */
typedef struct {
    uint8_t max_sfb;            ///< number of scalefactor bands per group
    enum WindowSequence window_sequence[2];
    uint8_t use_kb_window[2];   ///< If set, use Kaiser-Bessel window, otherwise use a sinus window.
    int num_window_groups;
    uint8_t group_len[8];
    LongTermPrediction ltp;
    const uint16_t *swb_offset; ///< table of offsets to the lowest spectral coefficient of a scalefactor band, sfb, for a particular window
    const uint8_t *swb_sizes;   ///< table of scalefactor band sizes for a particular window
    int num_swb;                ///< number of scalefactor window bands
    int num_windows;
    int tns_max_bands;
    int predictor_present;
    int predictor_initialized;
    int predictor_reset_group;
    uint8_t prediction_used[41];
} IndividualChannelStream;

/**
 * Temporal Noise Shaping
 */
typedef struct {
    int present;
    int n_filt[8];
    int length[8][4];
    int direction[8][4];
    int order[8][4];
    float coef[8][4][TNS_MAX_ORDER];
} TemporalNoiseShaping;

/**
 * Dynamic Range Control - decoded from the bitstream but not processed further.
 */
typedef struct {
    int pce_instance_tag;                           ///< Indicates with which program the DRC info is associated.
    int dyn_rng_sgn[17];                            ///< DRC sign information; 0 - positive, 1 - negative
    int dyn_rng_ctl[17];                            ///< DRC magnitude information
    int exclude_mask[MAX_CHANNELS];                 ///< Channels to be excluded from DRC processing.
    int band_incr;                                  ///< Number of DRC bands greater than 1 having DRC info.
    int interpolation_scheme;                       ///< Indicates the interpolation scheme used in the SBR QMF domain.
    int band_top[17];                               ///< Indicates the top of the i-th DRC band in units of 4 spectral lines.
    int prog_ref_level;                             /**< A reference level for the long-term program audio level for all
                                                     *   channels combined.
                                                     */
} DynamicRangeControl;

typedef struct {
    int num_pulse;
    int start;
    int pos[4];
    int amp[4];
} Pulse;

/**
 * coupling parameters
 */
typedef struct {
    enum CouplingPoint coupling_point;  ///< The point during decoding at which coupling is applied.
    int num_coupled;       ///< number of target elements
    enum RawDataBlockType type[8];   ///< Type of channel element to be coupled - SCE or CPE.
    int id_select[8];      ///< element id
    int ch_select[8];      /**< [0] shared list of gains; [1] list of gains for right channel;
                            *   [2] list of gains for left channel; [3] lists of gains for both channels
                            */
    float gain[16][120];
} ChannelCoupling;

/**
 * Single Channel Element - used for both SCE and LFE elements.
 */
typedef struct {
    IndividualChannelStream ics;
    TemporalNoiseShaping tns;
    Pulse pulse;
    enum BandType band_type[128];                   ///< band types
    int band_type_run_end[120];                     ///< band type run end points
    float sf[120];                                  ///< scalefactors
    int sf_idx[128];                                ///< scalefactor indices (used by encoder)
    uint8_t zeroes[128];                            ///< band is not coded (used by encoder)
    DECLARE_ALIGNED(32, float,   coeffs)[1024];     ///< coefficients for IMDCT
    DECLARE_ALIGNED(32, float,   saved)[1024];      ///< overlap
    DECLARE_ALIGNED(32, float,   ret)[2048];        ///< PCM output
    DECLARE_ALIGNED(16, float,   ltp_state)[3072];  ///< time signal for LTP
    PredictorState predictor_state[MAX_PREDICTORS];
} SingleChannelElement;

/**
 * channel element - generic struct for SCE/CPE/CCE/LFE
 */
typedef struct {
    // CPE specific
    int common_window;        ///< Set if channels share a common 'IndividualChannelStream' in bitstream.
    int     ms_mode;          ///< Signals mid/side stereo flags coding mode (used by encoder)
    uint8_t ms_mask[128];     ///< Set if mid/side stereo is used for each scalefactor window band
    // shared
    SingleChannelElement ch[2];
    // CCE specific
    ChannelCoupling coup;
    SpectralBandReplication sbr;
} ChannelElement;

/**
 * main AAC context
 */
typedef struct {
    AVCodecContext *avctx;
    AVFrame frame;

    MPEG4AudioConfig m4ac;

    int is_saved;                 ///< Set if elements have stored overlap from previous frame.
    DynamicRangeControl che_drc;

    /**
     * @name Channel element related data
     * @{
     */
    uint8_t layout_map[MAX_ELEM_ID*4][3];
    int layout_map_tags;
    ChannelElement          *che[4][MAX_ELEM_ID];
    ChannelElement  *tag_che_map[4][MAX_ELEM_ID];
    int tags_mapped;
    /** @} */

    /**
     * @name temporary aligned temporary buffers
     * (We do not want to have these on the stack.)
     * @{
     */
    DECLARE_ALIGNED(32, float, buf_mdct)[1024];
    /** @} */

    /**
     * @name Computed / set up during initialization
     * @{
     */
    FFTContext mdct;
    FFTContext mdct_small;
    FFTContext mdct_ltp;
    DSPContext dsp;
    FmtConvertContext fmt_conv;
    int random_state;
    /** @} */

    /**
     * @name Members used for output interleaving
     * @{
     */
    float *output_data[MAX_CHANNELS];                 ///< Points to each element's 'ret' buffer (PCM output).
    /** @} */

    DECLARE_ALIGNED(32, float, temp)[128];

    enum OCStatus output_configured;
    int warned_num_aac_frames;
} AACContext;

#endif /* AVCODEC_AAC_H */

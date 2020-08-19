#if !defined(__TPPM_CFG_H__)
#define __TPPM_CFG_H__

#include <inttypes.h>

#define SUB_MODULES            16
#define BASIC_CHANNELS_COUNT    4
#define EXTRA_CHANNELS_COUNT   12
#define ONOFF_CHANNELS_COUNT   48

#define MAX_DELTA              0

#define GUARD_US               25

#define MIN_CHANNEL_WIDTH_US   976
#define MAX_CHANNEL_WIDTH_US   2000

#define MIN_SYNC_WIDTH_US      2500

#define MIN_PULSE_WIDTH_US     300

#define USEC_TO_WIDTH(us)      (us) // TODO

#define SEC_TO_MS(s)           ((s)*1000)
#define MS_TO_USEC(ms)         ((ms)*1000)

#define ONOFF_CHANNELS_BYTES   (ONOFF_CHANNELS_COUNT >> 3)
#define MIM_CHANNELS           BASIC_CHANNELS_COUNT
#define MAX_CHANNELS           ( BASIC_CHANNELS_COUNT + EXTRA_CHANNELS_COUNT )

#define MAX_PULSE_WIDTH_US     ( MIN_CHANNEL_WIDTH_US - MIN_PULSE_WIDTH_US - 2 * GUARD_US )

// #define MAX_SYNC_WIDTH_US      ( ( MIN_CHANNEL_WIDTH_US * ( MAX_CHANNELS - 4 ) ) + MIN_SYNC_WIDTH_US )
#define MAX_SYNC_WIDTH_US      ( ( ( MAX_CHANNEL_WIDTH_US + GUARD_US ) * MAX_CHANNELS ) + MIN_SYNC_WIDTH_US )

#define MIN_PULSE_WIDTH        USEC_TO_WIDTH( MIN_PULSE_WIDTH_US - GUARD_US )
#define MAX_PULSE_WIDTH        USEC_TO_WIDTH( MAX_PULSE_WIDTH_US + GUARD_US )

#define MIN_GAP_WIDTH          USEC_TO_WIDTH( MIN_CHANNEL_WIDTH_US - MAX_PULSE_WIDTH_US - GUARD_US)
#define MAX_GAP_WIDTH          USEC_TO_WIDTH( MAX_CHANNEL_WIDTH_US - MIN_PULSE_WIDTH_US + GUARD_US)

#define MIN_CHANNEL_WIDTH      USEC_TO_WIDTH( MIN_CHANNEL_WIDTH_US - GUARD_US)
#define MAX_CHANNEL_WIDTH      USEC_TO_WIDTH( MAX_CHANNEL_WIDTH_US + GUARD_US)

#define MIN_SYNC_WIDTH         USEC_TO_WIDTH( MIN_SYNC_WIDTH_US )
#define MAX_SYNC_WIDTH         USEC_TO_WIDTH( MAX_SYNC_WIDTH_US )

#if MAX_DELTA==0
#define ARE_VERY_CLOSE(a,b) ((a)==(b))
#else
#define ARE_VERY_CLOSE(a,b) ((((a)>(b)) ? (a)-(b) : (b)-(a)) <= MAX_DELTA)
#endif

#define IS_IN_RANGE(cval,vmin,vmax) (((cval)>=(vmin)) && ((cval)<=(vmax)))

namespace TPPM
{
    typedef uint16_t[BASIC_CHANNELS_COUNT] BasicChannels;
    typedef uint16_t[EXTRA_CHANNELS_COUNT] ExtraChannels;
    typedef uint8_t [ONOFF_CHANNELS_BYTES] OnOffChannels;
};

#endif // __TPPM_CFG_H__

#if !defined(__PPMSUM_H__)
#define __PPMSUM_H__

#include "Signal.h"

class PPMSum
{
public:
    Signal signals[2]; // pulses and gaps

    enum SignalLevel
    {
        LOW = 0,
        HIGH,
        UNKNOWN
    };

    static SignalLevel pulse_level = UNKNOWN;

    PPMSum(const PPMSum& ref)
    {
        *this = ref;
    }

    PPMSum()
    {
        reset();
    }

    inline PPMSum &operator=(const PPMSum &ref)
    {
        signals[0] = ref.signals[0];
        signals[1] = ref.signals[1];

        return *this;
    }

    inline void reset()
    {
        signals[0].reset();
        signals[1].reset();
    }

    inline void update(const SignalLevel &signal_level, const uint16_t &signal_width)
    {
        signals[signal_level].update(signal_width);
    }

    inline bool operator==(const PPMSum &ref)
    {
        return ( signals[0] == ref.signals[0] )
               &&
               ( signals[1] == ref.signals[1] );
    }

    inline is_valid_sync_frame()
    {
        return (pulse_level != UNKNOWN)
               &&
               (signals[pulse_level].is_valid(MIN_PULSE_WIDTH,MAX_PULSE_WIDTH)
               &&
               (signals[!pulse_level].is_valid(MIN_GAP_WIDTH,MAX_GAP_WIDTH)
               &&
               (signals[!pulse_level].captures == (signals[pulse_level].captures - 1))
               &&
               (signals[0] == signal[0][0])
               &&
               (signals[1] == signal[0][1]);
    }

    inline const Signal &signal(const bool &signal_level)
    {
        return signals[signal_level];
    }
};

#endif // __PPMSUM_H__

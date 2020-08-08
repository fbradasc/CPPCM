#if !defined(__PPMSUM_H__)
#define __PPMSUM_H__

#include "Signature.h"

class PPMSum
{
public:
    PPMSum(const PPMSum& ref)
    {
        *this = ref;
    }

    PPMSum(Signature &signature)
        : _signature(signature)
    {
        reset();
    }

    inline PPMSum &operator=(const PPMSum &ref)
    {
        _signals[0] = ref._signals[0];
        _signals[1] = ref._signals[1];

        return *this;
    }

    inline void reset()
    {
        _signals[0].reset();
        _signals[1].reset();
    }

    inline bool update(const uint8_t &signal_level, const uint16_t &signal_width)
    {
        uint16_t previous_pulse_width = 0;

        if (_flags.pulse_level_set && (signal_level == _flags.pulse_level))
        {
            // Get the last pulse width.
            // To obtain the PPM width it will be added to the current signal width
            //
            previous_pulse_width = _signals[!signal_level].get_last_width();
        }

        _signals[signal_level].update(signal_width+previous_pulse_width);

        if (_flags.pulse_level_set && (signal_level != _flags.pulse_level))
        {
            _signature.update(_signals[signal_level]);
        }

        return IS_IN_RANGE(signal_width, MIN_SYNC_WIDTH, MAX_SYNC_WIDTH);
    }

    inline bool operator==(const PPMSum &ref)
    {
        return ( _signals[0] == ref._signals[0] )
               &&
               ( _signals[1] == ref._signals[1] );
    }

    inline is_valid_sync_frame()
    {
        return _flags.pulse_level_set
               &&
               (_signals[_flags.pulse_level].is_valid(MIN_PULSE_WIDTH,MAX_PULSE_WIDTH)
               &&
               (_signals[!_flags.pulse_level].is_valid(_min_signal_width,_max_signal_width)
               &&
               (_signals[!_flags.pulse_level].captures == (_signals[_flags.pulse_level].captures - 1))
               &&
               (_signals[0] == signal[0][0])
               &&
               (_signals[1] == signal[0][1]);
    }

    static inline void set_pulse_level(const uint8_t &signal_level)
    {
        _flags.pulse_level_set = 1;
        _flags.pulse_level     = signal_level & 1;
        _min_signal_width      = MIN_CHANNEL_WIDTH;
        _max_signal_width      = MAX_CHANNEL_WIDTH;
    }

    static inline void reset_pulse_level()
    {
        _flags.pulse_level_set = 0;
        _flags.pulse_level     = 1;
        _min_signal_width      = MIN_GAP_WIDTH;
        _max_signal_width      = MAX_GAP_WIDTH;
    }

    inline const Signal &signal(const bool &signal_level)
    {
        return _signals[signal_level];
    }

private:
    struct
    {
        pulse_level_set: 1;
        pulse_level    : 1;
    }
    Flags;

    static Flags    _flags           ;
    static uint16_t _min_signal_width;
    static uint16_t _max_signal_width;

    Signal     _signals[2]; // pulses and gaps
    Signature &_signature;
};

#endif // __PPMSUM_H__

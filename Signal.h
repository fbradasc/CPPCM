#if !defined(__SIGNAL_H__)
#define __SIGNAL_H__

#include "cppm.h"

#include <inttypes.h>

#define MAX_DELTA 0

#if MAX_DELTA==0
#define ARE_VERY_CLOSE(a,b) ((a)==(b))
#else
#define ARE_VERY_CLOSE(a,b) ((((a)>(b)) ? (a)-(b) : (b)-(a)) <= MAX_DELTA)
#endif

#define IS_IN_RANGE(v,min,max) (((v)>=(min)) && ((v)<=(max)))

class Signal
{
public:
    uint16_t min_duration;
    uint16_t max_duration;
    uint16_t sum_duration;
    unit16_t durations[MAX_CHANNELS];
    uint8_t  captures;

    Signal()
    {
        reset();
    }

    inline void reset()
    {
        min_duration = 0xffff;
        max_duration = 0;
        sum_duration = 0;
        memset(durations, 0, sizeof(durations));
        captures = 0;
    }

    inline void update(const uint16_t &signal_duration)
    {
        if (signal_duration < MIN_SYNC_WIDTH)
        {
            min_duration = min(min_duration,signal_duration);
            max_duration = max(max_duration,signal_duration);
            sum_duration += signal_duration;

            if (captures < MAX_CHANNELS)
            {
                durations[captures] = signal_duration;
            }

            captures++;
        }
    }

    inline bool operator==(const Signal &ref)
    {
        return ARE_VERY_CLOSE(min_duration,ref.min_duration)
               &&
               ARE_VERY_CLOSE(max_duration,ref.max_duration)
               &&
               ARE_VERY_CLOSE(sum_duration,ref.sum_duration)
               &&
               (captures == ref.captures);
    }

    inline is_valid(const uint16_t &min_value, const uint16_t &max_value)
    {
        return IS_IN_RANGE(captures, MIN_CHANNELS, MAX_CHANNELS)
               &&
               IS_IN_RANGE(min_duration, min_value, max_value)
               &&
               IS_IN_RANGE(max_duration, min_value, max_value)
               &&
               IS_IN_RANGE(sum_duration / captures, min_value, max_value);
    }
};

#endif // __SIGNAL_H__

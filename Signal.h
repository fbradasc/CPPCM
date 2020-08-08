#if !defined(__SIGNAL_H__)
#define __SIGNAL_H__

#include "cppm.h"

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
    uint16_t min_width;
    uint16_t max_width;
    uint16_t sum_width;
    unit16_t widths[MAX_CHANNELS];
    uint8_t  captures;

    Signal(const Signal& ref)
    {
        *this = ref;
    }

    Signal()
    {
        reset();
    }

    inline Signal &operator=(const Signal &ref)
    {
        min_width = ref.min_width;
        max_width = ref.max_width;
        sum_width = ref.sum_width;
        captures  = ref.captures ;

        memcpy(widths, ref.widths, sizeof(widths));

        return *this;
    }

    inline void reset()
    {
        min_width = 0xffff;
        max_width = 0;
        sum_width = 0;
        memset(widths, 0, sizeof(widths));
        captures = 0;
    }

    inline void update(const uint16_t &signal_width)
    {
        if (signal_width < MIN_SYNC_WIDTH)
        {
            min_width = min(min_width,signal_width);
            max_width = max(max_width,signal_width);
            sum_width += signal_width;

            if (captures < MAX_CHANNELS)
            {
                widths[captures] = signal_width;
            }

            captures++;
        }
    }

    inline bool operator==(const Signal &ref)
    {
        return ARE_VERY_CLOSE(min_width,ref.min_width)
               &&
               ARE_VERY_CLOSE(max_width,ref.max_width)
               &&
               ARE_VERY_CLOSE(sum_width,ref.sum_width)
               &&
               (captures == ref.captures);
    }

    inline is_valid(const uint16_t &min_value, const uint16_t &max_value)
    {
        return IS_IN_RANGE(captures, MIN_CHANNELS, MAX_CHANNELS)
               &&
               IS_IN_RANGE(min_width, min_value, max_value)
               &&
               IS_IN_RANGE(max_width, min_value, max_value)
               &&
               IS_IN_RANGE(sum_width / captures, min_value, max_value);
    }

    inline uint16_t get_last_width()
    {
        return (captures > 0) ? widths[captures-1] : 0;
    }
};

#endif // __SIGNAL_H__

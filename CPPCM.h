#ifndef CPPCM_H
#define CPPCM_H

#include <inttypes.h>
#include <avr/interrupt.h>

#define ICP1                         8 // Input capture pin 1
#define CPPCM_MAX_CHANNELS           16
#define CPPCM_SYNC_PULSE_TIME_MIN  3600 // Minimum tolerable value - TDB
#define CPPCM_SYNC_PULSE_TIME_MAX 50000 // Maximum tolerable value - TDB
#define CPPCM_CHAN_PULSE_TIME_MIN  1000 // Minimum tolerable value - TDB
#define CPPCM_CHAN_PULSE_TIME_MAX  1000 // Minimum tolerable value - TDB

ISR(TIMER1_CAPT_vect);

class CPPCMReader
{
public:
    CPPCMReader()
        : _search(true )
        , _synced(false)
        , _state (false)
    {
    }

    void    start   (void);
    void    stop    (void);
    bool    ok      (void);
    uint8_t channels(void);
    void    read    (int16_t *values);

    friend void TIMER1_CAPT_vect();

private:
    uint8_t           _channels;
    volatile bool     _search;
    volatile bool     _synced;
    volatile bool     _state;
    volatile int16_t  _values[CPPCM_MAX_CHANNELS];
    volatile int16_t  _pulses[CPPCM_MAX_CHANNELS];
};

extern CPPCMReader CPPCM;

#endif

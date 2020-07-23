#ifndef CPPCM_H
#define CPPCM_H

#include <inttypes.h>
#include <avr/interrupt.h>

#define CPPCM_MAX_CHANNELS  16

ISR(TIMER1_CAPT_vect);

class CPPCMDsr
{
public:
    CPPCMDsr()
        : _buffer      (0)
        , _search      (true )
        , _synced      (false)
        , _signal_level(false) // PPM with positive shift by default
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
    volatile uint8_t  _buffer;
    volatile bool     _search;
    volatile bool     _synced;
    volatile bool     _signal_level;
    volatile int16_t  _servos[2][CPPCM_MAX_CHANNELS];
    volatile int16_t  _pulses[CPPCM_MAX_CHANNELS];
};

extern CPPCMDsr CPPCM;

#endif

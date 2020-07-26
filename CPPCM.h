#ifndef CPPCM_H
#define CPPCM_H

#include <inttypes.h>
#include <avr/interrupt.h>

#define CPPCM_MAX_CHANNELS 16

// number of consecutive good frames required at startup.
//
#define GOOD_FRAMES_COUNT 10

// number of consecutive bad frames accepted without going to failsafe.
//
#define HOLD_FRAMES_COUNT 25

// number of consecutive low throttle frames required before arming.
//
#define	ARM_FRAMES_COUNT 10

#define FAILSAFE_BUFFER_BIT 1

ISR(TIMER1_CAPT_vect);

class CPPCMDsr
{
public:
    enum Mode
    {
        SYNC_SEARCH,
        CHANNELS_CAPTURE
    };

    CPPCMDsr(uint8_t bits_per_pulses)
        : _good_frames       (GOOD_FRAMES_COUNT)
	    , _hold_frames       (HOLD_FRAMES_COUNT)
        , _arm_frames        (ARM_FRAMES_COUNT )
        , _buffer            (0)
        , _mode              (SYNC_SEARCH)
        , _synced            (false)
        , _got_failsafe_frame(false)
        , _pulse_level       (true) // PPM with positive shift by default
        , _bits_per_pulses   (bits_per_pulses)
    {
    }

    void    start   (void);
    void    stop    (void);
    bool    ok      (void);
    uint8_t channels(void);
    void    read    (int16_t *values);

    friend void TIMER1_CAPT_vect();

private:
    volatile struct Flags
    {
        bool _synced            : 1;
        bool _got_failsafe_frame: 1;
        bool _pulse_level       : 1;
    };

    volatile uint8_t  _good_frames; // Number of good frames to go before
                                    // accepting failsafe frame.
	volatile uint8_t  _hold_frames; // Number of bad frames to go before going
                                    // to failsafe
    volatile uint8_t  _arm_frames;  // Number of low throttle frames to go
                                    // before arming throttle.
    volatile uint8_t  _channels;
    volatile uint8_t  _buffer;
    volatile Mode     _mode;
    volatile int16_t  _servos[2][CPPCM_MAX_CHANNELS];
    volatile int16_t  _safety[CPPCM_MAX_CHANNELS];
    volatile int16_t  _pulses[CPPCM_MAX_CHANNELS];
    volatile uint8_t  _throttle_channel;
    volatile uint8_t  _bits_per_pulses;
};

extern CPPCMDsr CPPCM;

#endif

#include <Arduino.h>
#include <avr/io.h>
#include "CPPCM.h"

#define US_TO_TICKS(us)           (us/1000)
#define ICP1                       8 // Input capture pin 1
#define GUARD_US                  25
#define CHANNEL_WIDTH_MIN_US    1000
#define CHANNEL_WIDTH_MAX_US    2000
#define PULSE_WIDTH_MIN_US       300
#define PULSE_WIDTH_MAX_US      (CHANNEL_WIDTH_MIN_US-PULSE_WIDTH_MIN_US-2*GUARD_US)
#define SYNC_WIDTH_MIN_US       2500

#define PULSE_WIDTH_MIN     US_TO_TICKS(PULSE_WIDTH_MIN_US-GUARD_US)
#define PULSE_WIDTH_MAX     US_TO_TICKS(PULSE_WIDTH_MAX_US+GUARD_US)
#define GAP_WIDTH_MIN       US_TO_TICKS(CHANNEL_WIDTH_MIN_US-PULSE_WIDTH_MAX_US-GUARD_US)
#define GAP_WIDTH_MAX       US_TO_TICKS(CHANNEL_WIDTH_MAX_US-PULSE_WIDTH_MIN_US+GUARD_US)
#define SYNC_WIDTH_MAX      US_TO_TICKS(((CHANNEL_WIDTH_MAX_US+GUARD_US)*CPPCM_MAX_CHANNELS)+SYNC_WIDTH_MIN_US)

// Timer is just running normal free-run mode. The top will be defined as 2^16 -1
//
#define MAX_TICKS 65535

/**
 * Enable the input capture interrupt.
 */
void CPPCMDsr::start(void)
{
    // Define inputs: (interrupt pins)
    //
    pinMode(ICP1, INPUT); // Input capture, pin D8

    // Set internal pull-down resistor. Can be convenient in some cases. Used here to allow one port to be unconnected
    // You can use pull-up as well.
    //
    digitalWrite(ICP1, HIGH);

    // Init all the timer-settings.
    // We will use timer1, as it has  16 bit resolution and some nice features.

    // Input capture is connected to ICP1 (Arduino pin D8)
    // TCNT1 is the counter register
    // ICR1 is used as output/time-mark register. Used for input capture
    // ICF1 = input capture flag

    // Note ICR1 can't be used as top value in the timer, as that will disable input capture.

    // Will go throguh all register settings in timer1. Only very little have to be changed when using input capture.

    // First register. We don't really need to set anything here.
    // page 132
    //
    TCCR1A = (0 << WGM10 ) | // Waveform generation - normal count
             (0 << WGM11 ) | // Waveform generation - normal count
             (0 << COM1A1) | // compare output, not needed
             (0 << COM1A0) | // compare output, not needed
             (0 << COM1B1) | // compare output, not needed
             (0 << COM1B0) ; // compare output, not needed

    // TCCR1B, used to set prescaler for timer and the input-capture settings:
    //
    TCCR1B = (1 << ICNC1) | // Input capture noise canceler - set to active
             (1 << ICES1) | // Input capture edge select. 1 = rising, 0 = falling. We will toggle this, doesn't matter what it starts at
             (0 << CS10 ) | // Prescale 8
             (1 << CS11 ) | // Prescale 8
             (0 << CS12 ) | // Prescale 8
             (0 << WGM13) | // Just normal counter
             (0 << WGM12) ; // Just normal counter

    // Not used in this case:
    //
    TCCR1C = (0 << FOC1A) | // No force output compare (A)
             (0 << FOC1B) ; // No force output compare (B)

    // Enable the interrupts. We only use input capture.
    //
    TIMSK1 = (1 << ICIE1 ) | // Enable input capture interrupt
             (0 << OCIE1B) | // Disable output compare B
             (0 << OCIE1A) | // Disable output compare A
             (0 << TOIE1 ) ; // Disable overflow interrupt
}

/**
 * Disable the input capture interrupt.
 */
void CPPCMDsr::stop(void)
{
    bitClear(TIMSK1, ICIE1);
}

/**
 * Return the number of detected channels
 */
uint8_t CPPCMDsr::channels(void)
{
    return(_channels);
}

/**
 * Return true if a sane CPPCM signal has recently been detected on the input pin.
 */
bool CPPCMDsr::ok(void)
{
    // Need to devise a watchdog; this is never cleared when signal is lost
    //
    return(_synced);
}

/**
 * Atomically read the current CPPCM channel values into the array pointed to by `values`.
 *
 * Each channel value is mapped from a raw pulse width to an int16_t value (nominally 0-255).
 */
void CPPCMDsr::read(int16_t *values)
{
    noInterrupts();

    uint8_t buffer = _buffer; // use failsafe buffer if needed

    for (uint8_t i=0; i < _channels; ++i)
    {
        values[i] = map(_servos[buffer][i],
                          CHN_WIDTH_MIN,
                          CHN_WIDTH_MAX,
                          0, 255);
    }

    interrupts();
}

/**
 * TIMER1_CAPT_vect is invoked by the AVR timer hardware when a pulse appears on the input pin.
 *
 * On entry, the width of the pulse (as measured by timer1) will have been moved into ICR1.
 *
 * As usual, interrupts are disabled inside the handler.
 */
ISR(TIMER1_CAPT_vect)
{
    // Variables used:
    //
    static uint8_t  channels_count    = 0;
    static uint16_t last_capture_time = 0;
    static bool     pulse_expected    = true;

    // TODO: place the following variables in register
    //
    uint16_t capture_time; // current ticks read
    uint16_t signal_width; // current signal width
    uint8_t  signal_level; // signal high or low ?

    // Disable interrupt first, to avoid multiple interrupts causing hanging/restart - or just weird behavior:
    //
    TIMSK1 &= ~(1 << ICIE1);

    signal_level = TCCR1B & (1 << ICES1);

    // Toggle interrupt to detect falling/rising edge:
    //
    TCCR1B ^= (1 << ICES1);

    // Read the timer-value stored in ICR1 register
    //
    // It's the time copied from TCNT1 at input-capture event
    //
    signal_width = capture_time = ICR1;

    // Check if the timer has reached top/started over:
    //
    if (last_capture_time > capture_time)
    {
        signal_width += (MAX_TICKS - last_capture_time);
    }
    else
    {
        // Substract last capture time to get the real signal width:
        //
        signal_width -= last_capture_time;
    }

    // Save this capture time to be used in next interrupt:
    //
    last_capture_time = capture_time;

    if (signal_width > SYNC_WIDTH_MAX)
    {
        // Got a pulse after the maximum allowed SYNC pulse time
        //
        // Maybe we are receiving pulses after a connection lost
        //
        // Exit from channels capture mode and enter in searching mode
        //
        CPPCM._mode = CPPCMDsr::SYNC_SEARCH;

        // Set out of SYNC
        //
        CPPCM._synced = false;

        // Reset channel-count
        //
        channels_count = 0;
    }
    else
    if (signal_width > SYNC_WIDTH_MIN)
    {
        // Got a pulse within the allowed SYNC pulse time range
        //
        if (CPPCMDsr::SYNC_SEARCH == CPPCM._mode)
        {
            // Exit from searching mode and enter in channels capture mode
            //
            CPPCM._mode = CPPCMDsr::CHANNELS_CAPTURE;

            // Save the SYNC pulse signal level
            //
            //        +---------------------------------+
            //        | with positive shift we are here |
            //        +-----------------+---------------+
            //                          .
            //                          v
            //                         .==== [ signal_level=1 ]
            //                        ||
            //                        ||
            //                        ||
            //  == /// ===============' . . . . . . . . . . . .
            //
            //  <- >= SYNC_WIDTH_MIN ->
            //
            //  == /// ===============.
            //                        ||
            //                        ||
            //                        ||
            //   . . . . . . . . . . . `==== [ signal_level=0 ]
            //                          ^
            //                          |
            //        +-----------------+---------------+
            //        | with negative shift we are here |
            //        +---------------------------------+
            //
            CPPCM._pulse_level = signal_level;
        }
        else
        if (CPPCM._pulse_level != signal_level)
        {
            // Got a SYNC pulse with mismatching signal level
            //
            // Invalid pulse
            //
            // TODO
        }
        else
        if (channels_count == 0)
        {
            // Got a SYNC pulse after an empty frame
            //
            // Invalid frame
            //
            // TODO
        }
        else
        if (CPPCM._channels == 0)
        {
            // Got the first SYNC pulse ever
            //
            CPPCM._channels = channels_count;
        }
        else
        if (CPPCM._channels != channels_count)
        {
            // This frame has a mismatching number of channels
            //
            // Invalid frame
            //
            // TODO
        }
        else
        {
            // handle the collected data

            // Reset the failsafe counter
            //
            CPPCM._hold_frames = CPPCMDsr::HOLD_FRAMES;

            if (CPPCM._got_failsafe_frame)
            {
                // We are up and running
                //
                // switch the working buffer
                //
                CPPCM._buffer ^= 0x01;

                CPPCM._synced = true;
            }
            else
            if (CPPCM._good_frames <= 0)
            {
                // save collected data as fail safe
                //
                uint8_t a = CPPCM._buffer;
                uint8_t b = a^0x01;

                for (uint8_t c=0; c<channels_count; c++)
                {
                    CPPCM._servos[b][c] = CPPCM._servos[a][c];
                }

                CPPCM._buffer             = 0;
                CPPCM._got_failsafe_frame = true;
            }
            else
            {
                // TODO: learn transmitter signature and throttle channel

                --CPPCM._good_frames;
            }
        }

        // Reset channel-count
        //
        channels_count = 0;
    }
    else
    if (CPPCMDsr::SYNC_SEARCH == CPPCM._mode)
    {
        // In search mode ignore short pulses until a valid SYNC pulse

        /* nop */ ;
    }
    else
    if (channels_count > CPPCM_MAX_CHANNELS)
    {
        // Max channels count reached, wait for a new valid SYNC pulse

        /* nop */ ;
    }
    else
    if (signal_width < PULSE_WIDTH_MIN)
    {
        // Ignore glitches

        /* nop */ ;
    }
    else
    if (signal_width < PULSE_WIDTH_MAX)
    {
        if (CPPCM._pulse_level != signal_level)
        {
            CPPCM._pulses[channels_count] = signal_width;
        }
        else
        {
            // Ignore glitches

            /* nop */ ;
        }
    }
    else
    if (signal_width < GAP_WIDTH_MIN)
    {
        // Hmmm, zombie zone...
        //
        // TODO
        //
        /* nop */ ;
    }
    else
    if (signal_width < GAP_WIDTH_MAX)
    {
        if (CPPCM._pulse_level == signal_level)
        {
            // The pulse delay is recognised as valid servo pulse
            // Averaging the current reading with the latest good one
            //
            signal_width  += CPPCM._servos[CPPCM._buffer][channels_count];
            signal_width >>= 1;

            CPPCM._servos[CPPCM._buffer^0x01][channels_count] = signal_width;

            ++channels_count;
        }
    }
    else
    {
        // Pulse delay too large ?
        //
        // TODO
        //
        /* nop */ ;
    }

    // Enable interrupt again:
    //
    TIMSK1 |= (1 << ICIE1);
}

// Pre instantiate object
//
CPPCMDsr CPPCM;

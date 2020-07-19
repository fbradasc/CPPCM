#include <Arduino.h>
#include <avr/io.h>
#include "CPPCM.h"

// Timer is just running normal free-run mode. The top will be defined as 2^16 -1
//
#define TOP 65535

/**
 * Enable the input capture interrupt.
 */
void CPPCMReader::start(void)
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
void CPPCMReader::stop(void)
{
    bitClear(TIMSK1, ICIE1);
}

/**
 * Return the number of detected channels
 */
uint8_t CPPCMReader::channels(void)
{
    return(_channels);
}

/**
 * Return true if a sane CPPCM signal has recently been detected on the input pin.
 */
bool CPPCMReader::ok(void)
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
void CPPCMReader::read(int16_t *values)
{
    noInterrupts();

    for (int i=0; i < _channels; ++i)
    {
        values[i] = map(_values[i],
                          CPPCM_CHAN_PULSE_WIDTH_MIN,
                          CPPCM_CHAN_PULSE_WIDTH_MAX,
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
    static uint8_t  channel  = 0;
    static uint16_t last_pulse_time = 0;

    // TODO: place the following variables in register
    //
    uint16_t time_read ; // current time read
    uint16_t pulse_time; //
    uint8_t  ppm_signal; // PPM signal high or low ?

    // Disable interrupt first, to avoid multiple interrupts causing hanging/restart - or just weird behavior:
    //
    TIMSK1 &= ~(1 << ICIE1);

    ppm_signal = TCCR1B & (1 << ICES1);

    // Toggle interrupt to detect falling/rising edge:
    //
    TCCR1B ^= (1 << ICES1);

    // Read the time-value stored in ICR1 register
    //
    // It's the time copied from TCNT1 at input-capture event
    //
    pulse_time = time_read = ICR1;

    // Check if the timer has reached top/started over:
    //
    if (last_pulse_time > pulse_time)
    {
        pulse_time += (TOP - last_pulse_time);
    }
    else
    {
        // Substract last time to get the time:
        //
        pulse_time -= last_pulse_time;
    }

    // Save this time to be used in next interrupt:
    //
    last_pulse_time = time_read;

    if (CPPCM._search)
    {
        // Searching for a SYNC pulse, either the first one or after
        // a corrupted frame or lost connection
        //
        if (pulse_time > CPPCM_SYNC_PULSE_TIME_MAX)
        {
            // Got a pulse after the maximum allowed SYNC pulse time
            //
            // We are not receiving valid PPM frames
            //
            // Set out of SYNC
            //
            CPPCM._synced  = false;

            // Reset the channels count
            //
            channel = 0;
        }
        else
        if (pulse_time > CPPCM_SYNC_PULSE_TIME_MIN)
        {
            // Got a pulse within the allowed SYNC pulse time range
            //
            // Exit from searching mode and enter in channels capture mode
            //
            CPPCM._search = false;

            // Save the SYNC pulse signal level
            //
            // It shall be the same of the PPM data periord level
            //
            CPPCM._state = ppm_signal;

            // Reset the channels count
            //
            channel = 0;
        }

        // In search mode ignore short pulses until a valid SYNC pulse
    }
    else
    {
        // Capturing channels
        //
        if (pulse_time > CPPCM_SYNC_PULSE_TIME_MAX)
        {
            // Got a pulse after the maximum allowed SYNC pulse time
            //
            // Maybe we are receiving pulses after a connection lost
            //
            // Exit from channels capture mode and enter in searching mode
            //
            CPPCM._search = true;

            // Set out of SYNC
            //
            CPPCM._synced     = false;

            // Reset channel-count
            //
            channel = 0;
        }
        else
        if (pulse_time > CPPCM_SYNC_PULSE_TIME_MIN)
        {
            // Got a pulse within the allowed SYNC pulse time range
            //
            if (CPPCM._state != ppm_signal)
            {
                // Got a SYNC pulse with mismatching signal level
                //
                // Invalid pulse
                //
                // TODO
            }
            else
            if (channel == 0)
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
                CPPCM._synced   = true;
                CPPCM._channels = channel;
            }
            else
            if (CPPCM._channels != channel)
            {
                // This frame has a mismatching number of channels
                //
                // Invalid frame
                //
                // TODO
            }

            // Reset channel-count
            //
            channel = 0;
        }
        else
        if (channel > CPPCM_MAX_CHANNELS_COUNT)
        {
            // Too many channels ?
            //
            // TODO
            //
            /* nop */ ;
        }
        else
        if (pulse_time < CPPCM_MARK_PULSE_TIME_MIN)
        {
            // Glitch ?
            //
            // TODO
            //
            /* nop */ ;
        }
        else
        if (pulse_time < CPPCM_PPCM_00_PULSE_TIME)
        {
            CPPCM._pulses[channel] = pulse_time;
        }
        else
        if (pulse_time < CPPCM_PPCM_01_PULSE_TIME)
        {
            CPPCM._pulses[channel] = pulse_time;
        }
        else
        if (pulse_time < CPPCM_PPCM_10_PULSE_TIME)
        {
            CPPCM._pulses[channel] = pulse_time;
        }
        else
        if (pulse_time < CPPCM_PPCM_11_PULSE_TIME)
        {
            CPPCM._pulses[channel] = pulse_time;
        }
        else
        if (pulse_time > CPPCM_CHAN_PULSE_TIME_MAX)
        {
            // Servo pulse too long ?
            //
            // TODO
            //
            /* nop */ ;
        }
        else
        if (pulse_time > CPPCM_CHAN_PULSE_TIME_MIN)
        {
            // The pulse is recognised as valid servo pulse
            //
            CPPCM._values[channel] = pulse_time;

            ++channel;
        }
    }

    // Enable interrupt again:
    //
    TIMSK1 |= (1 << ICIE1);
}

// Pre instantiate object
//
CPPCMReader CPPCM;

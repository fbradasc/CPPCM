#include <Arduino.h>
#include <avr/io.h>

#include "PPSum.h"

#define ICP1       8 // Input capture pin 1

// Timer is just running normal free-run mode. The top will be defined as 2^16 -1
//
#define MAX_TICKS  65535

/**
 * Initialize the user provided output buffers
 *
 * ...and...
 *
 * Enable the input capture interrupt.
 */
void TPPMSum::start(BasicChannels basic_channels,
                    ExtraChannels extra_channels,
                    OnOffChannels onoff_channels,
                    uint16_t      default_servo_value,
                    bool          default_onoff_value)
{
    for (uint8_t c=0; c<BASIC_CHANNELS_COUNT; ++c)
    {
        basic_channels[c] = default_servo_value;
    }

    for (uint8_t c=0; c<EXTRA_CHANNELS_COUNT; ++c)
    {
        extra_channels[c] = default_servo_value;
    }

    for (uint8_t c=0; c<ONOFF_CHANNELS_BYTES; ++c)
    {
        onoff_channels[c] = default_onoff_value;
    }

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
void TPPMSum::stop(void)
{
    bitClear(TIMSK1, ICIE1);
}

/**
 * Atomically read the current CPPCM channel values into the array pointed to by `values`.
 */
void TPPMSum::read(int16_t *values)
{
    noInterrupts();

    uint8_t buffer = _flags.frame_buffer; // use failsafe buffer if needed

    for (uint8_t i=0; i < channels(); ++i)
    {
        values[i] = _channels[buffer][i];
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
    static uint16_t last_capture_time = 0;
    static uint16_t last_pulse_width  = 0;

    uint8_t  signal_level         = PIN_LEVEL;
    uint16_t current_capture_time = TIMER;
    uint16_t signal_width         = current_capture_time;

    if (last_capture_time > current_capture_time)
    {
        signal_width += ( MAX_TIMER_VALUE - last_capture_time );
    }
    else
    {
        signal_width -= last_capture_time;
    }

    last_capture_time = current_capture_time;

    last_pulse_width = ppmsum.process(signal_level    ,
                                      signal_width    ,
                                      last_pulse_width);
}

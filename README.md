CPPCM-DSR
(Combined Pulse Position Coded Modulation - Dgital Signature Recongnition)
Library for Arduino
- PLEASE DO NOT USE - WORK IN PROGRESS - PLEASE DO NOT USE -
=============================================================================

This library provides a simple interface for reading and validating up to 16
channels of RC input from a single CPPCM signal:

    void setup(void)
    {
        CPPCM.begin();
    }

    void loop(void)
    {
        int16_t channels[CPPCM_MAX_CHANNELS];

        if (CPPCM.ok())
        {
            CPPCM.read(channels);

            for (uint8_t c = 0; c < CPPCM.channels(); c++)
            {
                // do something fun with the channels[c] values,
                // like fly a quadcopter...
            }
        }
    }

It analyse the data stream and automatically checks for:

- positive or negative shift
- valid number of pulses (autodetects and stores this)
- valid frame length
- valid pulse widths
- valid channel gap widths
- valid sync gap widths

The term "Code" in the project name means the pulse widths can be used to
superimpose to each frame a digital code of up to 2*(channels+1) bits.

Combined Pulse Position Coded Modulation (CPPCM) Receiver Library for Arduino
=============================================================================

This library provides a simple interface for reading
up to 16 channels of RC input from a single CPPCM signal:

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

In its current form, it most likely only works on Arduino Uno. It requires
exclusive access to timer1, preventing the use of PWM on digital output pins
9 & 10 (whose waveform generator also use timer1).

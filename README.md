TPPMSum - Library for Arduino
=============================================================================
**(Combined Tagged Pulse Position Modulation)**

**-- PLEASE DO NOT USE - WORK IN PROGRESS - PLEASE DO NOT USE --**

This library provides a simple interface to read and validate PPMSum frames of
up to 16 channels (up to 17 pulses), followed by a sync gap.

    #include "TPPMSum.h"

    // Instantiate the TPPMsum decoder
    //
    TPPMSum tppmsum;

    // Allocate the output buffers
    //
    TPPM::BasicChannels basic_channels;
    TPPM::ExtraChannels extra_channels;
    TPPM::OnOffChannels onoff_channels;

    void setup(void)
    {
        // Define here your own 4 bits receiver ID
        // or read it from the flash memory
        //
        const uint8_t rx_id = 0; // [0..15]

        // Initialize the output buffers and start the decoder
        //
        tppmsum.init(rx_id,
                     basic_channels,
                     extra_channels,
                     onoff_channels,
                     512           ,  // default servo value [0..1023]
                     0             ); // default onoff value [0,1]
    }

    void loop(void)
    {
        if (tppmsum.capturing() /* && !tppmsum.initializing() */)
        {
            uint8_t sub_module = tppmsum.read(basic_channels,
                                              extra_channels,
                                              onoff_channels);

            for (uint8_t c = 0; c < tppmsum.total_channels_count(); ++c)
            {
                if (c < tppmsum.basic_channels_count())
                {
                    // Use the basic_channels[c] value to:
                    //
                    // - drive the c-th control/servo
                    //
                }

                if (c < tppmsum.extra_channels_count())
                {
                    // Use the extra_channels[c] value to:
                    //
                    if (tppmsum.entangled())
                    {
                        // - drive the c-th control/servo of the sub_module-th sub module
                        //
                    }
                    else
                    {
                        // - drive the ( c + tppmsum.basic_channels_count() )-th control/servo
                        //
                    }
                }

                if (c < tppmsum.onoff_channels_count())
                {
                    // Use the onoff_channels's c-th bit value to:
                    //
                    // - switch ON/OFF the c-th function of the sub_module-th sub module
                    //
                    if (TPPMSum::is_on(onoff_channels, c))
                    {
                        // switch ON
                    }
                    else
                    {
                        // switch OFF
                    }
                }
            }
        }
    }

The term *Tagged* in the project name means the pulse widths can be used to
superimpose to each frame a digital *tag* of up to 2*(channels+1) bits.

The width of each pulse is 0.3 to 0.46 mS (300 to 460 uS) allowing to superimpose
2 bits each pulse, as for the following table:

    +------------------++-------+-------++-------+
    | Pulse width (us) || bit 1 | bit 0 || Value |
    +------------------++-------+-------++-------+
    | 300 <= x <  340  ||   0   |   0   ||   0   |
    | 340 <= x <  380  ||   0   |   1   ||   1   |
    | 380 <= x <  420  ||   1   |   0   ||   2   |
    | 420 <= x <= 460  ||   1   |   1   ||   3   |
    +------------------++-------+-------++-------+

The width of each channel is 0.976 to 2 mS (1.488 mS when sticks centered).

The sync gap is at least 2.5 mS long.

An N channels frame length is approximately (2*N+2.5) mS.

The input signal should look something like this for a **positive** *shift*:

               |<--------------- 1 frame at least ~(2*N+2.5) mS --------------->|
               |< 0.976~2mS >|    0.3~0.46mS -->| |<--     |<  at least 2.5mS  >|
                _             _                  _          _                    _
               | |           | |                | |        | |                  | |    
     __________| |___________| |_________ // ___| |________| |__________________| |____
      sync gap      ch1           ch2    etc.        chN          sync gap          ch1

Or like this for a **negative** *shift*:

               |<--------------- 1 frame at least ~(2*N+2.5) mS --------------->|
               |< 0.976~2mS >|    0.3~0.46mS -->| |<--     |<  at least 2.5mS  >|
     __________. .___________. ._________ // ___. .________. .__________________. .____
               | |           | |                | |        | |                  | |
               |_|           |_|                |_|        |_|                  |_|
      sync gap      ch1           ch2    etc.        chN          sync gap          ch1

## FRAME VALIDATION

The validation is done by evaluating the following parameters:

- positive or negative shift
- number of pulses
- valid frame length
- valid pulse widths
- valid channel gap widths
- valid sync gap widths

If any of the above parameters is invalid, the whole frame is discarded.

The above parameters are also used to evaluate the transmitter signature.

A *valid* frame that does not match the detected transmitter signature is
discarded as well.

When a frame is discarded the last good frame is used instead.

## FAIL SAFE

On receiving a sufficient number of good frames we save it for fail safe. 
Then, if the signal is corrupted for too long (more than 25 frames), we output
the fail safe frame instead of the last good frame.  

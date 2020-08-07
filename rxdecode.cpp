//***************************************************************************
//   rxdecode.asm      Pulse Position Modulation Decoder for 4 Channel Rx
//***************************************************************************
//            Bruce Abbott   bhabbott@paradise.net.nz
//
//        for Microchip PIC 12C508, 12C509, 12F629 or 12F675.
//
//============================= Description =================================
//
// PPM DECODER:
//
// Each frame consists of 4 to 8 channels (5 to 9 pulses), followed by a sync
// gap. We only decode the first 4 channels.
//
// The width of each channel is 1 to 2mS (1.5mS when sticks centered). The
// sync gap is at least 2.5mS long, and frame length is approximately 20mS.
//
// The input signal should look something like this:-
//
//            |<------------------ 1 frame (~20mS) -------------------->|
//            |< 1~2mS >|
//             _            _        _        _        _                    _
// ___________| |_______| |______| |______| |______| |__ // ____________| |____
//   sync gap     ch1       ch2      ch3      ch4       etc.  sync gap      ch1
//
// NOTE: This waveform shows positive 'shift'. If your Rx outputs negative
//       'shift', the waveform is inverted.
//
// There is just enough time to generate four decoded outputs after receiving
// channel 4, and before the end of the sync gap.
//
// GLITCH FILTER:
//
// Each channel is averaged with the value from the previous frame, reducing
// servo jitter on weak signals. If any channel in a frame is corrupted, the
// whole frame is discarded and the last good frame is used instead.
//
// FAILSAFE:
//
// On receiving a sufficient number of good frames we save it for failsafe.
// Then, if the signal is corrupted for too long, we output the failsafe frame
// instead of the last good frame.
//
// THROTTLE ARMING:
//
// When entering failsafe the throttle is cut, and it will not be restarted
// until a good signal is detected AND the throttle is manually reset. This
// should prevent the situation where motor-induced RF interference causes
// the decoder to cycle in and out of failsafe repeatedly.
//
// For this feature to work the throttle channel has to be determined. Futaba
// and Hitec tranmitters assign the throttle to channel 3. JR, Airtronics and
// GWS use channel 1.
//
// Some ESC's need to have the throttle set to maximum at startup, perhaps to
// disable the brake or to enter programming mode. Therefore, channel 1 is
// designated as throttle if it measures less than 1.3mS or more than 1.7mS at
// startup, otherwise channel 3 is assumed.
//
// The throttle channel is forced to 1.05mS during failsafe, rather than set
// to its failsafe value, in case that value was full throttle!
//
// =============================================================================
//                             Summary of Changes
//
// 2003/12/2  V0.5 - Initial release
//
// 2003/12/27 V0.6 - ASM Error if 'addwf PCL' not in page 0.
//                 - Select processor via MPLab's Device Select menu.
// 2004/1/7   V0.7 - No output until failsafe frame captured. Should now be
//                   compatible with JR/GWS transmitters (throttle on CH1).
//
// 2004/1/9   V0.8 - Increased range to 0.75~2.28mS, now accepts >125% throws.
//                   Resolution is slightly reduced (from 5uS to 6uS).
//                 - Failsafe and Hold frames now use averaged output values.
//                 - Ignore missing 4th channel (for 3 channel TX) NOT TESTED!
//
// 2004/2/15  V0.9 - Fixed bug:- OSCCAL not initialized in 12F675 code!
//                 - Detect throttle channel. The throttle is held OFF after
//                   failsafe, until re-armed manually. This prevents cyling
//                   into and out of failsafe due to electrical noise coming
//                   from the motor.
//
// 2004/6/20  V1.0 - Using TMR0 to detect loss of signal. This prevents lockup
//                   during a clean signal loss (no noise) which may occur if
//                   used in a receiver which has muting.
//                 - TMR0 is also used to set frame repeat time in failsafe.
//        - Fixed bug:- Initial channel output values were wrong.
//
// 2004/10/24 V1.1 - Positive and Negative shift versions.
//        - Ignore short glitches on signal transitions.
//        - Ensure throttle is low in failsafe, even if it was maxiumum
//                   at startup (possible requirement for ESC programming).
//
// 2004/12/12 V1.2 - Fixed bug: JR throttle arming was disabled due to a typo!
//
// 2005/1/20  V1.3 - Now accepts pulse widths from 180uS to 700uS.
//
// -----------------------------------------------------------------------------

#include <inttypes.h>

#define version  "1.3"

#define ARM_THROTTLE    // enable if throttle arming control wanted.
#define DETECT_JR   // enable for JR/Airtronics/GWS throttle detection.

//#define Negative_Shift  ; enable for Futaba/Hitec on 72MHz


// Make sure that PROCESSOR and <include> file are compatible with your CPU!
// 12C508(A) and 12C509(A) can use 12C508 definitions. 12F629 can use 12F675
// definitions (just don't try to use the A/D module...)

//#define __12C508  ; enable if processor not specified elsewhere
//#define __12F675  ; (MPLAB:- use menu <Configure/Select_Device>)

//#define NO_OSCCAL     ; enable if OSCCAL value was erased!

#define MAX_CHANNELS 4

// Bit definitions for the GPIO register and the TRIS register

#define CH_1     0    // pin 7   Channel 1 output
#define CH_2     1    // pin 6   Channel 2 output
#define CH_3     2    // pin 5   Channel 3 output
#define PPM_in   3    // pin 4   input pulse stream
#define CH_4     4    // pin 3   Channel 4 Output
#define LED      5    // pin 2   Signal Indicator LED

#define TrisBits 0xff & ~((1<<CH_1)|(1<<CH_2)|(1<<CH_3)|(1<<CH_4)|(1<<LED))


// Bits to be set with the OPTION instruction
//   No wake up
//   No weak pullups
//   Timer 0 source internal
//   Prescaler to Timer 0, divide by 256.
//
#define OptionBits 11000111b

// =========================================================================
// Macro for generating short time delays
//
#define WAIT_A_LITTLE_BIT(count) {uint8_t c=count; while (c > 1) { c -= 2; };}

// ==========================================================================
//                 RAM Variable Definitions
//
uint8_t    Flags            ;      // various boolean flags
uint8_t    PPMcount         ;      // pulse length. 1~255 = 0.75~2.28mS
uint8_t    PPM[MAX_CHANNELS];      // channels in
uint8_t    PWM[MAX_CHANNELS];      // channels out
uint8_t    PMM[MAX_CHANNELS];      // channels memory
uint8_t    FLS[MAX_CHANNELS];      // channels failsafe
uint8_t    GoodFrames       ;      // No. of good frames to go before
                                   // accepting failsafe frame.
uint8_t    HoldFrames       ;      // No. of bad frames to go before going
                                   // to failsafe
uint8_t    ArmFrames        ;      // No. of low throttle frames to go
                                   // before arming throttle.
uint8_t    Temp1            ;
uint8_t    Temp2            ;



// flag values
//
#define WATCH   0       // Watchdog timeout
#define GOT_FS  1       // have captured failsafe frame
#define GOT_4   2       // 4 channels found in current frame
#define DET_4   3       // 4 channel TX detected
#define JR      4       // JR throttle detected
#define ARMED   5       // throttle armed

// number of consecutive good frames required at startup.

#define GOODCOUNT 10

// number of consecutive bad frames accepted without going to failsafe.

#define HOLDCOUNT 25

// number of consecutive low throttle frames required before arming.

#define ARMCOUNT  10

//----------------------------------------------------------------------------
// GetPPM:             Get time to next PPM pulse
//----------------------------------------------------------------------------
//
// input:   PPM signal has just gone high.
// output:  PPMcount = Pulse Width * 6uS, next pulse has started
// error:   PPMcount = XX and error code in W.
//
// Error Codes
//      0 = good channel
//      1 = pulse too short, too long, or next pulse too soon
//      2 = no next pulse (ie. no channel)

#define PRECHARGE ((750-15)/6)        // = 0.75mS

uint8_t GetPPM()
{
    // preset count for signal high length
    //
    PPMcount = PRECHARGE;

    for (Temp1 = (30/6); Temp1>0; Temp1--)
    {
        // wait 30uS to skip short glitches
        //
        WAIT_A_LITTLE_BIT(2)

        PPMcount--;
    }

    for (;PPMcount>0; PPMcount--)
    {
        if (!signal_pulse())
        {
            // signal gone low ?
            //
            if (PPMcount < (PRECHARGE-(180/6)))
            {
                // less than minimum pulse width ?
                //
                return 1;
            }
            if (PPMcount > (PRECHARGE-(700/6)))
            {
                // greater than maximum pulse width ?
                //
                return 1;
            }

            for (Temp1 = (30/6); Temp1>0; Temp1--)
            {
                // wait 30uS to skip short glitches
                //
                WAIT_A_LITTLE_BIT(2)

                PPMcount--;
            }

            // count down to zero @ 0.75mS
            //
            for (; PPMcount>0; PPMcount--)
            {
                if (signal_pulse())
                {
                    // signal should stay low until 0.75mS
                    //
                    return 1;
                }
                // nop         // 6uS per loop
            }

            PPMcount++;    // count up, start at 1

            for (; PPMcount<255; PPMcount++)
            {
                if (signal_pulse())
                {
                    // signal should stay low until 0.75mS
                    //
                    return 0;
                }
                // nop         // 6uS per loop
            }

            // return timeout error @ 2.28mS
            //
            return 2;
        }
    }

    // timed out, signal high
    //
    return 1;
}



//------------------------------------------------------------------------------
//                   Output PPM widths to channels 1-4
//------------------------------------------------------------------------------
void Output()
{
    for (uint8_t ch=0; ch<MAX_CHANNELS; ch++)
    {
        set_pin(pins[ch], 1);

        // PWM[ch]=delay count, 1 to 255 = 0.75 to 2.28mS
        //
        // wait 0~1.53mS, total = 0.75~2.28mS
        //
        for (Temp2 = 0; Temp2 < PWM[ch]; Temp2++)
        {
            // wait 750uS
            //
            for (Temp1 = 0; Temp1 < ((750-6)/6); Temp1++)
            {
                clrwdt();
            }

            clrwdt();
        }

        set_pin(pins[ch], 0);
    }
}


//*******************************************************************************
//                 Main
//*******************************************************************************


void main()
{
    if (Flags & (1 << WATCH))
    {
        Failsafe();
    }

    Flags = 0;

    // wait 500mS for Rx to stabilise
    //
    wait_ms(500);

    set_pin(LED,0); // signal LED off

    uint8_t default_val = (1500-750)/6;

    for (uint8_t ch=0; ch<MAX_CHANNELS; ch++)
    {
        PMM[ch] = default_val;
    }

    PPM[3] = default_val; // init channel 4 (for 3 channel TX)

    GoodFrames  = GOODCOUNT;   // set number of good frames required
    HoldFrames  = HOLDCOUNT;   // set number of bad frames allowed
                               // before going to failsafe.
    ArmFrames   = ARMCOUNT ;   // set number of low throttle frames
                               // before arming throttle

    no_signal();
}


void Failsafe()
{
    HoldFrames = 1;  // stay in failsafe until signal returns

    if (!(Flags & (1<<GOT_FS)))
    {
        no_signal;
    }

    for (uint8_t ch=0; ch<MAX_CHANNELS; ch++)
    {
        PWM[ch] = FLS[ch];
    }

    Output();      // output failsafe frame

    bcf Flags &= ~ARMED; // keep throttle OFF

    ArmFrames = ARMCOUNT; // reset throttle arming delay
}

void no_signal()
{
    set_pin(LED,0);    // signal LED off

    wait_sync();
}

void wait_sync()
{
    clrwdt();          // we're still sane, no reset please!

    Temp1 = 0;
    Temp2 = 9;

    wait_gap();
}

void wait_gap()
{
    while (Temp2>0)
    {
        if (!signal_pulse())
        {
            PPMcount = 0;

            do
            {
                Temp1--;

                if (Temp1 == 0)
                {
                    Temp2--;

                    if (Temp2 == 0) // timed out ?
                    {
                        badframe(); // can't find sync gap! } 10uS per loop
                    }
                }

                if (signal_pulse()) // still in gap ?
                {
                    wait_gap();
                }

                PPMcount++;

                TMR0 = 128-(23000/256); // set frame timeout to 23mS

                do
                {
                    clrwdt();

                    if (TMR0 & (1<<7)) // timer reached 23mS ?
                    {
                        badframe();
                    }
                }
                while (!signal_pulse());

                Flags &= ~GOT_4; // channel 4 not received yet

                WAIT_A_LITTLE_BIT(3);

                uint8_t error;

                error = GetPPM();

                if (error)
                {
                    badframe();
                }

                PPM[0] = PPMcount;

                error = GetPPM();

                if (error)
                {
                    badframe();
                }

                PPM[1] = PPMcount;

                error = GetPPM();

                if (error)
                {
                    badframe();
                }

                PPM[2] = PPMcount;

                error = GetPPM();

                if (error == 2)
                {
                    if (!(Flags & (1 << GOT_FS)))
                    {
                        // no failsafe frame got yet, ignore missing channel 4
                        //
                        goto update();
                    }
                    if (!(Flags & (1 << DET_4)))
                    {
                        // 4 channels not detected, ignore missing channel 4
                        //
                        goto update();
                    }
                }
                else
                if (error)
                {
                    badframe();
                }

                PPM[3] = PPMcount;

                Flags |= GOT_4;

                update();
            }
            while (PPMcount < 255); // gap > 2.56mS ?
        }

        clrwdt();

        Temp1--;

        if (Temp1==0)
        {
            Temp2--;
        }
    }

    badframe();    // can't find sync gap!
}


void badframe()
{
    // wait 9mS to skip other channels
    //
    do
    {
        clrwdt();
    }
    while (TMR0 < (128-((23000-9000)/256)));

    ArmFrames = ARMCOUNT;    // reset throttle arming delay

    HoldFrames--;

    if (HoldFrames > 0)
    {
        if (Flags & (1<<GOT_FS))
        {
            Output(); // good frame available for hold -> output last good frame
        }
        else
        {
            no_signal(); // no good frame available for hold -> just stay silent
        }
    }
    else
    {
        // too many bad frames ?
        //
        Failsafe();
    }
}

//
// Got a good frame. Output the averaged pulse widths of this frame
// and the last frame.
//
void update()
{
    for (uint8_t ch=0; ch<MAX_CHANNELS; ch++)
    {
        PMM[ch] += PWM[ch];
        PWM[ch] = PMM[ch] >> 1; // PWM out = average(this+last)
    }

    if (!(Flags & (1<<GOT_FS)))
    {
        // failsafe captured -> output
        //
        output_done();
    }

#ifdef  ARM_THROTTLE
    if (Flags & (1<<ARMED))
    {
        // throttle armed -> output
        //
        Output();
    }

    uint8_t thr_chan = 3;

    if (Flags & (1 << JR))
    {
        thr_chan = 0;
    }

    if (PWM[thr_chan] < (1300-750)/6)
    {
        // throttle < 1.3mS ?
        //
        ArmFrames--;

        if (ArmFrames>0)
        {
            Flags |= (1<<ARMED);

            Output();
        }
    }
    else
    {
        // no, reset arming delay
        //
        ArmFrames = ARMCOUNT;
    }

    // set throttle to failsafe value
    //
    PWM[thr_chan] = FLS[thr_chan];
#endif   // ARM_THROTTLE

    // output good frame
    //
    Output();

output_done:
    // remember this frame
    //
    for (uint8_t ch=0; ch<MAX_CHANNELS; ch++)
    {
        PMM[ch] = PPM[ch];
    }

    // reset failsafe timeout
    //
    HoldFrames = HOLDCOUNT;

    // signal LED off
    //
    set_pin(LED,0);

    if (!(Flags & (1<<GOT_FS)))
    {
        clrwdt();

        GoodFrames--;

        if (GoodFrames<=0)
        {
            // Got enough good frames, now get failsafe values
            //
            if (Flags & (1<<GOT_4))
            {
                // channel 4 detected
                //
                Flags |= (1<<DET_4);
            }

            for (uint8_t ch=0; ch<MAX_CHANNELS; ch++)
            {
                // copy good output to failsafe
                //
                FLS[ch] = PWM[ch];
            }

            // failsafe frame captured
            //
            Flags |= (1<<GOT_FS);

#ifdef   ARM_THROTTLE
#ifdef   DETECT_JR
            if (FLS[0] < ((1300-750)/6))
            {
                // channel 1 < 1.3mS
                //
                Flags |= (1<<JR);
            }
            else
            if (FLS[0] < ((1700-750)/6))
            {
                // channel 1 >= 1.7mS ?
                //
                if (FLS[2] >= ((1700-750)/6))
                {
                    // failsafe low throttle!
                    //
                    FLS[2] = (1100-750)/6;
                }
            }
            else
            {
                // failsafe low throttle!
                //
                FLS[0] = (1100-750)/6;
            }
#endif   // DETECT_JR

            Flags |= (1<<ARMED);
#endif   // ARM_THROTTLE
        }
    }

    wait_sync();
}

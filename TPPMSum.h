#if !defined(__TPPM_SUM_H__)
#define __TPPM_SUM_H__

#include "TPPMTag.h"

// Number of consecutive good frames required at startup.
//
#define GOOD_FRAMES_COUNT 10

// Number of consecutive bad frames accepted without going to failsafe.
//
#define HOLD_FRAMES_COUNT 25

// Number of consecutive bad frames accepted without re-init the encoder.
//
#define INIT_FRAMES_COUNT 50

ISR(TIMER1_CAPT_vect);

class TPPMSum
{
public:
    TPPMSum(const PPMSum& ref)
    {
        *this = ref;
    }

    TPPMSum()
        : _state(INIT_DECODE)
    {
        _flags.fail_safe_mode   = 0;
        _flags.signature_buffer = 0;
        _flags.frame_buffer     = ALT1_DATA_BUFFER;
        _flags.pulse_level_set  = 0;
        _flags.pulse_level      = HI_LEVEL;
        _flags.entangled        = 0;
        _min_signal_width       = MIN_GAP_WIDTH;
        _max_signal_width       = MAX_GAP_WIDTH;

        _tag.reset();

        _dsr[SIGNATURE_REF_DATA][LO_LEVEL].reset();
        _dsr[SIGNATURE_REF_DATA][HI_LEVEL].reset();

        _dsr[SIGNATURE_CUR_DATA][LO_LEVEL].reset();
        _dsr[SIGNATURE_CUR_DATA][HI_LEVEL].reset();
    }

    // Initialize the user provided channels arrays and start the PPMSum decoder
    //
    void init(TPPM::BasicChannels basic_channels     ,
              TPPM::ExtraChannels extra_channels     ,
              TPPM::OnOffChannels onoff_channels     ,
              uint16_t            default_servo_value,
              bool                default_onoff_value);

    // Stop the PPMSum decoder
    //
    void stop(void);

    // Retrieve the decoded channels data and returns the current controlled sub-module
    //
    uint8_t read(TPPM::BasicChannels basic_channels,
                 TPPM::ExtraChannels extra_channels,
                 TPPM::OnOffChannels onoff_channels);

    // Returns true if the decoder is capturing whole frames, either usable or not
    //
    inline bool capturing(void)
    {
        return _state > SYNC_SEARCH;
    }

    // Returns true if the decoder is NOT yet capturing usable frames
    //
    inline bool initializing(void)
    {
        return _state < PPM_CAPTURE;
    }

    inline uint8_t total_channels_count(void)
    {
        return max(BASIC_CHANNELS_COUNT,
                   max(extra_channels(),
                       onoff_channels()));
    }

    // Returns the number of basic channels
    //
    // Currently the basic channels are the first 4 in the frame
    //
    inline uint8_t basic_channels_count(void)
    {
        return BASIC_CHANNELS_COUNT;
    }

    // If the frame does not have a digital tag, it returns:
    //
    // - the number of raw channels captured in the frame except the basic ones
    //
    // If the frame has a digital tag, it returns:
    //
    // - the number of the supported multiplexed channels which are encoded
    //   in the 3 channels which, in the frame, follow the basic ones
    //
    inline uint8_t extra_channels_count(void)
    {
        if (_flags.entangled)
        {
            return EXTRA_CHANNELS_COUNT;
        }

        return _dsr[_flags.signature_buffer].captures - BASIC_CHANNELS_COUNT;
    }

    // If the frame does not have a digital tag, it returns 0
    //
    // If the frame has a digital tag, it returns:
    //
    // - the number of the supported ON/OFF switches which are encoded
    //   in the last 3 channels in the frame
    //
    inline uint8_t onoff_channels_count(void)
    {
        if (_flags.entangled)
        {
            return ONOFF_CHANNELS_COUNT;
        }

        return 0;
    }

    //------------------------------------//
    //                                    //
    // Superimposed Coding data accessors //
    //                                    //
    //------------------------------------//

    // returns true if the transmitter transmits superimposed tag
    //
    inline bool entangled(void) { return _flags.entangled; }

private:
    friend void TIMER1_CAPT_vect();

    enum
    {
        ALT1_DATA_BUFFER = 0,
        ALT2_DATA_BUFFER    ,
        FAIL_SAFE_BUFFER    ,
        FRAME_BUFFERS
    }
    Buffer;

    enum
    {
        LO_LEVEL = 0,
        HI_LEVEL    ,
        SIGNAL_LEVELS
    }
    SignalLevel;

    enum
    {
        INIT_DECODE,
        SYNC_SEARCH,
        ACKNOWLEDGE,
        PPM_CAPTURE
    }
    Status;

    enum
    {
        SIGNATURE_REF_DATA = 0,
        SIGNATURE_CUR_DATA    ,
        SIGNATURE_BUFFERS
    }
    SignatureBuffer;

    class Signature
    {
        uint16_t min_width;
        uint16_t max_width;
        uint16_t sum_width;
        uint16_t captures;

        Signature()
            : min_width(0)
            , max_width(0)
            , sum_width(0)
            , captures(0)
        {}

        inline void reset()
        {
            min_width = 0xffff;
            max_width = 0;
            sum_width = 0;
            captures  = 0;
        }

        inline void update(const uint16_t &width)
        {
            min_width  = min(min_width, width);
            max_width  = max(max_width, width);
            sum_width += width;

            ++captures;
        }

        inline bool operator==(const Signal &ref)
        {
            return ARE_VERY_CLOSE(min_width, ref.min_width)
                   &&
                   ARE_VERY_CLOSE(max_width, ref.max_width)
                   &&
                   ARE_VERY_CLOSE(sum_width, ref.sum_width)
                   &&
                   (captures == ref.captures);
        }

        inline is_valid(const uint16_t &min_value, const uint16_t &max_value)
        {
            return IS_IN_RANGE(captures, MIN_CHANNELS, MAX_CHANNELS)
                   &&
                   IS_IN_RANGE(min_width, min_value, max_value)
                   &&
                   IS_IN_RANGE(max_width, min_value, max_value)
                   &&
                   IS_IN_RANGE(sum_width / captures, min_value, max_value);
        }
    };

    struct
    {
        uint8_t pulse_level_set : 1;
        uint8_t pulse_level     : 1;
        uint8_t fail_safe_set   : 1;
        uint8_t frame_buffer    : 1;
        uint8_t signature_buffer: 1;
        uint8_t entangled       : 1;
        uint8_t fail_safe_mode  : 1;
    }
    Flags;

    Status        _state           ;
    Flags         _flags           ;
    uint16_t      _min_signal_width;
    uint16_t      _max_signal_width;

    Signature     _dsr[SIGNATURE_BUFFERS][SIGNAL_LEVELS];     // pulses and gaps

    uint16_t      _raw_channels[FRAME_BUFFERS][MAX_CHANNELS]; // 0: alt frame buffer
                                                              // 1: alt frame buffer
                                                              // 2: fail safe buffer
    TPPM::ExtraChannels _extra_channels;
    TPPM::OnOffChannels _onoff_channels;
    TPPMTag             _tag;
    uint8_t             _good_frames;
    uint8_t             _hold_frames;

    inline uint16_t process(uint8_t  signal_level,
                            uint16_t signal_width,
                            uint16_t pulse_width )
    {
        uint16_t channel_width = signal_width + pulse_width;
        uint8_t &channel       = _dsr[_flags.signature_buffer][signal_level].captures;

        if (signal_width < MIN_SYNC_WIDTH)
        {
            // It's a pulse or a channel gap,
            // update the Digital Signature data for the Recognition checks
            //
            if (_flags.pulse_level_set)
            {
                if (signal_level == _flags.pulse_level)
                {
                    // The current signal is a gap so it's a channel
                    //
                    if (channel < MAX_CHANNELS)
                    {
                        // Save the channel width in the current frame buffer
                        //
                        _raw_channels[_flags.frame_buffer][channel] = channel_width;
                    }
                }
            }

            // Update the Digital Signature data
            //
            _dsr[_flags.signature_buffer][signal_level].update(channel_width);
        }

        if (_flags.pulse_level_set)
        {
            if (signal_level == _flags.pulse_level)
            {
                // The current signal is a gap so the next signal will be a pulse.
                //
                // Clear the saved pulse width.
                //
                pulse_width = 0;
            }
            else
            {
                // Save the pulse width to be added to the next signal width (gap) to
                // to obtain the PPM channel width
                //
                pulse_width = signal_width;

                _tag.update(channel, signal_width);
            }
        }

        bool sync_detected = IS_IN_RANGE(signal_width, MIN_SYNC_WIDTH, MAX_SYNC_WIDTH);

        if (INIT_DECODE == _state)
        {
            _flags.fail_safe_mode  = 1;
            _flags.pulse_level_set = 0;
            _flags.pulse_level     = HI_LEVEL;
            _min_signal_width      = MIN_GAP_WIDTH;
            _max_signal_width      = MAX_GAP_WIDTH;
            _good_frames           = 0;
            _hold_frames           = 0;
            _state                 = SYNC_SEARCH;
        }

        if (SYNC_SEARCH == _state)
        {
            // Search for a valid sync signal discarding all the others
            //
            if (sync_detected)
            {
                _tag.reset();

                _dsr[SIGNATURE_REF_DATA][LO_LEVEL].reset();
                _dsr[SIGNATURE_REF_DATA][HI_LEVEL].reset();

                _dsr[SIGNATURE_CUR_DATA][LO_LEVEL].reset();
                _dsr[SIGNATURE_CUR_DATA][HI_LEVEL].reset();

                _flags.signature_buffer = SIGNATURE_REF_DATA;
                _flags.pulse_level      = signal_level & HI_LEVEL;
                _flags.pulse_level_set  = 1;
                _flags.entangled        = 0;
                _min_signal_width       = MIN_CHANNEL_WIDTH;
                _max_signal_width       = MAX_CHANNEL_WIDTH;
                _state                  = ACKNOWLEDGE;
            }
        }
        else
        if (ACKNOWLEDGE == _state)
        {
            // Capture a number of consecutive matching frames in order to identify the transmitter
            //
            if (sync_detected)
            {
                // A sync signal has been detected, let's analyze the frame in order to find if
                // it matches the reference one
                // (or set it as the reference if it's good and it's the first one)
                //
                if (_flags.pulse_level_set
                    &&
                    (_dsr[_flags.signature_buffer][ _flags.pulse_level].is_valid(MIN_PULSE_WIDTH  , MAX_PULSE_WIDTH  )
                    &&
                    (_dsr[_flags.signature_buffer][!_flags.pulse_level].is_valid(_min_signal_width, _max_signal_width)
                    &&
                    (_dsr[_flags.signature_buffer][!_flags.pulse_level].captures == (_dsr[_flags.signature_buffer][_flags.pulse_level].captures - 1))
                    &&
                    (_dsr[_flags.signature_buffer][LO_LEVEL] == _dsr[SIGNATURE_REF_DATA][LO_LEVEL])
                    &&
                    (_dsr[_flags.signature_buffer][HI_LEVEL] == _dsr[SIGNATURE_REF_DATA][HI_LEVEL])
                    &&
                    (!_tag.is_encoded() || _tag.is_valid()))
                {
                    // A good frame has been captured, we need to collect a few of them
                    // in order to be sure we are entangled to the transmitter
                    //
                    ++_good_frames;

                    if (SIGNATURE_REF_DATA == _flags.signature_buffer)
                    {
                        // This is the first good frame since decoder initialization
                        // Keep its data as reference for further frames checks
                        //
                        _tag.entangle();

                        _flags.signature_buffer = SIGNATURE_CUR_DATA;

                        _flags.entangled = _tag.is_encoded();
                    }

                    _dsr[_flags.signature_buffer][LO_LEVEL].reset();
                    _dsr[_flags.signature_buffer][HI_LEVEL].reset();

                    if (GOOD_FRAMES_COUNT <= _good_frames)
                    {
                        // A number of consecutive good frames has been captured
                        //
                        if (!_flags.fail_safe_set)
                        {
                            // Fail safe channels values have not yet been saved
                            // save them now
                            //
                            for (uint8_t ch=0; ch < MAX_CHANNELS; ch++)
                            {
                                _raw_channels[FAIL_SAFE_BUFFER][ch] = _raw_channels[_flags.frame_buffer][ch];
                            }

                            _flags.fail_safe_set = true;
                        }

                        // We can now collect frames for real use
                        //
                        _state = PPM_CAPTURE;
                    }

                    _flags.fail_safe_mode = 0;
                }
                else
                {
                    // A mismatching frame has been captured, re-init the decoder
                    //
                    _state = INIT_DECODE;
                }
            }
        }
        else
        if (PPM_CAPTURE == _state)
        {
            if (sync_detected)
            {
                if (_flags.pulse_level_set)
                {
                    if (_dsr[_flags.signature_buffer][ _flags.pulse_level].is_valid(MIN_PULSE_WIDTH  , MAX_PULSE_WIDTH  )
                        &&
                        _dsr[_flags.signature_buffer][!_flags.pulse_level].is_valid(_min_signal_width, _max_signal_width)
                        &&
                        (_dsr[_flags.signature_buffer][!_flags.pulse_level].captures == (_dsr[_flags.signature_buffer][_flags.pulse_level].captures - 1)))
                    {
                        // It's a good frame
                        //
                        if (!_flags.entangled || (_tag.is_encoded() && _tag.is_valid()))
                        {
                            // And it's for me - reset the hold frames counter
                            //
                            _hold_frames = 0;

                            // Keep in sync the alternative frame buffer with the current one
                            //
                            uint8_t old_frame_buffer =  _flags.frame_buffer;
                            uint8_t new_frame_buffer = !_flags.frame_buffer;

                            for (uint8_t ch=0; ch < MAX_CHANNELS; ch++)
                            {
                                _raw_channels[new_frame_buffer][ch] = _raw_channels[old_frame_buffer][ch];
                            }

                            if (_flags.entangled)
                            {
                                // I'm entangled
                                //
                                // Let's decode the extra channels according to the superimposed tag
                                //
                                _tag.decode(_raw_channels[new_frame_buffer],
                                            _extra_channels,
                                            _onoff_channels);
                            }

                            // Switch the active frame buffer
                            //
                            _flags.frame_buffer = new_frame_buffer;

                            // Let's go out from fail safe mode and let's use the good frames
                            //
                            _flags.fail_safe_mode = 0;
                        }
                        else
                        if (!_tag.is_encoded())
                        {
                            // I'm entangled but I received a good NOT tagged PPM frame
                            //
                            // Increase the hold frames counter
                            //
                            ++_hold_frames;
                        }
                        else
                        {
                            // I'm entangled and I received a good tagged PPM frame which it's not for me.
                            //
                            // Freeze with the last good frame, maybe the transmitter is controlling another receiver.
                            //
                            // Reset the hold frames counter
                            //
                            _hold_frames = 0;

                            // Let's go out from fail safe mode and let's use the last good frame
                            //
                            _flags.fail_safe_mode = 0;
                        }
                    }
                    else
                    {
                        // It's a bad frame
                        //
                        // Increase the hold frames counter
                        //
                        ++_hold_frames;
                    }

                    if (HOLD_FRAMES_COUNT <= _hold_frames)
                    {
                        // The failures count to enter in fail safe mode has been reached
                        //
                        _flags.fail_safe_mode = _flags.fail_safe_set;

                        // Avoid overflows due to too many increments
                        //
                        _hold_frames = HOLD_FRAMES_COUNT;
                    }
                }
                else
                {
                    // Hmmm, a pulse level was not detected, anyway we entered in the PPM_CAPTURE mode...
                    //
                    // ...this should never happens...
                    //
                    // ...let's re-initialize the decoder
                    //
                    _state = INIT_DECODE;
                }
            }
        }

        return pulse_width;
    }
};

#endif // __TPPM_SUM_H__

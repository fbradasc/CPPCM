#if !defined(__PPMSUM_H__)
#define __PPMSUM_H__

#include "CPPCM.h"
#include "PPMTag.h"

ISR(TIMER1_CAPT_vect);

class PPMSum
{
public:
    PPMSum(const PPMSum& ref)
    {
        *this = ref;
    }

    PPMSum(uint8_t rx_id)
        : _state(INIT_DECODE)
        , _tag  (rx_id)
    {
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

    void    start     (void);
    void    stop      (void);
    void    read      (int16_t *values);

    inline bool    ok          (void) { return _state > SYNC_SEARCH; }
    inline bool    initializing(void) { return _state < PPM_CAPTURE; }
    inline uint8_t channels    (void) { return _dsr[_flags.frame_buffer].captures; }

    //------------------------------------//
    //                                    //
    // Superinposed Coding data accessors //
    //                                    //
    //------------------------------------//

    // returns true if the trasmitter transmits superinposed code
    //
    inline bool    entangled (void) { return _flags.entangled; }

    // returns the trasmitter ID in the superinposed code
    //
    inline uint8_t transmiter(void) { return _tag.tx_id(); }

    // returns the scan field in the superinposed code
    //
    inline uint8_t scan_index(void) { return _tag.scan(); }

    // returns the submodule ID field in the superinposed code
    //
    inline uint8_t sub_module(void) { return _tag.rx_sub_id(); }

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
    }
    Flags;

    Status    _state           ;
    Flags     _flags           ;
    uint16_t  _min_signal_width;
    uint16_t  _max_signal_width;

    Signature _dsr[SIGNATURE_BUFFERS][SIGNAL_LEVELS]; // pulses and gaps

    uint16_t  _channels[FRAME_BUFFERS][MAX_CHANNELS]; // 0: alt frame buffer
                                                      // 1: alt frame buffer
                                                      // 2: fail safe buffer
    PPMTag    _tag;
    uint8_t   _good_frames;
    uint8_t   _hold_frames;

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
                        _channels[_flags.frame_buffer][channel] = channel_width;
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
            // Capture a number of consecutive matching frames in order to identify the transmiter
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
                    // in order to be sure we are entangled to the transmiter
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
                                _channels[FAIL_SAFE_BUFFER][ch] = _channels[_flags.frame_buffer][ch];
                            }

                            _flags.fail_safe_set = true;
                        }

                        // We can now collect frames for real use
                        //
                        _state = PPM_CAPTURE;
                    }
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
                        if (_flags.entangled && !_tag.is_encoded())
                        {
                            // Bad frame - increase the bad frames counter
                            //
                            ++_hold_frames;
                        }
                        else
                        if (!_flags.entangled || _tag.is_valid())
                        {
                            // Good frame - reset the bad frames counter
                            //
                            _hold_frames = 0;

                            uint8_t old_frame_buffer =  _flags.frame_buffer;
                            uint8_t new_frame_buffer = !_flags.frame_buffer;

                            for (uint8_t ch=0; ch < MAX_CHANNELS; ch++)
                            {
                                _channels[new_frame_buffer][ch] = _channels[old_frame_buffer][ch];
                            }

                            // Switch the active frame buffer
                            //
                            _flags.frame_buffer = new_frame_buffer;
                        }
                        // Good frame but not for me, stay with the current frame buffer
                        //
                    }
                    else
                    {
                        // Bad frame - increase the bad frames counter
                        //
                        ++_hold_frames;
                    }
                }

                if (!_flags.pulse_level_set || (HOLD_FRAMES_COUNT <= _hold_frames))
                {
                    // No pulse_level detected or max failures count reached
                    //
                    // Re-init the decoder
                    //
                    _state = INIT_DECODE;
                }
            }
        }

        return pulse_width;
    }
};

#endif // __PPMSUM_H__

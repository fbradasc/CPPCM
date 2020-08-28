#if !defined(__TPPM_TAG_H__)
#define __TPPM_TAG_H__
//
// The tag superimposition is available and decoded only on a 8 channels
// frame to allow a frame length of 20ms.
//
//            +---------------+--------------+
//            |   width (ms)  |     span     |
//            | min   |  max  |  (ms) | (us) |
// +---------++=======+=======+=======+======+
// | channel || 0.976 | 2.000 | 1.024 | 1024 |
// | pulse   || 0.300 | 0.460 | 0.160 |  160 |
// | gap     || 0.516 | 1.700 | 1.184 | 1184 |
// +---------++-------+-------+-------+------+
//
// Digital nibble channels encoding on 4 bits:
//
// +--------------------++-------+-------+-------+-------+
// | Channel width (us) ||       |       |       |       |
// |    step: 64 us     || bit 3 | bit 2 | bit 1 | bit 0 |
// +====================++=======+=======+=======+=======+
// |  976 <= x <  1040  ||   0   |   0   |   0   |   0   |
// | 1040 <= x <  1104  ||   0   |   0   |   0   |   1   |
// | 1104 <= x <  1168  ||   0   |   0   |   1   |   0   |
// | 1168 <= x <  1232  ||   0   |   0   |   1   |   1   |
// | 1232 <= x <  1296  ||   0   |   1   |   0   |   0   |
// | 1296 <= x <  1360  ||   0   |   1   |   0   |   1   |
// | 1360 <= x <  1424  ||   0   |   1   |   1   |   0   |
// | 1424 <= x <  1488  ||   0   |   1   |   1   |   1   |
// | 1488 <= x <  1552  ||   1   |   0   |   0   |   0   |
// | 1552 <= x <  1616  ||   1   |   0   |   0   |   1   |
// | 1616 <= x <  1680  ||   1   |   0   |   1   |   0   |
// | 1680 <= x <  1744  ||   1   |   0   |   1   |   1   |
// | 1744 <= x <  1808  ||   1   |   1   |   0   |   0   |
// | 1808 <= x <  1872  ||   1   |   1   |   0   |   1   |
// | 1872 <= x <  1936  ||   1   |   1   |   1   |   0   |
// | 1936 <= x <= 2000  ||   1   |   1   |   1   |   1   |
// +--------------------++-------+-------+-------+-------+
//
#include "TPPMCfg.hpp"

// The tag superimposition is available and decoded only on a 8 channels
// frame to allow a frame length of 20ms.
//
// An 8 channles frames contain 9 pulses:
//
//     |<ch.1>|<ch.2>|<ch.3>|<ch.4>|<ch.5>|<ch.6>|<ch.7>|<ch.8>|<  sync  >|
//      _      _      _      _      _      _      _      _      _          _
//     | |    | |    | |    | |    | |    | |    | |    | |    | |        | |
// ..._| |____| |____| |____| |____| |____| |____| |____| |____| |_......_| |__
//
//      ^      ^      ^      ^      ^      ^      ^      ^      ^          ^
//    pulse1 pulse2 pulse3 pulse4 pulse4 pulse6 pulse7 pulse8 pulse9 ... pulse1
//
// The number of pulses over which the tag is superimposed
//
#define MAX_SUPERIMPOSED_PULSES   9

// Minimum valid pulse width (MIN_PULSE_WIDTH)
//
#define MIN_TAG_THRESHOLD         USEC_TO_WIDTH( 300 )

// Maximum valid pulse width (MAX_PULSE_WIDTH)
//
#define MAX_TAG_THRESHOLD         USEC_TO_WIDTH( 460 )

// Each pulse can encode 2 bits (4 different values)
//
// +---------------------------------------------++-------+-------+
// | Pulse width                                 ||       |       |
// |    step: TAG_THRESHOLD_STEP                 || bit 1 | bit 0 |
// +=============================================++=======+=======+
// | MIN_TAG_THRESHOLD <= x <  TAG_THRESHOLD_00  ||   0   |   0   |
// | TAG_THRESHOLD_00  <= x <  TAG_THRESHOLD_01  ||   0   |   1   |
// | TAG_THRESHOLD_01  <= x <  TAG_THRESHOLD_10  ||   1   |   0   |
// | TAG_THRESHOLD_10  <= x <= MAX_TAG_THRESHOLD ||   1   |   1   |
// +---------------------------------------------++-------+-------+
//
#define TAG_THRESHOLD_STEP        ((MAX_TAG_THRESHOLD-MIN_TAG_THRESHOLD)/4)

#define TAG_THRESHOLD_00          (MIN_TAG_THRESHOLD + TAG_THRESHOLD_STEP)
#define TAG_THRESHOLD_01          (TAG_THRESHOLD_00  + TAG_THRESHOLD_STEP)
#define TAG_THRESHOLD_10          (TAG_THRESHOLD_01  + TAG_THRESHOLD_STEP)

// Utility macro to get a bit value in a word
//
#define BIT_VAL(word,bit_pos)     (((word) >> (bit_pos)) & 0x01)

// Initialization value for the coupled encoder ID
//
#define UNDEFINED_ENCODER         0xff

// The pulses of a normal PPMSum frame do not have the tag superimposition
// thus they shall have all the same witdh.
//
// The tag detection algorithm converts each pulse width in a bit pair
// then pack all the pairs in the lower 18 LSB bits in a 32 bits word.
//
// The resulting word for a normal PPMSum frame can be one of the following:
//
#define INVALID_PATTERN_00        0b00000000000000000000000000000000
#define INVALID_PATTERN_01        0b00000000000000010101010101010101
#define INVALID_PATTERN_10        0b00000000000000101010101010101010
#define INVALID_PATTERN_11        0b00000000000000111111111111111111

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
// The following macros are used to configure the decoder() method used to  //
// demultiplex the multiplexed channels and retrieve the data they contain. //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

// Channels distribution:
//
// +-----++-------------------------------------------------------+----------+
// |     ||                         scan                          | Update   |
// | CH. ||  0   |  1   |  2   |  3   |  4   |  5   |  6   |  7   | Rate     |
// +=====++======+======+======+======+======+======+======+======+==========+
// |  1  ||                          P.1                          | 50.00 Hz |
// |  2  ||                          P.2                          | 50.00 Hz |
// |  3  ||                          P.3                          | 50.00 Hz |
// |  4  ||                          P.4                          | 50.00 Hz |
// +-----++------+------+------+------+------+------+------+------+----------+
// |  5  || P.5  | P.6  | P.7  | P.8  | P.5  | P.6  | P.7  | P.8  | 12.50 Hz |
// |  6  || P.9  | P.10 | P.11 | P.12 | P.9  | P.10 | P.11 | P.12 | 12.50 Hz |
// |  7  || P.13 | P.14 | P.15 | P.16 | P.13 | P.14 | P.15 | P.16 | 12.50 Hz |
// +-----++------+------+------+------+------+------+------+------+----------+
// |  8  || D.1  | D.2  | D.3  | D.4  | D.5  | D.6  | D.7  | D.8  |  6.25 Hz |
// +-----++------+------+------+------+------+------+------+------+----------+
// | P: Proportional                                                         |
// | D: Digital nibble - 4 bits; 16 levels                                   |
// +-------------------------------------------------------------------------+

// Number of multiplexed proportional channels (ch.5, ch.6 and ch.7)
//
#define MULTI_PROP_CHANNELS       3

// Position (0 based) of the first multiplexed proportional channel (ch.5)
//
#define MULTI_PROP_CHANNEL_POS    4

// Position (0 based) of the multiplexed digital channel in the frame (ch.8)
//
#define MULTI_BITS_CHANNEL_POS    7

// The multiplexed digital channel can encode 4 bits (16 different values)
//
// +--------------------------------------------------++---------------+
// | Channel width                                    ||      bit      |
// |    step: ONOFF_THRESHOLD_STEP                    || 3 | 2 | 1 | 0 |
// +==================================================++===+===+===+===+
// | MIN_CHANNEL_WIDTH    <= x < ONOFF_THRESHOLD_0000 || 0 | 0 | 0 | 0 |
// | ONOFF_THRESHOLD_0000 <= x < ONOFF_THRESHOLD_0001 || 0 | 0 | 0 | 1 |
// | ONOFF_THRESHOLD_0001 <= x < ONOFF_THRESHOLD_0010 || 0 | 0 | 1 | 0 |
// | ONOFF_THRESHOLD_0010 <= x < ONOFF_THRESHOLD_0011 || 0 | 0 | 1 | 1 |
// | ONOFF_THRESHOLD_0011 <= x < ONOFF_THRESHOLD_0100 || 0 | 1 | 0 | 0 |
// | ONOFF_THRESHOLD_0100 <= x < ONOFF_THRESHOLD_0101 || 0 | 1 | 0 | 1 |
// | ONOFF_THRESHOLD_0101 <= x < ONOFF_THRESHOLD_0110 || 0 | 1 | 1 | 0 |
// | ONOFF_THRESHOLD_0110 <= x < ONOFF_THRESHOLD_0111 || 0 | 1 | 1 | 1 |
// | ONOFF_THRESHOLD_0111 <= x < ONOFF_THRESHOLD_1000 || 1 | 0 | 0 | 0 |
// | ONOFF_THRESHOLD_1000 <= x < ONOFF_THRESHOLD_1001 || 1 | 0 | 0 | 1 |
// | ONOFF_THRESHOLD_1001 <= x < ONOFF_THRESHOLD_1010 || 1 | 0 | 1 | 0 |
// | ONOFF_THRESHOLD_1010 <= x < ONOFF_THRESHOLD_1011 || 1 | 0 | 1 | 1 |
// | ONOFF_THRESHOLD_1011 <= x < ONOFF_THRESHOLD_1100 || 1 | 1 | 0 | 0 |
// | ONOFF_THRESHOLD_1100 <= x < ONOFF_THRESHOLD_1101 || 1 | 1 | 0 | 1 |
// | ONOFF_THRESHOLD_1101 <= x < ONOFF_THRESHOLD_1110 || 1 | 1 | 1 | 0 |
// | ONOFF_THRESHOLD_1110 <= x < MAX_CHANNEL_WIDTH    || 1 | 1 | 1 | 1 |
// +--------------------------------------------------++---+---+---+---+
//
#define ONOFF_THRESHOLD_STEP      ((MAX_CHANNEL_WIDTH-MIN_CHANNEL_WIDTH)/16)

#define ONOFF_THRESHOLD_0000      (MIN_CHANNEL_WIDTH    + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_0001      (ONOFF_THRESHOLD_0000 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_0010      (ONOFF_THRESHOLD_0001 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_0011      (ONOFF_THRESHOLD_0010 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_0100      (ONOFF_THRESHOLD_0011 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_0101      (ONOFF_THRESHOLD_0100 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_0110      (ONOFF_THRESHOLD_0101 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_0111      (ONOFF_THRESHOLD_0110 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_1000      (ONOFF_THRESHOLD_0111 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_1001      (ONOFF_THRESHOLD_1000 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_1010      (ONOFF_THRESHOLD_1001 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_1011      (ONOFF_THRESHOLD_1010 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_1100      (ONOFF_THRESHOLD_1011 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_1101      (ONOFF_THRESHOLD_1100 + ONOFF_THRESHOLD_STEP)
#define ONOFF_THRESHOLD_1110      (ONOFF_THRESHOLD_1101 + ONOFF_THRESHOLD_STEP)

class TPPMTag
{
public:
    TPPMTag()
    {}

    inline void reset()
    {
        _pulse_min_width = 0xffff;
        _pulse_max_width = 0;
        _tag.raw_bits    = 0;

        _data.reset();
    }

    inline void set_decoder_id(const uint8_t &decoder_id)
    {
        _data.decoder_id = decoder_id & 0x0f;
    }

    inline void update(const uint8_t &captures, const uint16_t &pulse_width)
    {
        _data.reset(); // to check if it shall be reset at each pulse

        _pulse_min_width  = min(_pulse_min_width, pulse_width);
        _pulse_max_width  = max(_pulse_max_width, pulse_width);

        if (captures <= MAX_SUPERIMPOSED_PULSES)
        {
            // collect data
            //
            if (IS_IN_RANGE(pulse_width,MIN_TAG_THRESHOLD,MAX_TAG_THRESHOLD))
            {
                // It's a valid pulse with superinposed code
                //
                uint8_t bit_index = (captures << 1);

                // clear the bits
                //
                _tag.raw_bits &= ~(0x11 << bit_index);

                // set the bits
                //
                if (pulse_width >= TAG_THRESHOLD_10)
                {
                    _tag.raw_bits |= (0x11 << bit_index);
                }
                else
                if (pulse_width >= TAG_THRESHOLD_01)
                {
                    _tag.raw_bits |= (0x10 << bit_index);
                }
                else
                if (pulse_width >= TAG_THRESHOLD_00)
                {
                    _tag.raw_bits |= (0x01 << bit_index);
                }
            }
            else
            {
                // invalid data -> invalidate the tag
                //
                _tag.raw_bits = 0;
            }
        }
        else
        if (captures == MAX_SUPERIMPOSED_PULSES)
        {
            _tag.raw_bits &= INVALID_PATTERN_11; // mask off all the extra bits

            // We have reached the maximum number of coded pulses
            //
            _data.encoded = !ARE_VERY_CLOSE(_pulse_min_width, _pulse_max_width)
                            &&
                            ( _tag.raw_bits != INVALID_PATTERN_00 )
                            &&
                            ( _tag.raw_bits != INVALID_PATTERN_01 )
                            &&
                            ( _tag.raw_bits != INVALID_PATTERN_10 )
                            &&
                            ( _tag.raw_bits != INVALID_PATTERN_11 );

            if (_data.encoded)
            {
                // To ensure an unencoded PPM frame is not validated we check
                // for odd parity on the encoder_id bits, decoder_id bits
                // and scan bits.
                //
                // In an unencoded PPM all the pulses shall have the same width
                // thus the frame will fall in one of the following cases:
                //
                // _raw_bits              : 000000000000000000
                // _encoder_id+_quick_bttn: 0000
                // _decoder_id+_part_index:     00000000
                // _scan_index            :             000
                // _encoder_odd_parity    :                0   -> !match
                // _decoder_odd_parity    :                 0  -> !match
                // _scan_odd_parity       :                  0 -> !match
                // ------------------------------------------------------
                // _raw_bits              : 010101010101010101
                // _encoder_id+_quick_bttn: 0101
                // _decoder_id+_part_index:     01010101
                // _scan_index            :             010
                // _encoder_odd_parity    :                1   ->  match
                // _decoder_odd_parity    :                 0  -> !match
                // _scan_odd_parity       :                  1 -> !match
                // ------------------------------------------------------
                // _raw_bits              : 101010101010101010
                // _encoder_id+_quick_bttn: 1010
                // _decoder_id+_part_index:     10101010
                // _scan_index            :             101
                // _encoder_odd_parity    :                0   -> !match
                // _decoder_odd_parity    :                 1  ->  match
                // _scan_odd_parity       :                  0 -> !match
                // ------------------------------------------------------
                // _raw_bits              : 111111111111111111
                // _encoder_id+_quick_bttn: 1111
                // _decoder_id+_part_index:     11111111
                // _scan_index            :             111
                // _encoder_odd_parity    :                1   ->  match
                // _decoder_odd_parity    :                 1  ->  match
                // _scan_odd_parity       :                  1 -> !match
                //
                _data.trusted = (BIT_VAL(_tag.raw_bits, ENCODER_ID_BIT_0     )^
                                 BIT_VAL(_tag.raw_bits, ENCODER_ID_BIT_1     )^
                                 BIT_VAL(_tag.raw_bits, QUICK_BTTN_BIT_0     )^
                                 BIT_VAL(_tag.raw_bits, QUICK_BTTN_BIT_1     )^
                                 BIT_VAL(_tag.raw_bits, ENCODER_ID_PARITY_BIT))
                                &&
                                (BIT_VAL(_tag.raw_bits, DECODER_ID_BIT_0     )^
                                 BIT_VAL(_tag.raw_bits, DECODER_ID_BIT_1     )^
                                 BIT_VAL(_tag.raw_bits, DECODER_ID_BIT_2     )^
                                 BIT_VAL(_tag.raw_bits, DECODER_ID_BIT_3     )^
                                 BIT_VAL(_tag.raw_bits, PART_INDEX_BIT_0     )^
                                 BIT_VAL(_tag.raw_bits, PART_INDEX_BIT_1     )^
                                 BIT_VAL(_tag.raw_bits, PART_INDEX_BIT_2     )^
                                 BIT_VAL(_tag.raw_bits, PART_INDEX_BIT_3     )^
                                 BIT_VAL(_tag.raw_bits, DECODER_ID_PARITY_BIT))
                                &&
                                (BIT_VAL(_tag.raw_bits, SCAN_INDEX_BIT_0     )^
                                 BIT_VAL(_tag.raw_bits, SCAN_INDEX_BIT_1     )^
                                 BIT_VAL(_tag.raw_bits, SCAN_INDEX_BIT_2     )^
                                 BIT_VAL(_tag.raw_bits, SCAN_INDEX_PARITY_BIT))
                                &&
                                // It's trusted only if it comes from the
                                // coupled encoder, if any
                                //
                                ((_data.coupled_id == UNDEFINED_ENCODER)
                                 ||
                                 (_tag.encoder_id == _data.coupled_id));

                // It's valid only if it is trusted and it is for me
                //
                _data.valid = _data.trusted
                              &&
                              ( _data.decoder_id == _tag.decoder_id );
            }
        }
        else
        {
            // too much fields -> invalidate the tag
            //
            reset();
        }
    }

    inline void decode(TPPM::BasicChannels raw_channels_in   ,
                       TPPM::ExtraChannels extra_channels_out,
                       TPPM::OnOffChannels onoff_channels_out)
    {
        if ((NULL == raw_channels_in)
            ||
            ((NULL == extra_channels_out)
             &&
             (NULL == onoff_channels_out)))
        {
            return;
        }

        if (NULL != extra_channels_out)
        {
            // Extra channels addressed by the superimposed tag's scan index:
            //
            // +-----++-------------------------------------------------------+
            // |     ||                         scan                          |
            // | CH. ||  0   |  1   |  2   |  3   |  4   |  5   |  6   |  7   |
            // +=====++======+======+======+======+======+======+======+======+
            // |  5  || P.5  | P.6  | P.7  | P.8  | P.5  | P.6  | P.7  | P.8  |
            // |  6  || P.9  | P.10 | P.11 | P.12 | P.9  | P.10 | P.11 | P.12 |
            // |  7  || P.13 | P.14 | P.15 | P.16 | P.13 | P.14 | P.15 | P.16 |
            // +-----++------+------+------+------+------+------+------+------+
            //
            for (uint8_t c=0; c<MULTI_PROP_CHANNELS; ++c)
            {
                extra_channels_out[(_tag.scan_index & 3) | (c << 2)] = raw_channels_in[c+MULTI_PROP_CHANNEL_POS];
            }
        }

        if (NULL != onoff_channels_out)
        {
            // On/Off channels addressed by the superimposed tag's scan index:
            //
            // +-----++-------------------------------------------------------+
            // |     ||                         scan                          |
            // | CH. ||  0   |  1   |  2   |  3   |  4   |  5   |  6   |  7   |
            // +=====++======+======+======+======+======+======+======+======+
            // |  8  || D.1  | D.2  | D.3  | D.4  | D.5  | D.6  | D.7  | D.8  |
            // +-----++------+------+------+------+------+------+------+------+
            //
            uint16_t &channel_val = raw_channels_in[MULTI_BITS_CHANNEL_POS];

            // On/Off channels bits addressed by the superimposed tag's scan index:
            //
            // +----+----------------------++------------------------------------------------------------------+
            // |    |                      ||                          scan index                              |
            // | CH | Function             ||   0  |   1  |   2   |   3    |   4    |   5    |   6    |   7    |
            // +----+----------------------++------+------+-------+--------+--------+--------+--------+--------+
            // |  8 | on/off channels bits || 0..3 | 4..7 | 8..11 | 12..15 | 16..19 | 20..23 | 24..27 | 28..31 |
            // +----+----------------------++------+------+-------+--------+--------+--------+--------+--------+
            //
            uint8_t bit_index = ((_tag.scan_index & 3) << 2);

            // On/Off channels bytes addressed by the superimposed tag's scan index:
            //
            // +----+-----------------------++-------------------------------+
            // |    |                       ||           scan index          |
            // | CH | Function              || 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
            // +----+-----------------------++---+---+---+---+---+---+---+---+
            // |  7 | on/off channels bytes || 0 | 0 | 1 | 1 | 2 | 2 | 3 | 3 |
            // +----+-----------------------++---+---+---+---+---+---+---+---+
            //
            uint8_t byte_index = _tag.scan_index >> 1;

            // clear the bits
            //
            onoff_channels_out[byte_index] &= ~(0x1111 << bit_index);

            // set the bits
            //
            if (channel_val >= ONOFF_THRESHOLD_1110)
            {
                onoff_channels_out[byte_index] |= (0x1111 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_1101)
            {
                onoff_channels_out[byte_index] |= (0x1110 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_1100)
            {
                onoff_channels_out[byte_index] |= (0x1101 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_1011)
            {
                onoff_channels_out[byte_index] |= (0x1100 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_1010)
            {
                onoff_channels_out[byte_index] |= (0x1011 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_1001)
            {
                onoff_channels_out[byte_index] |= (0x1010 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_1000)
            {
                onoff_channels_out[byte_index] |= (0x1001 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_0111)
            {
                onoff_channels_out[byte_index] |= (0x1000 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_0110)
            {
                onoff_channels_out[byte_index] |= (0x0111 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_0101)
            {
                onoff_channels_out[byte_index] |= (0x0110 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_0100)
            {
                onoff_channels_out[byte_index] |= (0x0101 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_0011)
            {
                onoff_channels_out[byte_index] |= (0x0100 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_0010)
            {
                onoff_channels_out[byte_index] |= (0x0011 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_0001)
            {
                onoff_channels_out[byte_index] |= (0x0010 << bit_index);
            }
            else
            if (channel_val >= ONOFF_THRESHOLD_0000)
            {
                onoff_channels_out[byte_index] |= (0x0001 << bit_index);
            }
        }
    }

    inline void connect()
    {
        if (_data.valid)
        {
            _data.coupled_id = _tag.encoder_id;
        }
    }

    inline bool is_valid()
    {
        return _data.valid;
    }

    inline bool is_trusted()
    {
        return _data.trusted;
    }

    inline bool is_encoded()
    {
        return _data.encoded;
    }

    inline uint8_t encoder_id()
    {
        return _tag.encoder_id;
    }

    inline uint8_t part_index()
    {
        return _tag.part_index;
    }

    inline uint8_t scan_index()
    {
        return _tag.scan_index;
    }

private:
    union Tag
    {
        Tag(): raw_bits(0) {}

        uint32_t raw_bits;

        struct
        {
            uint32_t encoder_id: 2;
            uint32_t quick_pb_1: 1;
            uint32_t quick_pb_2: 1;
            uint32_t decoder_id: 4;
            uint32_t part_index: 4;
            uint32_t scan_index: 3;
            uint32_t encoder_cs: 1;
            uint32_t decoder_cs: 1;
            uint32_t scan_cs   : 1;
        };
    };

    struct Data
    {
        Data()
            : coupled_id(UNDEFINED_ENCODER)
            , decoder_id(        0)
            , valid     (    false)
            , trusted   (    false)
            , encoded   (    false)
        {}

        inline void reset()
        {
            valid   = false;
            trusted = false;
            encoded = false;
        }

        uint16_t coupled_id: 8;
        uint16_t decoder_id: 4;
        uint16_t valid     : 1;
        uint16_t trusted   : 1;
        uint16_t encoded   : 1;
    };

    Tag  _tag;
    Data _data;

    uint16_t _pulse_min_width;
    uint16_t _pulse_max_width;

    enum CodeBits
    {
        ENCODER_ID_BIT_0  = 0,
        ENCODER_ID_BIT_1     ,
        QUICK_BTTN_BIT_0     ,
        QUICK_BTTN_BIT_1     ,
        DECODER_ID_BIT_0     ,
        DECODER_ID_BIT_1     ,
        DECODER_ID_BIT_2     ,
        DECODER_ID_BIT_3     ,
        PART_INDEX_BIT_0     ,
        PART_INDEX_BIT_1     ,
        PART_INDEX_BIT_2     ,
        PART_INDEX_BIT_3     ,
        SCAN_INDEX_BIT_0     ,
        SCAN_INDEX_BIT_1     ,
        SCAN_INDEX_BIT_2     ,
        ENCODER_ID_PARITY_BIT,
        DECODER_ID_PARITY_BIT,
        SCAN_INDEX_PARITY_BIT
    };
};

#endif // __TPPM_TAG_H__

#if !defined(__TPPM_TAG_H__)
#define __TPPM_TAG_H__

#include "TPPMCfg.h"

#define MAX_SUPERINPOSED_CHANNELS 11

#define INVALID_PATTERN_00        0b00000000000000000000000000000000
#define INVALID_PATTERN_01        0b00000000000101010101010101010101
#define INVALID_PATTERN_10        0b00000000001010101010101010101010
#define INVALID_PATTERN_11        0b00000000001111111111111111111111

#define MIN_CODE_THRESHOLD        USEC_TO_WIDTH( 300 ) // MIN_PULSE_WIDTH
#define MAX_CODE_THRESHOLD        USEC_TO_WIDTH( 460 ) // MAX_PULSE_WIDTH
#define CODE_THRESHOLD_STEP       ( ( MAX_CODE_THRESHOLD - MIN_CODE_THRESHOLD ) / 4 )

#define CODE_THRESHOLD_00         ( MIN_CODE_THRESHOLD + CODE_THRESHOLD_STEP )
#define CODE_THRESHOLD_01         ( CODE_THRESHOLD_00  + CODE_THRESHOLD_STEP )
#define CODE_THRESHOLD_10         ( CODE_THRESHOLD_01  + CODE_THRESHOLD_STEP )

#define BIT_VAL(word,bit_pos)     (((word) >> (bit_pos)) & 0x01)

#define ONOFF_THRESHOLD_STEP      ( ( MAX_CHANNEL_WIDTH - MIN_CHANNEL_WIDTH ) / 4 )

#define ONOFF_THRESHOLD_00        ( MIN_CHANNEL_WIDTH  + ONOFF_THRESHOLD_STEP )
#define ONOFF_THRESHOLD_01        ( ONOFF_THRESHOLD_00 + ONOFF_THRESHOLD_STEP )
#define ONOFF_THRESHOLD_10        ( ONOFF_THRESHOLD_01 + ONOFF_THRESHOLD_STEP )

#define MUXED_CHANNLES            3
#define FIRST_EXTRA_CHANNEL       4
#define FIRST_ONOFF_CHANNEL       7

class TPPMTag
{
public:
    TPPMTag()
        ; _raw_bits  (0)
        , _coupled_id(0)  // until coupled all transmitters are valid
        , _encoder_id(0)
        , _decoder_id(0)
        , _part_index(0)
        , _scan_index(0)
        , _valid     (false)
        , _trusted   (false)
        , _encoded   (false)
    {}

    inline void reset()
    {
        _raw_bits   = 0;
        _coupled_id = 0;
        _encoder_id = 0;
        _decoder_id = 0;
        _part_index = 0;
        _scan_index = 0;
        _valid      = false;
        _trusted    = false;
        _encoded    = false;
    }

    inline void set_decoder_id(const uint8_t &decoder_id)
    {
        _decoder_id = decoder_id & 0x07;
    }

    inline void update(const uint8_t &captures, const uint16_t &pulse_width)
    {
        _trusted = false;

        if (captures <= MAX_SUPERINPOSED_CHANNELS)
        {
            // collect data
            //
            if (IS_IN_RANGE(pulse_width,MIN_CODE_THRESHOLD,MAX_CODE_THRESHOLD))
            {
                // It's a valid pulse with superinposed code
                //
                uint8_t bit_index = (captures << 1);

                // clear the bits
                //
                _raw_bits &= ~(0x11 << bit_index);

                // set the bits
                //
                if (pulse_width >= CODE_THRESHOLD_10)
                {
                    _raw_bits |= (0x11 << bit_index);
                }
                else
                if (pulse_width >= CODE_THRESHOLD_01)
                {
                    _raw_bits |= (0x10 << bit_index);
                }
                else
                if (pulse_width >= CODE_THRESHOLD_00)
                {
                    _raw_bits |= (0x01 << bit_index);
                }
            }
            else
            {
                // invalid data -> invalidate the tag
                //
                _raw_bits = 0;
            }
        }
        else
        if (captures == MAX_SUPERINPOSED_CHANNELS)
        {
            bool valid = false;

            _raw_bits &= INVALID_PATTERN_11; // mask off all the extra bits

            // We have reached the maximum number of coded pulses
            //
            _encoded = ! ARE_VERY_CLOSE(_pulse.min_width, _pulse.max_width)
                       &&
                       ( _raw_bits != INVALID_PATTERN_00 )
                       &&
                       ( _raw_bits != INVALID_PATTERN_01 )
                       &&
                       ( _raw_bits != INVALID_PATTERN_10 )
                       &&
                       ( _raw_bits != INVALID_PATTERN_11 );

            if (_encoded)
            {
                // To ensure an unencoded PPM frame is not validated we check for
                // even parity on the encoder_id bits and for odd parity on the decoder_id
                // and scan bis.
                //
                // In an unencoded PPM all the pulses shall have the same width
                // thus the frame will fall in one of the following cases:
                //
                // _raw_bits              : 0000000000000000000000
                // _encoder_id            : 00000000 (expected even parity: 0)
                // _decoder_id+_part_index: 00000000 (expected odd  parity: 1)
                // _scan_index            : 0000     (expected odd  parity: 1)
                // leader_id_even_parity  : 0 ->     parity match
                // main_index_odd_parity  : 0 -> (E) parity does not match
                // scan_index_odd_parity  : 0 -> (E) parity does not match
                // ------------------------------------------------------
                // _raw_bits              : 0101010101010101010101
                // _encoder_id            : 00000000 (expected even parity: 0)
                // _decoder_id+_part_index: 11111111 (expected odd  parity: 1)
                // _scan_index            : 000      (expected odd  parity: 1)
                // leader_id_even_parity  : 1 -> (E) parity does not match
                // main_index_odd_parity  : 1 ->     parity match
                // scan_index_odd_parity  : 1 ->     parity match
                // ------------------------------------------------------
                // _raw_bits              : 1010101010101010101010
                // _encoder_id            : 11111111 (expected even parity: 0)
                // _decoder_id+_part_index: 00000000 (expected odd  parity: 1)
                // _scan_index            : 111      (expected odd  parity: 0)
                // leader_id_even_parity  : 0 ->     parity match
                // main_index_odd_parity  : 0 -> (E) parity does not match
                // scan_index_odd_parity  : 0 ->     parity match
                // ------------------------------------------------------
                // _raw_bits              : 1111111111111111111111
                // _encoder_id            : 11111111 (expected even parity: 0)
                // _decoder_id+_part_index: 11111111 (expected odd  parity: 1)
                // _scan_index            : 111      (expected odd  parity: 0)
                // leader_id_even_parity  : 1 -> (E) parity does not match
                // main_index_odd_parity  : 1 ->     parity match
                // scan_index_odd_parity  : 1 -> (E) parity does not match
                //
                valid = !(BIT_VAL(_raw_bits, TX_ID_BIT_0          ) ^
                          BIT_VAL(_raw_bits, TX_ID_BIT_1          ) ^
                          BIT_VAL(_raw_bits, TX_ID_BIT_2          ) ^
                          BIT_VAL(_raw_bits, TX_ID_BIT_3          ) ^
                          BIT_VAL(_raw_bits, TX_ID_BIT_4          ) ^
                          BIT_VAL(_raw_bits, TX_ID_BIT_5          ) ^
                          BIT_VAL(_raw_bits, TX_ID_BIT_6          ) ^
                          BIT_VAL(_raw_bits, TX_ID_BIT_7          ) ^
                          BIT_VAL(_raw_bits, TX_ID_EVEN_PARITY_BIT))
                        &&
                        (BIT_VAL(_raw_bits, RX_ID_BIT_0           ) ^
                         BIT_VAL(_raw_bits, RX_ID_BIT_1           ) ^
                         BIT_VAL(_raw_bits, RX_ID_BIT_2           ) ^
                         BIT_VAL(_raw_bits, RX_ID_BIT_3           ) ^
                         BIT_VAL(_raw_bits, RX_SUB_ID_BIT_0       ) ^
                         BIT_VAL(_raw_bits, RX_SUB_ID_BIT_1       ) ^
                         BIT_VAL(_raw_bits, RX_SUB_ID_BIT_2       ) ^
                         BIT_VAL(_raw_bits, RX_SUB_ID_BIT_3       ) ^
                         BIT_VAL(_raw_bits, RX_ID_ODD_PARITY_BIT) )
                        &&
                        (BIT_VAL(_raw_bits, SCAN_BIT_0            ) ^
                         BIT_VAL(_raw_bits, SCAN_BIT_1            ) ^
                         BIT_VAL(_raw_bits, SCAN_BIT_2            ) ^
                         BIT_VAL(_raw_bits, SCAN_ODD_PARITY_BIT)  );

                if (valid)
                {
                    _encoder_id = (BIT_VAL(_raw_bits, TX_ID_BIT_0) << 0) |
                                  (BIT_VAL(_raw_bits, TX_ID_BIT_1) << 1) |
                                  (BIT_VAL(_raw_bits, TX_ID_BIT_2) << 2) |
                                  (BIT_VAL(_raw_bits, TX_ID_BIT_3) << 3) |
                                  (BIT_VAL(_raw_bits, TX_ID_BIT_4) << 4) |
                                  (BIT_VAL(_raw_bits, TX_ID_BIT_5) << 5) |
                                  (BIT_VAL(_raw_bits, TX_ID_BIT_6) << 6) |
                                  (BIT_VAL(_raw_bits, TX_ID_BIT_7) << 7) ;

                    // It's really valid only if it comes from the coupled tx,
                    // if any, and...
                    //
                    valid = (_coupled_id == 0) || (_encoder_id == _coupled_id);
                }

                _trusted = valid;

                if (valid)
                {
                    uint8_t decoder_id;

                    decoder_id = (BIT_VAL(_raw_bits, RX_ID_BIT_0) << 0) |
                                 (BIT_VAL(_raw_bits, RX_ID_BIT_1) << 1) |
                                 (BIT_VAL(_raw_bits, RX_ID_BIT_2) << 2) |
                                 (BIT_VAL(_raw_bits, RX_ID_BIT_3) << 3) ;

                    // ...it's really valid only if it is for me
                    //
                    valid = ( decoder_id == _decoder_id );
                }

                if (valid)
                {
                    _part_index = (BIT_VAL(_raw_bits, RX_SUB_ID_BIT_0) << 0) |
                                  (BIT_VAL(_raw_bits, RX_SUB_ID_BIT_1) << 1) |
                                  (BIT_VAL(_raw_bits, RX_SUB_ID_BIT_2) << 2) |
                                  (BIT_VAL(_raw_bits, RX_SUB_ID_BIT_3) << 3) ;

                    _scan_index = (BIT_VAL(_raw_bits, SCAN_BIT_0) << 0) |
                                  (BIT_VAL(_raw_bits, SCAN_BIT_2) << 1) |
                                  (BIT_VAL(_raw_bits, SCAN_BIT_3) << 2) ;
                }
            }

            _valid = valid;
        }
        else
        {
            // too much fields -> invalidate the tag
            //
            _raw_bits = 0;
            _valid    = false;
            _trusted  = false;
            _encoded  = false;
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

        for (uint8_t c=0; c<MUXED_CHANNLES; ++c)
        {
            if (NULL != extra_channels_out)
            {
                // Extra channels addressed by the superimposed tag's scan index:
                //
                // +----+----------------++----------------------------------------+
                // |    |                ||                scan index              |
                // | CH | Function       ||  0  |  1 |  2 |  3 |  4 |  5 |  6 |  7 |
                // +----+----------------++-----+----+----+----+----+----+----+----+
                // |  4 | extra channels ||  0  |  1 |  2 |  3 |  0 |  1 |  2 |  3 |
                // |  5 | extra channels ||  4  |  5 |  6 |  7 |  4 |  5 |  6 |  7 |
                // |  6 | extra channels ||  8  |  9 | 10 | 11 |  8 |  9 | 10 | 11 |
                // +----+----------------++-----+----+----+----+----+----+----+----+
                //
                extra_channels_out[(_scan_index & 3) | (c << 2)] = raw_channels_in[c+FIRST_EXTRA_CHANNEL];
            }

            if (NULL != onoff_channels_out)
            {
                // On/Off channels addressed by the superimposed tag's scan index:
                //
                // +----+-----------------++-------------------------------------------------------------------------------+
                // |    |                 ||                                   scan index                                  |
                // | CH | Function        ||    0    |    1    |    2    |    3    |    4    |    5    |    6    |    7    |
                // +----+-----------------++---------+---------+---------+---------+---------+---------+---------+---------+
                // |  7 | on/off channels ||  0 /  1 |  2 /  3 |  4 /  5 |  6 /  7 |  8 /  9 | 10 / 11 | 12 / 13 | 14 / 15 |
                // |  8 | on/off channels || 16 / 17 | 18 / 19 | 20 / 21 | 22 / 23 | 24 / 25 | 26 / 27 | 28 / 28 | 30 / 31 |
                // |  9 | on/off channels || 32 / 33 | 34 / 35 | 36 / 37 | 38 / 39 | 40 / 41 | 42 / 43 | 44 / 45 | 46 / 47 |
                // +----+-----------------++---------+---------+---------+---------+---------+---------+---------+---------+
                //
                uint8_t &channel_val = raw_channels_in[c+FIRST_ONOFF_CHANNEL];

                // On/Off channels bits addressed by the superimposed tag's scan index:
                //
                // +----+----------------------++---------------------------------------------------------------+
                // |    |                      ||                           scan index                          |
                // | CH | Function             ||   0   |   1   |   2   |   3   |   4   |   5   |   6   |   7   |
                // +----+----------------------++-------+-------+-------+-------+-------+-------+-------+-------+
                // |  7 | on/off channels bits || 0 / 1 | 2 / 3 | 4 / 5 | 6 / 7 | 0 / 1 | 2 / 3 | 4 / 5 | 6 / 7 |
                // |  8 | on/off channels bits || 0 / 1 | 2 / 3 | 4 / 5 | 6 / 7 | 0 / 1 | 2 / 3 | 4 / 5 | 6 / 7 |
                // |  9 | on/off channels bits || 0 / 1 | 2 / 3 | 4 / 5 | 6 / 7 | 0 / 1 | 2 / 3 | 4 / 5 | 6 / 7 |
                // +----+----------------------++-------+-------+-------+-------+-------+-------+-------+-------+
                //
                uint8_t bit_index = ((_scan_index & 3) << 1);

                // On/Off channels bytes addressed by the superimposed tag's scan index:
                //
                // +----+-----------------------++-------------------------------+
                // |    |                       ||           scan index          |
                // | CH | Function              || 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 |
                // +----+-----------------------++---+---+---+---+---+---+---+---+
                // |  7 | on/off channels bytes || 0 | 0 | 0 | 0 | 1 | 1 | 1 | 1 |
                // |  8 | on/off channels bytes || 2 | 2 | 2 | 2 | 3 | 3 | 3 | 3 |
                // |  9 | on/off channels bytes || 4 | 4 | 4 | 4 | 5 | 5 | 5 | 5 |
                // +----+-----------------------++---+---+---+---+---+---+---+---+
                //
                uint8_t byte_index = (c << 2) + ((_scan_index & 4) >> 3);

                // clear the bits
                //
                onoff_channels_out[byte_index] &= ~(0x11 << bit_index);

                // set the bits
                //
                if (channel_val >= ONOFF_THRESHOLD_10)
                {
                    onoff_channels_out[byte_index] |= (0x11 << bit_index);
                }
                else
                if (channel_val >= ONOFF_THRESHOLD_01)
                {
                    onoff_channels_out[byte_index] |= (0x10 << bit_index);
                }
                else
                if (channel_val >= ONOFF_THRESHOLD_00)
                {
                    onoff_channels_out[byte_index] |= (0x01 << bit_index);
                }
            }
        }
    }

    inline void connect()
    {
        if (_valid)
        {
            _coupled_id = _encoder_id;
        }
    }

    inline bool is_valid()
    {
        return _valid;
    }

    inline bool is_trusted()
    {
        return _trusted;
    }

    inline bool is_encoded()
    {
        return _encoded;
    }

    inline uint8_t encoder_id()
    {
        return _encoder_id;
    }

    inline uint8_t part_index()
    {
        return _part_index;
    }

    inline uint8_t scan_index()
    {
        return _scan_index;
    }

private:
    uint32_t _raw_bits  ;
    uint8_t  _coupled_id;
    uint8_t  _encoder_id;
    uint8_t  _decoder_id;
    uint8_t  _part_index;
    uint8_t  _scan_index;
    bool     _valid     ;
    bool     _trusted   ;
    bool     _encoded   ;

    enum CodeBits
    {
        TX_ID_BIT_0       = 0,
        RX_ID_BIT_0          ,
        TX_ID_BIT_1          ,
        RX_ID_BIT_1          ,
        TX_ID_BIT_2          ,
        RX_ID_BIT_2          ,
        TX_ID_BIT_3          ,
        RX_ID_BIT_3          ,
        TX_ID_BIT_4          ,
        RX_SUB_ID_BIT_0      ,
        TX_ID_BIT_5          ,
        RX_SUB_ID_BIT_1      ,
        TX_ID_BIT_6          ,
        RX_SUB_ID_BIT_2      ,
        TX_ID_BIT_7          ,
        RX_SUB_ID_BIT_3      ,
        SCAN_BIT_0           ,
        TX_ID_EVEN_PARITY_BIT, // this ensures the frame can't have equal pulses
        SCAN_BIT_1           ,
        RX_ID_ODD_PARITY_BIT ,
        SCAN_BIT_2           ,
        SCAN_ODD_PARITY_BIT
    };
};

#endif // __TPPM_TAG_H__

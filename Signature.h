#if !defined(__SIGNATURE_H__)
#define __SIGNATURE_H__

#include <inttypes.h>

#include "Signal.h"

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

class Signature
{
public:
    Signature(const uint8_t &rx_id)
        : _rx_id        (rx_id)
        , _raw_bits     (0)
        , _paired_tx_id (0)  // until paired all transmitters are valid
        , _valid        (false)
        , _encoded      (false)
    {}

    inline void reset()
    {
        _raw_bits     = 0;
        _paired_tx_id = 0;
        _valid        = false;
        _encoded      = false;
    }

    inline void update(const Signal &pulse)
    {
        if ((pulse.captures > 0) && (pulse.captures <= MAX_SUPERINPOSED_CHANNELS))
        {
            // collect data
            //
            uint8_t &pulse_duration = pulse.duration[pulse.captures-1];

            if (IS_IN_RANGE(pulse_duration,MIN_CODE_THRESHOLD,MAX_CODE_THRESHOLD))
            {
                // It's a valid pulse with superinposed code
                //
                uint8_t bit_index = ((pulse.captures-1) << 1);

                // clear the bits
                //
                _raw_bits &= ~(0x11 << bit_index);

                // set the bits
                //
                if (pulse_duration >= CODE_THRESHOLD_10)
                {
                    _raw_bits |= (0x11 << bit_index);
                }
                else
                if (pulse_duration >= CODE_THRESHOLD_01)
                {
                    _raw_bits |= (0x10 << bit_index);
                }
                else
                if (pulse_duration >= CODE_THRESHOLD_00)
                {
                    _raw_bits |= (0x01 << bit_index);
                }
            }
            else
            {
                // invalid data -> invalidate signature
                //
                _raw_bits = 0;
            }
        }
        else
        if (pulse.caputures == MAX_SUPERINPOSED_CHANNELS)
        {
            bool valid = false;

            _raw_bits &= INVALID_PATTERN_11; // mask off all the extra bits

            // We have reached the maximum number of coded pulses
            //
            _encoded = ! ARE_VERY_CLOSE(_pulse.min_duration, _pulse.max_duration)
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
                // even parity on the tx_id bits and for odd parity on the rx_id
                // and scan bis.
                //
                // In an unencoded PPM all the pulses shall have the same width
                // thus the frame will fall in one of the following cases:
                //
                // _raw_bits        : 0000000000000000000000
                // _tx_id           : 00000000 (expected even parity: 0)
                // _rx_id+_rx_sub_id: 00000000 (expected odd  parity: 1)
                // _scan            : 0000     (expected odd  parity: 1)
                // tx_id_even_parity: 0 ->     parity match
                // rx_id_odd_parity : 0 -> (E) parity does not match
                // scan_odd_parity  : 0 -> (E) parity does not match
                // ------------------------------------------------------
                // _raw_bits        : 0101010101010101010101
                // _tx_id           : 00000000 (expected even parity: 0)
                // _rx_id+_rx_sub_id: 11111111 (expected odd  parity: 1)
                // _scan            : 000      (expected odd  parity: 1)
                // tx_id_even_parity: 1 -> (E) parity does not match
                // rx_id_odd_parity : 1 ->     parity match
                // scan_odd_parity  : 1 ->     parity match
                // ------------------------------------------------------
                // _raw_bits        : 1010101010101010101010
                // _tx_id           : 11111111 (expected even parity: 0)
                // _rx_id+_rx_sub_id: 00000000 (expected odd  parity: 1)
                // _scan            : 111      (expected odd  parity: 0)
                // tx_id_even_parity: 0 ->     parity match
                // rx_id_odd_parity : 0 -> (E) parity does not match
                // scan_odd_parity  : 0 ->     parity match
                // ------------------------------------------------------
                // _raw_bits        : 1111111111111111111111
                // _tx_id           : 11111111 (expected even parity: 0)
                // _rx_id+_rx_sub_id: 11111111 (expected odd  parity: 1)
                // _scan            : 111      (expected odd  parity: 0)
                // tx_id_even_parity: 1 -> (E) parity does not match
                // rx_id_odd_parity : 1 ->     parity match
                // scan_odd_parity  : 1 -> (E) parity does not match
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
                    uint8_t rx_id;

                    rx_id = (BIT_VAL(_raw_bits, RX_ID_BIT_0) << 0) |
                            (BIT_VAL(_raw_bits, RX_ID_BIT_1) << 1) |
                            (BIT_VAL(_raw_bits, RX_ID_BIT_2) << 2) |
                            (BIT_VAL(_raw_bits, RX_ID_BIT_3) << 3) ;

                    // It's really valid only if it is for me and ...
                    //
                    valid = ( rx_id == _rx_id );
                }

                if (valid)
                {
                    _tx_id = (BIT_VAL(_raw_bits, TX_ID_BIT_0) << 0) |
                             (BIT_VAL(_raw_bits, TX_ID_BIT_1) << 1) |
                             (BIT_VAL(_raw_bits, TX_ID_BIT_2) << 2) |
                             (BIT_VAL(_raw_bits, TX_ID_BIT_3) << 3) |
                             (BIT_VAL(_raw_bits, TX_ID_BIT_4) << 4) |
                             (BIT_VAL(_raw_bits, TX_ID_BIT_5) << 5) |
                             (BIT_VAL(_raw_bits, TX_ID_BIT_6) << 6) |
                             (BIT_VAL(_raw_bits, TX_ID_BIT_7) << 7) ;

                    // ... it's really valid only if it comes from the paired tx,
                    // if any
                    //
                    valid = (_paired_tx_id == 0) || (_tx_id == _paired_tx_id);
                }

                if (valid)
                {
                    _rx_sub_id = (BIT_VAL(_raw_bits, RX_SUB_ID_BIT_0) << 0) |
                                 (BIT_VAL(_raw_bits, RX_SUB_ID_BIT_1) << 1) |
                                 (BIT_VAL(_raw_bits, RX_SUB_ID_BIT_2) << 2) |
                                 (BIT_VAL(_raw_bits, RX_SUB_ID_BIT_3) << 3) ;

                    _scan = (BIT_VAL(_raw_bits, SCAN_BIT_0) << 0) |
                            (BIT_VAL(_raw_bits, SCAN_BIT_2) << 1) |
                            (BIT_VAL(_raw_bits, SCAN_BIT_3) << 2) ;
                }
            }

            _valid = valid;
        }
        else
        {
            // too much fields -> invalidate signature
            //
            _raw_bits = 0;
            _valid    = false;
            _encoded  = false;
        }
    }

    inline void pair()
    {
        if (_valid)
        {
            _paired_tx_id = _tx_id;
        }
    }

    inline bool is_valid()
    {
        return _valid;
    }

    inline bool is_encoded()
    {
        return _encoded;
    }

    inline uint8_t tx_id()
    {
        return _tx_id;
    }

    inline uint8_t rx_sub_id()
    {
        return _rx_sub_id;
    }

    inline uint8_t scan()
    {
        return _scan;
    }

private:
    uint8_t  _rx_id;
    uint32_t _raw_bits;
    uint8_t  _paired_tx_id;
    uint8_t  _tx_id;
    uint8_t  _rx_sub_id;
    uint8_t  _scan;
    bool     _valid;
    bool     _encoded;

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

#endif // __SIGNATURE_H__

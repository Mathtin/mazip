#ifndef ARBASECODER_H
#define ARBASECODER_H

#include "mazipinc.h"
#include "mazipint.h"
#include "mfttransform.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

enum { WAITING = 0, CODING = 1, COMPLETED, ERROR, NOT_MAZIP };

typedef uint32_t TCode;

class TArBaseCoder {
  public:
    TArBaseCoder(bool mft);
    ~TArBaseCoder(){};

    virtual operator bool() const {
        return state == CODING;
    }

  protected:
    static const size_t CODE_BIT_SIZE = UINT_BIT_SIZE;
    static const size_t MAX_CODE = (1ull << CODE_BIT_SIZE) - 1;
    static const size_t MAX_FREQ = (1ull << (CODE_BIT_SIZE - 2)) - 1;
    static const uint32_t EXPANDER = (MAX_CODE >> 2) + 1;
    static const uint32_t LEFT_BIT = 1u << (CODE_BIT_SIZE - 1);
    static const uint32_t LEFT_SHIFT = CODE_BIT_SIZE - 1;

    static const uint32_t FQTR = (MAX_CODE >> 2) + 1;
    static const uint32_t TQTR = FQTR * 3;

    static const size_t REAL_ALPHABET = 0x100;
    static const size_t ALPHABET = 0x101;
    static const TSymbol EOF_SYM;
    TArBaseCoder(TArBaseCoder &);
    void UpdateFreq(TSymbol & sym);
    inline void CalcBorders(const TSymbol & sym) {
        right = left + (range * cumFreq[sym - 1]) / cumFreq[0] - 1;
        left = left + (range * cumFreq[sym]) / cumFreq[0];
    }
    inline void CalcRange() {
        range = (size_t)(right - left) + 1;
    }
    inline void ExpandBorders() {
        left = (left - EXPANDER) << 1;
        right = ((right - EXPANDER) << 1) | 1;
    }
    inline void ShiftBorders() {
        left = left << 1;
        right = (right << 1) | 1;
    }
    inline bool BordersCentered() {
        return left >= FQTR && right < TQTR;
    }
    inline bool BordersLeftBitEqual() {
        return (left & LEFT_BIT) == (right & LEFT_BIT);
    }
    size_t freq[ALPHABET + 1];
    size_t cumFreq[ALPHABET + 1];
    byte stob[ALPHABET + 1];
    TSymbol btos[REAL_ALPHABET];
    TCode left;
    TCode right;
    size_t range;
    byte bitBuffer;
    byte bitBufferSize;
    byte state;
    bool mft;
    TMFTTransform mfttr;
};

#endif // ! ARBASECODER_H

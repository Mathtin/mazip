#include "arbasecoder.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

TArBaseCoder::TArBaseCoder(bool mft)
    : left(0)
    , right(MAX_CODE)
    , range(MAX_CODE)
    , bitBuffer(0)
    , bitBufferSize(0)
    , state(WAITING)
    , mft(mft) {
    stob[0] = 255u;
    for (size_t i = 0; i < REAL_ALPHABET; ++i) {
        btos[i] = i + 1;
        stob[i + 1] = i;
    }
    stob[REAL_ALPHABET] = 255u;
    stob[ALPHABET] = 255u;
    for (size_t i = 0; i <= ALPHABET; ++i) {
        freq[i] = 1;
        cumFreq[i] = ALPHABET - i;
    }
    freq[0] = 0;
}

void TArBaseCoder::UpdateFreq(TSymbol & sym) {
    /* LIMIT FREQUENCY */
    size_t i;
    if (cumFreq[0] == MAX_FREQ) {
        size_t cum = 0;
        for (i = ALPHABET; i + 1; --i) {
            freq[i] = (freq[i] + 1) >> 1;
            cumFreq[i] = cum;
            cum += freq[i];
        }
    }
    /* UPDATE CODING TABLE */
    for (i = sym; freq[i] == freq[i - 1]; --i) {
    }
    if (i != sym) {
        std::swap(btos[stob[i]], btos[stob[sym]]);
        std::swap(stob[i], stob[sym]);
    }
    sym = i;
    /* UPDATE FREQ */
    ++freq[i];
    for (--i; i + 1; --i) {
        ++cumFreq[i];
    }
}

const TSymbol TArBaseCoder::EOF_SYM = 0x101;

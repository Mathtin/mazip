#include "ardecoder.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

int TArDecoder::Begin() {
    if (state == CODING) {
        return state;
    }
    state = CODING;
    if (buffer.size() < sizeof(MAZIP_HEADER)) {
        state = NOT_MAZIP;
        return state;
    }
    for (pos = 0; pos < sizeof(MAZIP_HEADER) - 1; ++pos) {
        if (buffer[pos] != MAZIP_HEADER[pos]) {
            state = NOT_MAZIP;
            return state;
        }
    }
    byte b = buffer[pos];
    if (b & 1u) {
        mft = true;
    } else {
        mft = false;
    }
    ++pos;
    for (size_t i = 0; i < CODE_BIT_SIZE; ++i) {
        value = (value << 1) | FetchBit();
    }
    return state;
}

int TArDecoder::Complete() {
    state = COMPLETED;
    return state;
}

void TArDecoder::DecodeSymbol(TSymbol & sym) {
    CalcRange();
    size_t symFreq = (((size_t)(value - left) + 1) * cumFreq[0] - 1) / range;
    for (sym = 1; symFreq < cumFreq[sym] && sym < ALPHABET + 1; ++sym) {
    }
    CalcBorders(sym);
    if (sym == EOF_SYM) {
        state = COMPLETED;
        return;
    }
    while (true) {
        if (BordersLeftBitEqual()) {
            ShiftBorders();
            value = (value << 1) | FetchBit();
            continue;
        } else if (BordersCentered()) {
            ExpandBorders();
            value = (value - EXPANDER) << 1 | FetchBit();
            continue;
        }
        break;
    }
    UpdateFreq(sym);
}

int TArDecoder::Decode(IBuffer & buf) {
    byte b;
    for (size_t i = 0; i < buf.size() && state == CODING; ++i) {
        if (Decode(b)) {
            return state;
        }
        buf[i] = b;
    }
    while (state == CODING) {
        if (Decode(b)) {
            return state;
        }
        buf.push(b);
    }
    return state;
}

bit TArDecoder::FetchBit() {
    if (bitBufferSize == 0) {
        bitBufferSize = BYTE_BIT_SIZE;
        if (pos < buffer.size()) {
            bitBuffer = buffer[pos];
        }
        ++pos;
    }
    if (pos > buffer.size()) {
        ++emptyBits;
        if (emptyBits > CODE_BIT_SIZE - 2) {
            state = ERROR;
        }
    }
    bit b = bitBuffer >> (BYTE_BIT_SIZE - 1);
    bitBuffer <<= 1, --bitBufferSize;
    return b;
}

#include "arencoder.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

int TArEncoder::Encode(const IBuffer & buf) {
    if (state != CODING) {
        return state;
    }
    for (size_t i = 0; i < buf.size() && state == CODING; ++i) {
        if (Encode(buf[i])) {
            return state;
        }
    }
    return state;
}

int TArEncoder::Begin() {
    if (state == CODING) {
        return CODING;
    }
    state = CODING;
    for (size_t i = 0; i < sizeof(MAZIP_HEADER) - 1; ++i) {
        buffer.push(MAZIP_HEADER[i]);
    }
    byte b = 0;
    if (mft) {
        b |= 1u;
    }
    buffer.push(b);
    return state;
}

int TArEncoder::Complete() {
    if (state != COMPLETED) {
        EncodeSymbol(EOF_SYM);
        EncodeSymbol(EOF_SYM);
        EncodeSymbol(EOF_SYM);
        EncodeSymbol(EOF_SYM);
        ++expanded;
        left <<= 1;
        StoreLeftBit();
        FlushBitBuffer();
        state = COMPLETED;
    }
    return COMPLETED;
}

void TArEncoder::EncodeSymbol(const TSymbol & sym) {
    CalcRange();
    CalcBorders(sym);
    while (true) {
        if (BordersLeftBitEqual()) {
            StoreLeftBit();
            ShiftBorders();
            continue;
        } else if (BordersCentered()) {
            ExpandBorders();
            ++expanded;
            continue;
        }
        break;
    }
    TSymbol tmp = sym;
    UpdateFreq(tmp);
}

void TArEncoder::StoreLeftBit() {
    bit b = left >> LEFT_SHIFT;
    bitBuffer = (bitBuffer << 1) | b, ++bitBufferSize;
    if (bitBufferSize == BYTE_BIT_SIZE) {
        buffer.push(bitBuffer);
        bitBuffer = bitBufferSize = 0;
    }
    b = !b;
    for (; expanded; --expanded) {
        bitBuffer = (bitBuffer << 1) | b, ++bitBufferSize;
        if (bitBufferSize == BYTE_BIT_SIZE) {
            buffer.push(bitBuffer);
            bitBuffer = bitBufferSize = 0;
        }
    }
}

void TArEncoder::FlushBitBuffer() {
    if (bitBufferSize) {
        buffer.push(bitBuffer << bitBufferSize);
        bitBuffer = bitBufferSize = 0;
    }
}

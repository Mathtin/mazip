#include "mfttransform.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

TMFTTransform::TMFTTransform() {
    for (size_t i = 0; i < 0x100; ++i) {
        keys[i] = i;
    }
}

byte TMFTTransform::Transform(byte b) {
    if (keys[0] == b) {
        return 0u;
    }
    byte pos = 0;
    while (keys[pos] != b) {
        ++pos;
    }
    for (byte i = pos; i > 0; --i) {
        keys[i] = keys[i - 1];
    }
    keys[0] = b;
    return pos;
}

byte TMFTTransform::Reverse(byte pos) {
    if (pos == 0) {
        return keys[pos];
    }
    byte b = keys[pos];
    for (byte i = pos; i > 0; --i) {
        keys[i] = keys[i - 1];
    }
    keys[0] = b;
    return b;
}

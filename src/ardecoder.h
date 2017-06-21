#ifndef ARDECODER_H
#define ARDECODER_H

#include "arbasecoder.h"
#include "mazipinc.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

class TArDecoder : public TArBaseCoder {
  public:
    TArDecoder(const IBuffer & buff)
        : TArBaseCoder(true), buffer(buff), pos(0), value(0), emptyBits(0) {}
    ~TArDecoder(){};

    int Begin();
    int Complete();

    int Decode(byte & b) {
        TSymbol sym;
        DecodeSymbol(sym);
        if (state != CODING) {
            return state;
        }
        b = stob[sym];
        if (mft) {
            b = mfttr.Reverse(b);
        }
        return 0;
    }
    int Decode(IBuffer &);

  private:
    void DecodeSymbol(TSymbol &);
    bit FetchBit();
    const IBuffer & buffer;
    size_t pos;
    TCode value;
    size_t emptyBits;
};

#endif // ! ARDECODER_H

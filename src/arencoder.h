#ifndef ARENCODER_H
#define ARENCODER_H

#include "arbasecoder.h"
#include "mazipinc.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

class TArEncoder : public TArBaseCoder {
  public:
    TArEncoder(IBuffer & buff, bool mft)
        : TArBaseCoder(mft), buffer(buff), expanded(0) {}
    ~TArEncoder() {}

    int Begin();
    int Complete();

    int Encode(const byte & b) {
        if (state != CODING) {
            return state;
        }
        TSymbol sym;
        if (mft) {
            sym = btos[mfttr.Transform(b)];
        } else {
            sym = btos[b];
        }
        EncodeSymbol(sym);
        return 0;
    }
    int Encode(const IBuffer &);

    const IBuffer & Get() const {
        return buffer;
    }

  private:
    void EncodeSymbol(const TSymbol &);
    void StoreLeftBit();
    void FlushBitBuffer();
    IBuffer & buffer;
    size_t expanded;
};

#endif // ! ARENCODER_H

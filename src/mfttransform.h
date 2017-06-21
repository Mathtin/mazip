#ifndef MFTTRANSFORM_H
#define MFTTRANSFORM_H

#include "mazipinc.h"
#include "mazipint.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

class TMFTTransform {
  public:
    TMFTTransform();
    ~TMFTTransform() {}
    byte Transform(byte b);
    byte Reverse(byte b);

  protected:
    int keys[0x100];
};

#endif // ! MFTTRANSFORM_H

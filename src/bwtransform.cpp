#include "bwtransform.h"
/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

int pos[1 << 17];
int sa[1 << 17];
int tmp[1 << 17];
size_t count[0x100];
int N, gap;

#define REP(i, n) for (int i = 0; i < n; ++i)
static bool sufCmp(int i, int j) {
    if (pos[i] != pos[j]) {
        return pos[i] < pos[j];
    }
    i += gap;
    j += gap;
    return (i < N && j < N) ? pos[i] < pos[j] : i > j;
}

static void buildSA(byte * s, int n) {
    N = n;
    tmp[0] = 0;
    REP(i, N) sa[i] = i, pos[i] = s[i];
    for (gap = 1;; gap *= 2) {
        std::sort(sa, sa + N, sufCmp);
        REP(i, N - 1) tmp[i + 1] = tmp[i] + sufCmp(sa[i], sa[i + 1]);
        REP(i, N) pos[sa[i]] = tmp[i];
        if (tmp[N - 1] == N - 1)
            break;
    }
}

static uint16_t BWTBuffer(byte * input, byte * output, size_t sz) {
    uint16_t opos = 0;
    memcpy(output, input, sz);
    memcpy(output + sz, input, sz);
    buildSA(output, sz * 2);
    for (size_t i = 0, j = 0; i < sz * 2; ++i) {
        if (sa[i] >= 0 && sa[i] < (int)sz) {
            output[j++] = output[sz + sa[i] - 1];
        }
        if (sa[i] == 0) {
            opos = j - 1;
        }
    }
    return opos;
}

static void BWRBuffer(byte * input, byte * output, size_t sz, size_t j) {
    for (size_t i = 0; i < 0x100; ++i) {
        count[i] = 0;
    }
    for (size_t i = 0; i < sz; ++i) {
        ++count[input[i]];
    }
    size_t sum = 0;
    for (size_t i = 0; i < 0x100; ++i) {
        sum += count[i];
        count[i] = sum - count[i];
    }
    for (size_t i = 0; i < sz; ++i) {
        pos[count[input[i]]++] = i;
    }
    j = pos[j];
    for (size_t i = 0; i < sz; i++) {
        output[i] = input[j];
        j = pos[j];
    }
}

static byte PageToFlag(size_t page) {
    if (page == L_PAGE) {
        return FLAG_BEST;
    } else if (page == S_PAGE) {
        return FLAG_FAST;
    }
    return 0;
}

BWTransform::BWTransform(TMaFile & buffer, bool write)
    : file(buffer)
    , offsetPages(-1)
    , grow(0)
    , gpos(0)
    , w(write)
    , pageSet(false) {
    bwtpage = file.Page() + sizeof(uint16_t);
    bwtsize = (file.size() / file.Page()) * bwtpage + 1;
    if (file.size() % file.Page()) {
        bwtsize += file.size() % file.Page() + sizeof(uint16_t);
    }
    gpos = (bwtsize - 1) % bwtpage;
    if (w) {
        LoadPage(0);
    }
}

BWTransform::~BWTransform() {
    if (w && offsetPages != (size_t)-1) {
        SwapOff();
    }
}

void BWTransform::LoadPage(size_t p) {
    if (p == offsetPages) {
        return;
    } else if (w && offsetPages != (size_t)-1) {
        SwapOff();
    }
    file.LoadPage(p);
    offsetPages = p;
    if (w) {
        return;
    }
    uint16_t opos = BWTBuffer(file.data(), buff + 3, file.PageSize());
    buff[0] = PageToFlag(file.Page());
    buff[1] = opos;
    buff[2] = opos >> 8;
}

void BWTransform::SwapOff() {
    if (grow - 2 > 0) {
        resize(bwtsize + grow);
        grow = 0;
    }
    BWRBuffer(buff + 3, file.data(), file.PageSize(),
              buff[1] | ((buff[2]) << 8));
}

void BWTransformSTDIN::LoadPage(size_t p) {
    bwtsize = file.size();
    if (bwtsize == 0) {
        return;
    }
    offsetPages = p;
    byte * filepage = file.data();
    size_t i;
    for (i = 0; i < file.size() && i < file.Page(); ++i) {
        filepage[i] = file[i];
    }
    grow = i + sizeof(uint16_t);
    if (p == 0) {
        ++grow;
    }
    uint16_t opos = BWTBuffer(filepage, buff + 3, i);
    buff[0] = PageToFlag(file.Page());
    buff[1] = opos;
    buff[2] = opos >> 8;
}

void BWTransformSTDOUT::LoadPage(size_t p) {
    if (p == offsetPages) {
        return;
    } else if (offsetPages != (size_t)-1) {
        SwapOff();
    }
    offsetPages = p;
}

void BWTransformSTDOUT::SwapOff() {
    if (gpos < 2) {
        return;
    }
    size_t off = gpos - 2;
    gpos = 0;
    byte * offBuff = file.data();
    BWRBuffer(buff + 3, offBuff, off, buff[1] | ((buff[2]) << 8));
    for (size_t i = 0; i < off; ++i) {
        file[i] = offBuff[i];
    }
}

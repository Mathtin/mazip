#include "bwtransform.h"
/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

size_t sa[L_PAGE];
size_t tmp[L_PAGE];
size_t tmp2[L_PAGE];
size_t count[L_PAGE];
size_t *cl, *workTmp;

#define REP(i, n) for (size_t i = 0; i < n; ++i)
#define REP1(i, n) for (size_t i = 1; i < n; ++i)
#define REVREP(i, n) for (size_t i = n; i + 1; --i)
#define NOT_EQUAL_CL(i, step, sz)                                              \
    cl[sa[i]] != cl[sa[i - 1]] ||                                              \
        cl[(sa[i] + step) % sz] != cl[(sa[i - 1] + step) % sz]

static void SortSA(const size_t & step, const size_t & clcount,
                   const size_t & sz) {
    REP(i, sz) {
        workTmp[i] = (sa[i] + sz - step) % sz;
    }
    memset(count, 0, clcount * sizeof(size_t));
    REP(i, sz) {
        ++count[cl[workTmp[i]]];
    }
    REP1(i, clcount) {
        count[i] += count[i - 1];
    }
    REVREP(i, sz - 1) {
        sa[--count[cl[workTmp[i]]]] = workTmp[i];
    }
}

static void CalcCl(const size_t & step, size_t & clcount, const size_t & sz) {
    workTmp[sa[0]] = 0;
    clcount = 0;
    REP1(i, sz) {
        if (NOT_EQUAL_CL(i, step, sz)) {
            ++clcount;
        }
        workTmp[sa[i]] = clcount;
    }
    ++clcount;
    std::swap(cl, workTmp);
}

static void buildSA(const byte * s, const size_t & sz) {
    cl = tmp, workTmp = tmp2;
    memset(count, 0, 0x100 * sizeof(size_t));
    REP(i, sz) {
        ++count[s[i]];
    }
    REP1(i, 0x100) {
        count[i] += count[i - 1];
    }
    REP(i, sz) {
        sa[--count[s[i]]] = i;
    }
    cl[sa[0]] = 0;
    size_t clcount = 0;
    REP1(i, sz) {
        if (s[sa[i]] != s[sa[i - 1]]) {
            ++clcount;
        }
        cl[sa[i]] = clcount;
    }
    ++clcount;
    for (size_t h = 0; (1u << h) < sz; ++h) {
        SortSA(1 << h, clcount, sz);
        CalcCl(1 << h, clcount, sz);
    }
}

static uint16_t BWTBuffer(byte * input, byte * output, size_t sz) {
    if (sz == 0) {
        return 0;
    }
    uint16_t opos = 0;
    buildSA(input, sz);
    REP(i, sz) {
        output[i] = input[(sa[i] + sz - 1) % sz];
        if (sa[i] == 0) {
            opos = i;
        }
    }
    return opos;
}

static void BWRBuffer(byte * input, byte * output, size_t sz, size_t j) {
    memset(count, 0, 0x100 * sizeof(size_t));
    REP(i, sz) {
        ++count[input[i]];
    }
    size_t sum = 0;
    REP(i, 0x100) {
        sum += count[i];
        count[i] = sum - count[i];
    }
    REP(i, sz) {
        tmp[count[input[i]]++] = i;
    }
    j = tmp[j];
    REP(i, sz) {
        output[i] = input[j];
        j = tmp[j];
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
    if (grow > 2) {
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

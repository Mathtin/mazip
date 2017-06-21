#ifndef MAZIPINC_H
#define MAZIPINC_H

#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <vector>

typedef uint32_t uint;
typedef uint16_t TSymbol;
typedef uint8_t byte;
typedef uint8_t bit;
typedef void * ptr;
typedef std::vector<byte> TBuffer;
typedef std::vector<size_t> TCountArray;
typedef std::vector<TSymbol> TCodingTable;

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

static const size_t BYTE_BIT_SIZE = 8;
static const size_t SHORT_BIT_SIZE = 16;
static const size_t UINT_BIT_SIZE = 32;
static const size_t ULONG_BIT_SIZE = 64;

static const size_t S_PAGE = 1 << 12;
static const size_t M_PAGE = 1 << 14;
static const size_t L_PAGE = 1 << 16;

enum {
    FLAG_STDOUT = 1u << 0,
    FLAG_DECOMP = 1u << 1,
    FLAG_LIST = 1u << 2,
    FLAG_TEST = 1u << 3,
    FLAG_RECUR = 1u << 4,
    FLAG_FAST = 1u << 5,
    FLAG_BEST = 1u << 6,
    FLAG_UNKNOWN = 1u << 7
};

const byte MAZIP_HEADER[] = {"\xff\xefMaZIP"};
static const size_t SUBSCRIPTION_LEN = 9;

#endif // ! MAZIPINC_H

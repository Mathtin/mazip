#ifndef MAZIP_H
#define MAZIP_H

#include "ardecoder.h"
#include "arencoder.h"
#include "bwtransform.h"
#include "mafile.h"
#include <stdio.h>

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

extern const char suffix[];

bool Flag(const char * arg);

bool MaZIP(const char * filename);

char * TruncSuffix(char * filename);

char * RetSuffix(char * filename);

char * MakeSuffix(char * filename);

int ProceedFlag(const char & f);

int ProceedFlags(const char * arg, uint & flags);

int CompressTMA(IBuffer & comp, IBuffer & out);

int DecompressTMA(IBuffer & comp, IBuffer & out);

void Compress(char * filename, uint flags);

void Decompress(char * filename, uint flags);

void List(char * filename, uint flags);

void Test(char * filename, uint flags);

#endif // ! MAZIP_H

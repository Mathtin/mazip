#include "mazip.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

const char suffix[] = {".mz"};

static void SubscribeFile(IBuffer & file, uint flags, size_t size) {
    file.push(flags);
    file.push(size);
    file.push(size >> 8);
    file.push(size >> 16);
    file.push(size >> 24);
    file.push(size >> 32);
    file.push(size >> 40);
    file.push(size >> 48);
    file.push(size >> 56);
}

static void GetSubscribtion(IBuffer & file, uint & flags, size_t & size) {
    size_t sz = file.size() - SUBSCRIPTION_LEN;
    flags = file[sz++];
    size = (size_t)file[sz++];
    size |= (size_t)file[sz++] << 8;
    size |= (size_t)file[sz++] << 16;
    size |= (size_t)file[sz++] << 24;
    size |= (size_t)file[sz++] << 32;
    size |= (size_t)file[sz++] << 40;
    size |= (size_t)file[sz++] << 48;
    size |= (size_t)file[sz++] << 56;
}

static void FlushSubscribtion(TMaSTDIn & file, IBuffer & buff) {
    buff.resize(SUBSCRIPTION_LEN * 2);
    size_t i = 0;
    for (; file.size();) {
        buff[i] = file[i];
        i = (i + 1) % SUBSCRIPTION_LEN;
    }
    for (size_t j = i; j; --j) {
        buff[j - 1 + SUBSCRIPTION_LEN] = buff[j - 1];
    }
    for (size_t j = 0; j < SUBSCRIPTION_LEN; ++j) {
        buff[j] = buff[j + i];
    }
    buff.resize(SUBSCRIPTION_LEN);
}

static void PrintList(size_t csize, size_t osize, const char * filename) {
    float ratio = ((double)csize) / ((double)osize) * 100;
    printf(
        "          compressed        uncompressed  ratio uncompressed_name\n");
    printf("%20llu%20llu%6.1f%% %s\n", (unsigned long long int)csize,
           (unsigned long long int)osize, ratio, filename);
}

bool Flag(const char * arg) {
    return arg[0] == '-' && arg[1] != '\0';
}

bool MaZIP(const char * filename) {
    size_t len = strlen(filename);
    return strcmp(filename + len + 1 - sizeof(suffix), suffix) == 0;
}

bool MaZIP(const TMaFile & file) {
    if (file.size() < sizeof(MAZIP_HEADER) + SUBSCRIPTION_LEN) {
        return false;
    }
    for (size_t pos = 0; pos < sizeof(MAZIP_HEADER) - 1; ++pos) {
        if (file[pos] != MAZIP_HEADER[pos]) {
            return false;
        }
    }
    return true;
}

char * TruncSuffix(char * filename) {
    size_t len = strlen(filename);
    filename[len + 1 - sizeof(suffix)] = '\0';
    return filename;
}

char * RetSuffix(char * filename) {
    size_t len = strlen(filename);
    filename[len] = suffix[0];
    return filename;
}

char * MakeSuffix(char * filename) {
    size_t len = strlen(filename);
    char * res = (char *)malloc(len + sizeof(suffix));
    memcpy(res, filename, len);
    memcpy(res + len, suffix, sizeof(suffix));
    return res;
}

int ProceedFlag(const char & f) {
    switch (f) {
    case 'c':
        return FLAG_STDOUT;
    case 'd':
        return FLAG_DECOMP;
    case 'l':
        return FLAG_LIST;
    case '1':
        return FLAG_FAST;
    case 't':
        return FLAG_TEST;
    case 'r':
        return FLAG_RECUR;
    case '9':
        return FLAG_BEST;
    default:
        return FLAG_UNKNOWN;
    }
}

int ProceedFlags(const char * arg, uint & flags) {
    while (*(++arg)) {
        flags |= ProceedFlag(*arg);
        if (flags & FLAG_UNKNOWN) {
            fprintf(stderr, "mazip: invalid option -- '%c'\n", *arg);
            return (int)*arg;
        }
    }
    if ((flags & FLAG_STDOUT) && (flags & FLAG_LIST)) {
        fprintf(stderr, "mazip: incompatible options -- 'c', 'l'\n");
        return 1;
    }
    if ((flags & FLAG_BEST) && (flags & FLAG_FAST)) {
        fprintf(stderr, "mazip: incompatible options -- '1', '9'\n");
        return 1;
    }
    return 0;
}

int CompressTMA(IBuffer & comp, IBuffer & out) {
    int res;
    TArEncoder encoder(out, true);
    encoder.Begin();
    res = encoder.Encode(comp);
    if (res != CODING) {
        return res;
    }
    encoder.Complete();
    return 0;
}

int DecompressTMA(IBuffer & comp, IBuffer & out) {
    int res;
    TArDecoder decoder(comp);
    decoder.Begin();
    res = decoder.Decode(out);
    if (res != CODING && res != COMPLETED) {
        return res;
    }
    decoder.Complete();
    return 0;
}

static void CompressFromSTDIN(uint flags) {
    TMaSTDIn input(FlagToPage(flags));
    BWTransformSTDIN bwtin(input);
    TMaSTDOut output(FlagToPage(flags));
    if (CompressTMA(bwtin, output)) {
        fprintf(stderr, "mazip: std input: failed to compress\n");
        return;
    }
    SubscribeFile(output, flags, input.Count() - 1);
}

static void DecompressFromSTDIN(uint flags) {
    TMaSTDIn input(FlagToPage(flags));
    TMaSTDOut output(FlagToPage(flags));
    BWTransformSTDOUT bwtout(output);
    int res;
    if ((res = DecompressTMA(input, bwtout))) {
        if (res == ERROR) {
            fprintf(stderr, "mazip: std input: unexpected end of file\n");
        } else if (res == NOT_MAZIP) {
            fprintf(stderr, "mazip: std input: not in mazip format\n");
        }
        return;
    }
}

void Compress(char * filename, uint flags) {
    if (MaZIP(filename)) {
        fprintf(stderr, "mazip: %s already has %s suffix -- unchanged\n",
                filename, suffix);
        return;
    } else if (filename[0] == '-' && filename[1] == '\0') {
        CompressFromSTDIN(flags);
        return;
    }
    size_t page = FlagToPage(flags);
    TMaFile input(filename, page);
    if (!input) {
        fprintf(stderr, "mazip: %s: no such file or directory\n", filename);
        return;
    }
    BWTransform bwtin(input);
    if (flags & FLAG_STDOUT) {
        TMaSTDOut output(page);
        if (CompressTMA(bwtin, output)) {
            fprintf(stderr, "mazip: %s: failed to compress\n", filename);
            return;
        }
        SubscribeFile(output, flags, input.size());
    } else {
        char * outname = MakeSuffix(filename);
        TMaFile output(outname, page, true);
        free(outname);
        if (!output) {
            fprintf(stderr, "mazip: %s: failed to create file\n", filename);
            return;
        } else if (CompressTMA(bwtin, output)) {
            fprintf(stderr, "mazip: %s: failed to compress\n", filename);
            return;
        }
        SubscribeFile(output, flags, input.size());
        if (flags & FLAG_LIST) {
            PrintList(output.size(), input.size(), filename);
        }
    }
}

void Decompress(char * filename, uint flags) {
    int res;
    if (filename[0] == '-' && filename[1] == '\0') {
        DecompressFromSTDIN(flags);
        return;
    } else if (!MaZIP(filename)) {
        fprintf(stderr, "mazip: %s: unknown suffix -- ignored\n", filename);
        return;
    }
    TMaFile input(filename, FlagToPage(flags));
    if (!input) {
        fprintf(stderr, "mazip: %s: no such file or directory\n", filename);
        return;
    }
    if (flags & FLAG_STDOUT) {
        TMaSTDOut output(FlagToPage(flags));
        BWTransformSTDOUT bwtout(output);
        if ((res = DecompressTMA(input, bwtout))) {
            if (res == ERROR) {
                fprintf(stderr, "mazip: %s: unexpected end of file\n",
                        RetSuffix(filename));
            } else if (res == NOT_MAZIP) {
                fprintf(stderr, "mazip: %s: not in mazip format\n",
                        RetSuffix(filename));
            }
            return;
        }
    } else {
        TMaFile output(TruncSuffix(filename), FlagToPage(flags), true);
        if (!output) {
            fprintf(stderr, "mazip: %s: failed to create file\n", filename);
            return;
        }
        BWTransform bwtout(output, true);
        if ((res = DecompressTMA(input, bwtout))) {
            if (res == ERROR) {
                fprintf(stderr, "mazip: %s: unexpected end of file\n",
                        RetSuffix(filename));
            } else if (res == NOT_MAZIP) {
                fprintf(stderr, "mazip: %s: not in mazip format\n",
                        RetSuffix(filename));
            }
            return;
        }
        if (flags & FLAG_LIST) {
            PrintList(input.size(), output.size(), filename);
        }
    }
}

void List(char * filename, uint flags) {
    uint iflags;
    size_t sz;
    if (filename[0] == '-' && filename[1] == '\0') {
        TMaSTDIn comp(FlagToPage(flags));
        if (!MaZIP(comp)) {
            fprintf(stderr, "mazip: std input: not in mazip format\n");
            return;
        }
        VectorBuffer sbuff;
        FlushSubscribtion(comp, sbuff);
        GetSubscribtion(sbuff, iflags, sz);
        PrintList(comp.Count() - 1, sz, "STDIN");
        return;
    }
    TMaFile comp(filename, FlagToPage(flags));
    if (!comp) {
        fprintf(stderr, "mazip: %s: no such file or directory\n", filename);
        return;
    }
    if (!MaZIP(comp)) {
        fprintf(stderr, "mazip: %s: not in mazip format\n", filename);
        return;
    }
    GetSubscribtion(comp, iflags, sz);
    TruncSuffix(filename);
    PrintList(comp.size(), sz, filename);
}

static void TestFromSTDIN(uint flags) {
    TMaSTDIn comp(FlagToPage(flags));
    TVoidFileOut out;
    int res;
    {
        BWTransformVOIDOUT bwtout(out);
        if ((res = DecompressTMA(comp, bwtout))) {
            if (res == ERROR) {
                fprintf(stderr, "mazip: std input: unexpected end of file\n");
            } else if (res == NOT_MAZIP) {
                fprintf(stderr, "mazip: std input: not in mazip format\n");
            } else {
                fprintf(stderr, "mazip: std input: unknown result\n");
            }
            return;
        }
    }
    uint iflags;
    size_t sz;
    VectorBuffer sbuff;
    FlushSubscribtion(comp, sbuff);
    GetSubscribtion(sbuff, iflags, sz);
    if (sz != out.Count()) {
        fprintf(stderr, "mazip: std input: unexpected end of file\n");
        return;
    }
    if (flags & FLAG_LIST) {
        PrintList(comp.Count() - 1, sz, "STDIN");
    }
}

void Test(char * filename, uint flags) {
    int res;
    if (filename[0] == '-' && filename[1] == '\0') {
        TestFromSTDIN(flags);
        return;
    }
    TMaFile comp(filename, FlagToPage(flags));
    if (!comp) {
        fprintf(stderr, "mazip: %s: no such file or directory\n", filename);
        return;
    }
    TVoidFileOut out;
    {
        BWTransformVOIDOUT bwtout(out);
        if ((res = DecompressTMA(comp, bwtout))) {
            if (res == ERROR) {
                fprintf(stderr, "mazip: %s: unexpected end of file\n",
                        filename);
            } else if (res == NOT_MAZIP) {
                fprintf(stderr, "mazip: %s: not in mazip format\n", filename);
            }
            return;
        }
    }
    uint iflags;
    size_t sz;
    GetSubscribtion(comp, iflags, sz);
    if (sz != out.Count()) {
        fprintf(stderr, "mazip: %s: unexpected end of file\n", filename);
        return;
    }
    if (flags & FLAG_LIST) {
        PrintList(comp.size(), sz, TruncSuffix(filename));
    }
}

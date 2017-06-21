#include "mazip.h"
#include "platform.h"

#ifndef WINDOWS
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <fcntl.h>
#endif

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

#ifndef WINDOWS
static void copyString(const std::string & input, char * dst, size_t dst_size) {
    strncpy(dst, input.c_str(), dst_size - 1);
    dst[dst_size - 1] = '\0';
}
#endif

static void DoSomething(char * filename, uint flags) {
    if (flags & FLAG_DECOMP) {
        Decompress(filename, flags);
    } else if (flags & FLAG_TEST) {
        Test(filename, flags);
    } else if (flags & FLAG_FAST) {
        Compress(filename, flags);
    } else if (flags & FLAG_BEST) {
        Compress(filename, flags);
    } else if (flags & FLAG_LIST) {
        List(filename, flags);
    } else {
        Compress(filename, flags);
    }
}

int main(int argc, char * argv[]) {
    int res, i;
    uint flags = 0;
#ifndef WINDOWS
    char buffer[256];
#else
    setmode(fileno(stdout), O_BINARY);
    setmode(fileno(stdin), O_BINARY);
#endif
    if (argc < 2) {
        printf("Usage: [ -cdltr19 ] file ... \n");
        return 0;
    }
    for (i = 1; i < argc && Flag(argv[i]); ++i) {
        if ((res = ProceedFlags(argv[i], flags))) {
            return 0;
        }
    }
    for (; i < argc; ++i) {
#ifndef WINDOWS
        if (fs::is_directory(argv[i])) {
            if (flags & FLAG_RECUR) {
                for (auto & p : fs::recursive_directory_iterator(argv[i])) {
                    copyString(p.path(), buffer,
                               p.path().string().length() + 1);
                    DoSomething(buffer, flags);
                }
            } else {
                fprintf(stderr, "mazip: %s is a directory -- ignored\n",
                        argv[i]);
            }
        } else {
#endif
            DoSomething(argv[i], flags);
#ifndef WINDOWS
        }
#endif
    }
    return 0;
}

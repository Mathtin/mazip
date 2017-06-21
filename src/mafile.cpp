#include "mafile.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

TMaFile::TMaFile(const char * filename, size_t p, bool write)
    : page(p)
    , file(NULL)
    , offsetPages(-1)
    , filesize(0)
    , end(false)
    , w(write)
    , state(MA_ERROR) {
    file = fopen(filename, write ? "wb+" : "rb");
    if (file == NULL) {
        return;
    }
    state = MA_OPENED;
    fseek(file, 0L, SEEK_END);
    filesize = ftell(file);
    rewind(file);
}

TMaFile::~TMaFile() {
    if (w && offsetPages != (size_t)-1) {
        SwapOff();
    }
    if (file) {
        fclose(file);
    }
}

void TMaFile::LoadPage(TMaFile * self, size_t p) {
    if (p == self->offsetPages) {
        return;
    } else if (self->w && self->offsetPages != (size_t)-1) {
        self->SwapOff();
    }
    // self->resize(p * page + 1);
    fseek(self->file, p * self->page, SEEK_SET);
    self->offsetPages = p;
    size_t load = self->PageSize();
    load = fread(self->buff, 1, load, self->file);
    fseek(self->file, 0L, SEEK_END);
    self->end = true;
}

void TMaFile::LoadPage(size_t p) const {
    TMaFile::LoadPage((TMaFile *)this, p);
}

void TMaFile::SwapOff() {
    fseek(file, offsetPages * page, SEEK_SET);
    size_t off = PageSize();
    off = fwrite(buff, 1, off, file);
}

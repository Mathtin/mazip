#ifndef MAFILE_H
#define MAFILE_H

#include "mazipinc.h"
#include "mazipint.h"
#include <cstring>
#include <stdio.h>

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

enum { MA_OPENED, MA_ERROR };

class TMaFile : public IBuffer {
  public:
    TMaFile(const char * filename, size_t p) : TMaFile(filename, p, false) {}
    TMaFile(const char * filename, size_t p, bool write);
    ~TMaFile();

    virtual size_t FileSize() const {
        return filesize;
    }

    virtual size_t Page() const {
        return page;
    }

    virtual void SetPage(size_t p) {
        size_t newOff = offsetPages * page / p;
        page = p;
        offsetPages = (size_t)-1;
        LoadPage(newOff);
    }

    virtual bool Opened() {
        return state != MA_ERROR;
    }

    virtual operator bool() {
        return state == MA_OPENED;
    }

    /* IBuffer */
    virtual size_t size() const {
        return filesize;
    }

    virtual void resize(size_t sz) {
        if (filesize > sz) {
            return;
        }
        fseek(file, sz - 1, SEEK_SET);
        fputc('\0', file);
        end = true;
        filesize = sz;
    }

    virtual void push(const byte & b) {
        if (!end) {
            fseek(file, 0L, SEEK_END);
            end = true;
        }
        fputc((const char &)b, file), ++filesize;
    }

    virtual byte * data() {
        return buff;
    }

    virtual const byte * data() const {
        return buff;
    }

    virtual byte & operator[](size_t pos) {
        if (pos / page != offsetPages) {
            LoadPage(pos / page);
        }
        return buff[pos % page];
    }

    virtual const byte & operator[](size_t pos) const {
        if (pos / page != offsetPages) {
            LoadPage(pos / page);
        }
        return buff[pos % page];
    }

    virtual IBuffer & operator=(const IBuffer & o) {
        resize(o.size());
        size_t sz = o.size();
        for (size_t i = 0; i < sz; ++i) {
            (*this)[i] = o[i];
        }
        return *this;
    }

    void LoadPage(size_t) const;
    void SwapOff();

    inline size_t PageSize() const {
        return page <= filesize - offsetPages * page
                   ? page
                   : filesize - offsetPages * page;
    }

  protected:
    size_t page;
    byte buff[L_PAGE];
    FILE * file;
    size_t offsetPages;
    size_t filesize;
    bool end;
    bool w;
    int state;
    static void LoadPage(TMaFile *, size_t);
    TMaFile()
        : page(L_PAGE)
        , file(NULL)
        , offsetPages(-1)
        , filesize(0)
        , w(false)
        , state(MA_OPENED){};
};

class TMaSTDOut : public TMaFile {
  public:
    TMaSTDOut(size_t p) : w(false), count(0) {
        page = p;
    }
    ~TMaSTDOut() {
        if (w) {
            fputc(b, stdout), ++count;
            w = false;
        }
    }

    virtual bool Opened() {
        return true;
    }

    virtual void SetPage(size_t p) {
        page = p;
    }

    /* IBuffer */
    virtual size_t size() const {
        return 0;
    }
    virtual void resize(size_t sz) {
        return;
    }
    virtual void push(const byte & c) {
        if (w) {
            fputc(b, stdout), ++count;
            w = false;
        }
        fputc(c, stdout), ++count;
    }
    virtual byte * data() {
        return buff;
    }
    virtual const byte * data() const {
        return buff;
    }
    virtual byte & operator[](size_t pos) {
        if (w) {
            fputc(b, stdout), ++count;
            w = false;
        }
        w = true;
        return b;
    }
    virtual const byte & operator[](size_t pos) const {
        return b;
    }
    virtual IBuffer & operator=(const IBuffer & o) {
        if (w) {
            fputc(b, stdout), ++count;
            w = false;
        }
        size_t sz = o.size();
        for (size_t i = 0; i < sz; ++i) {
            push(o[i]);
        }
        return *this;
    }

    size_t Count() {
        return count;
    }

  protected:
    bool w;
    byte b;
    size_t count;
};

class TMaSTDIn : public TMaFile {
  public:
    TMaSTDIn(size_t p) : b(0), sz(-1), count(0) {
        next = fgetc(stdin), ++count;
        page = p;
    }
    ~TMaSTDIn() {}

    virtual bool Opened() {
        return true;
    }

    virtual void SetPage(size_t p) {
        page = p;
    }

    /* IBuffer */
    virtual size_t size() const {
        return sz;
    }
    virtual void resize(size_t sz) {
        return;
    }
    virtual void push(const byte & c) {
        return;
    }
    virtual byte * data() {
        return buff;
    }
    virtual const byte * data() const {
        return buff;
    }
    virtual byte & operator[](size_t pos) {
        b = next;
        if (sz) {
            next = fgetc(stdin), ++count;
            sz += !(next ^ sz);
        }
        return b;
    }
    virtual const byte & operator[](size_t pos) const {
        ((TMaSTDIn *)this)->b = next;
        if (sz) {
            ((TMaSTDIn *)this)->next = fgetc(stdin),
                       ++((TMaSTDIn *)this)->count;
            ((TMaSTDIn *)this)->sz += !(next ^ sz);
        }
        return b;
    }
    virtual IBuffer & operator=(const IBuffer & o) {
        return *this;
    }

    size_t Count() {
        return count;
    }

  protected:
    byte b;
    size_t next;
    size_t sz;
    size_t count;
};

class TVoidFileOut : public TMaFile {
  public:
    TVoidFileOut() : count(0) {}
    ~TVoidFileOut() {}
    virtual bool Opened() {
        return true;
    }
    virtual void SetPage(size_t p) {
        page = p;
    }
    /* IBuffer */
    virtual size_t size() const {
        return 0;
    }
    virtual void resize(size_t sz) {
        return;
    }
    virtual void push(const byte & c) {
        ++count;
    }
    virtual byte * data() {
        return NULL;
    }
    virtual const byte * data() const {
        return NULL;
    }
    virtual byte & operator[](size_t pos) {
        ++count;
        return b;
    }
    virtual const byte & operator[](size_t pos) const {
        return b;
    }
    virtual IBuffer & operator=(const IBuffer & o) {
        return *this;
    }
    size_t Count() {
        return count;
    }

  protected:
    size_t count;
    byte b;
};

class TVoidFileIn : public TMaFile {
  public:
    TVoidFileIn() : count(0) {}
    ~TVoidFileIn() {}
    bool Opened() {
        return true;
    }
    /* IBuffer */
    virtual size_t size() const {
        return (size_t)-1;
    }
    virtual void resize(size_t sz) {
        return;
    }
    virtual void push(const byte & c) {
        return;
    }
    virtual byte * data() {
        return NULL;
    }
    virtual const byte * data() const {
        return NULL;
    }
    virtual byte & operator[](size_t pos) {
        ++count;
        return b;
    }
    virtual const byte & operator[](size_t pos) const {
        ++((TVoidFileIn *)this)->count;
        return b;
    }
    virtual IBuffer & operator=(const IBuffer & o) {
        return *this;
    }
    size_t Count() {
        return count;
    }

  protected:
    size_t count;
    byte b;
};

#endif // ! MAFILE_H

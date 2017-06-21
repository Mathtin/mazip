#ifndef BWTRANSFORM_H
#define BWTRANSFORM_H

#include "mafile.h"
#include "mazipinc.h"
#include <cstring>

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

static size_t FlagToPage(byte flags) {
    if (flags & FLAG_BEST) {
        return L_PAGE;
    } else if (flags & FLAG_FAST) {
        return S_PAGE;
    }
    return M_PAGE;
}

class BWTransform : public IBuffer {
  public:
    BWTransform(TMaFile & buff) : BWTransform(buff, false) {}
    BWTransform(TMaFile & buff, bool write);
    ~BWTransform();

    virtual operator bool() {
        return (bool)file;
    }

    /* IBuffer */
    virtual size_t size() const {
        return bwtsize;
    }

    virtual void resize(size_t sz) {
        bwtsize = sz;
        gpos = (bwtsize - 1) % bwtpage;
        sz = ((bwtsize - 1) / bwtpage) * file.Page();
        if ((bwtsize - 1) % bwtpage) {
            sz += (bwtsize - 1) % bwtpage - 2;
        }
        file.resize(sz);
    }

    virtual void push(const byte & b) {
        if (!pageSet) {
            file.SetPage(FlagToPage(buff[0]));
            bwtpage = file.Page() + sizeof(uint16_t);
            pageSet = true;
        }
        if (gpos == bwtpage) {
            LoadPage(offsetPages + 1);
        }
        buff[1 + gpos++] = b, ++grow;
    }

    virtual byte * data() {
        return NULL;
    }

    virtual const byte * data() const {
        return NULL;
    }

    virtual byte & operator[](size_t pos) {
        if (((int32_t)pos - 1) / (int32_t)bwtpage != (int32_t)offsetPages) {
            LoadPage(((int32_t)pos - 1) / (int32_t)bwtpage);
        }
        return buff[((int32_t)pos - 1) % (int32_t)bwtpage + 1];
    }

    virtual const byte & operator[](size_t pos) const {
        return ((BWTransform *)this)->operator[](pos);
    }

    virtual IBuffer & operator=(const IBuffer & o) {
        return *this;
    }

    virtual void LoadPage(size_t);
    virtual void SwapOff();

  protected:
    size_t bwtpage;
    byte buff[L_PAGE * 2 + 3];
    TMaFile & file;
    size_t offsetPages;
    size_t bwtsize;
    size_t grow, gpos;
    bool w;
    bool pageSet;
};

class BWTransformSTDIN : public BWTransform {
  public:
    BWTransformSTDIN(TMaSTDIn & mastdin) : BWTransform(mastdin, false) {
        LoadPage(0);
    }
    ~BWTransformSTDIN(){};

    // IBuffer //
    virtual size_t size() const {
        return bwtsize;
    }
    virtual void resize(size_t sz) {
        return;
    }
    virtual void push(const byte & b) {
        return;
    }

    virtual byte & operator[](size_t pos) {
        b = buff[((int32_t)pos - 1) % (int32_t)bwtpage + 1];
        if (!(--grow)) {
            LoadPage(offsetPages + 1);
        }
        return b;
    }

    virtual const byte & operator[](size_t pos) const {
        return ((BWTransformSTDIN *)this)->operator[](pos);
    }
    virtual IBuffer & operator=(const IBuffer & o) {
        return *this;
    }

    virtual void LoadPage(size_t);

  protected:
    byte b;
};

class BWTransformSTDOUT : public BWTransform {
  public:
    BWTransformSTDOUT(TMaSTDOut & mastdout) : BWTransform(mastdout, false) {
        gpos = 0;
    }
    ~BWTransformSTDOUT() {
        SwapOff();
    }

    // IBuffer //
    virtual size_t size() const {
        return 1;
    }
    virtual void resize(size_t sz) {
        return;
    }

    virtual void push(const byte & b) {
        if (!pageSet) {
            file.SetPage(FlagToPage(buff[0]));
            bwtpage = file.Page() + sizeof(uint16_t);
            pageSet = true;
        }
        if (gpos == bwtpage) {
            LoadPage(offsetPages + 1);
        }
        buff[1 + gpos++] = b;
    }

    virtual byte & operator[](size_t pos) {
        if (((int32_t)pos - 1) / (int32_t)bwtpage != (int32_t)offsetPages) {
            LoadPage(((int32_t)pos - 1) / (int32_t)bwtpage);
        }
        return buff[((int32_t)pos - 1) % (int32_t)bwtpage + 1];
    }

    virtual const byte & operator[](size_t pos) const {
        return ((BWTransformSTDOUT *)this)->operator[](pos);
    }
    virtual IBuffer & operator=(const IBuffer & o) {
        return *this;
    }

    virtual void LoadPage(size_t);
    virtual void SwapOff();
};

class BWTransformVOIDOUT : public BWTransform {
  public:
    BWTransformVOIDOUT(TVoidFileOut & mavoid) : BWTransform(mavoid, false) {
        gpos = 0;
    }
    ~BWTransformVOIDOUT() {
        SwapOff();
    }

    // IBuffer //
    virtual size_t size() const {
        return 1;
    }
    virtual void resize(size_t sz) {
        return;
    }

    virtual void push(const byte & b) {
        if (!pageSet) {
            file.SetPage(FlagToPage(buff[0]));
            bwtpage = file.Page() + sizeof(uint16_t);
            pageSet = true;
        }
        if (gpos == bwtpage) {
            LoadPage(offsetPages + 1);
        }
        buff[1 + gpos++] = b;
    }

    virtual byte & operator[](size_t pos) {
        if (((int32_t)pos - 1) / (int32_t)bwtpage != (int32_t)offsetPages) {
            LoadPage(((int32_t)pos - 1) / (int32_t)bwtpage);
        }
        return buff[((int32_t)pos - 1) % (int32_t)bwtpage + 1];
    }

    virtual const byte & operator[](size_t pos) const {
        return ((BWTransformVOIDOUT *)this)->operator[](pos);
    }
    virtual IBuffer & operator=(const IBuffer & o) {
        return *this;
    }

    virtual void LoadPage(size_t p) {
        if (p == offsetPages) {
            return;
        } else if (offsetPages != (size_t)-1) {
            SwapOff();
        }
        offsetPages = p;
    }

    virtual void SwapOff() {
        if (gpos < 2) {
            return;
        }
        gpos -= 2;
        for (size_t i = 0; i < gpos; ++i) {
            file[i] = 0;
        }
        gpos = 0;
    }
};

#endif // ! BWTRANSFORM_H

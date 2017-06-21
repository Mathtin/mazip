#ifndef MAZIPINT_H
#define MAZIPINT_H

#include "mazipinc.h"

/*
 *  Author: Daniil [Mathtin] Shigapov
 *  Copyright (c) 2017 Mathtin <wdaniil@mail.ru>
 *  This file is released under the MIT license.
 */

class IBuffer {
  public:
    virtual ~IBuffer(){};
    virtual size_t size() const = 0;
    virtual void resize(size_t) = 0;
    virtual void push(const byte &) = 0;
    virtual byte * data() = 0;
    virtual const byte * data() const = 0;
    virtual byte & operator[](size_t) = 0;
    virtual const byte & operator[](size_t) const = 0;
    virtual IBuffer & operator=(const IBuffer & o) {
        resize(o.size());
        size_t sz = o.size();
        for (size_t i = 0; i < sz; ++i) {
            (*this)[i] = o[i];
        }
        return *this;
    }
    virtual operator bool() {
        return true;
    }
};

class VectorBuffer : public std::vector<byte>, public IBuffer {
  public:
    VectorBuffer(size_t sz) : std::vector<byte>(sz){};
    VectorBuffer(){};
    virtual ~VectorBuffer(){};
    virtual size_t size() const {
        return std::vector<byte>::size();
    }
    virtual void resize(size_t sz) {
        std::vector<byte>::resize(sz);
    }
    virtual void push(const byte & b) {
        std::vector<byte>::push_back(b);
    }
    virtual byte * data() {
        return std::vector<byte>::data();
    }
    virtual const byte * data() const {
        return std::vector<byte>::data();
    }
    virtual byte & operator[](size_t s) {
        return std::vector<byte>::operator[](s);
    }
    virtual const byte & operator[](size_t s) const {
        return std::vector<byte>::operator[](s);
    }
    virtual IBuffer & operator=(const IBuffer & o) {
        resize(o.size());
        size_t sz = o.size();
        for (size_t i = 0; i < sz; ++i) {
            (*this)[i] = o[i];
        }
        return *this;
    }

  protected:
};

#endif // ! MAZIPINT_H

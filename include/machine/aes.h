// EPOS AES Mediator Common Package

#ifndef __aes_h
#define __aes_h

#include <system/config.h>

__BEGIN_SYS

class AES_Common
{
protected:
    AES_Common() {}
};

__END_SYS

#ifdef __AES_H
#include __AES_H

__BEGIN_SYS

template<unsigned int KEY_SIZE>
using AES = _SYS::_AES<KEY_SIZE>;

__END_SYS

#else

#include <utility/aes.h>

__BEGIN_SYS

template<unsigned int KEY_SIZE>
using AES = _UTIL::_AES<KEY_SIZE>;

__END_SYS

#endif

#endif

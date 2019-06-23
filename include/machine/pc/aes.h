// EPOS PC AES Mediator Declarations

#ifndef __pc_aes_h
#define __pc_aes_h

#include <machine/aes.h>


//TODO: this is just a place holder. Replace with x86 AES!
#include <utility/aes.h>

__BEGIN_SYS

template<unsigned int KEY_SIZE>
class _AES: private AES_Common, public _UTIL::_AES<KEY_SIZE> {};

__END_SYS

#endif

// EPOS Conversion Utility Declarations

#ifndef __convert_h
#define __convert_h

#include <system/config.h>

__BEGIN_UTIL

namespace Convert
{

template<typename Count, typename Time, typename Temporary = typename LARGER<Time>::Result>
inline Count s2count(const Count f, const Time & t) { return static_cast<Temporary>(t) * f / 1; }
template<typename Count, typename Time, typename Temporary = typename LARGER<Time>::Result>
inline Count ms2count(const Count f, const Time & t) { return static_cast<Temporary>(t) * f / 1000; }
template<typename Count, typename Time, typename Temporary = typename LARGER<Time>::Result>
inline Count us2count(const Count f, const Time & t) { return static_cast<Temporary>(t) * f / 1000000; }

template<typename Count, typename Time, typename Temporary = typename LARGER<Time>::Result>
inline Time count2s(const Count f, const Count & c) { return static_cast<Temporary>(c) * 1 / f; }
template<typename Count, typename Time, typename Temporary = typename LARGER<Time>::Result>
inline Time count2ms(const Count f, const Count & c) { return static_cast<Temporary>(c) * 1000 / f; }
template<typename Count, typename Time, typename Temporary = typename LARGER<Time>::Result>
inline Time count2us(const Count f, const Count & c) { return static_cast<Temporary>(c) * 1000000 / f; }

};

__END_UTIL

#endif

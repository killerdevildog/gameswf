// tu_types.h	-- Ignacio CastaÒo, Thatcher Ulrich 2003

// This source code has been donated to the Public Domain.  Do
// whatever you want with it.

// Minimal typedefs.  Modernized to use C++11 standard types.


#ifndef TU_TYPES_H
#define TU_TYPES_H


#include "base/tu_config.h"
#include <stdio.h>
#include <cstdint>


#if defined(__i386__) || defined(_WIN32) || defined(__GNUC__) || defined(__x86_64__)
	// On known little-endian platforms, define this stuff.
	#define _TU_LITTLE_ENDIAN_	1
#else
	#undef _TU_LITTLE_ENDIAN_
#endif

// Use standard C++ fixed-width integer types
// These are compatible with SDL2's types (both use <cstdint>)
typedef uint8_t  Uint8;
typedef int8_t   Sint8;
typedef uint16_t Uint16;
typedef int16_t  Sint16;
typedef uint32_t Uint32;
typedef int32_t  Sint32;
typedef uint64_t Uint64;
typedef int64_t  Sint64;


typedef Uint8 uint8;
typedef Sint8 sint8;
typedef Sint8 int8;
typedef Uint16 uint16;
typedef Sint16 sint16;
typedef Sint16 int16;
typedef Uint32 uint32;
typedef Sint32 sint32;
typedef Sint32 int32;
typedef Uint64 uint64;
typedef Sint64 sint64;
typedef Sint64 int64;

#if TU_USES_FLOAT_AS_COORDINATE_COMPONENT
	typedef float coord_component;
#else
	typedef Sint16 coord_component;
#endif


// A function to run some validation checks.
#ifdef _WIN32
__declspec(dllexport) bool	tu_types_validate();
#else // _WIN32
bool	tu_types_validate();
#endif


#endif // TU_TYPES_H


// Local Variables:
// mode: C++
// c-basic-offset: 8 
// tab-width: 8
// indent-tabs-mode: t
// End:

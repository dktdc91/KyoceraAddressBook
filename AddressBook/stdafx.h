// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

#define ADDRESS_BOOK_LIBRARY
#define callback __stdcall

#if defined ADDRESS_BOOK_LIBRARY
#  define ADDRESS_BOOK_EXPORT _declspec(dllexport)
#else
#  define ADDRESS_BOOK_EXPORT _declspec(dllimport)
#endif

// TODO: reference additional headers your program requires here

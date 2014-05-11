#ifndef __COMPILE_DEFS_H__
#define __COMPILE_DEFS_H__

#define SHAREWARE 1
#define BEGROOVY 2
#define CUSTOM 3
#define CUSTOM_MESSAGE "The rumors of my demise have\nbeen greatly exagerated"

#define BUILD CUSTOM //SHAREWARE
//#define MULTI_TILE	// multiple tile support

//#define USE_YLANGUAGE

#ifdef USE_YLANGUAGE
#include "YLanguageClass.h"
inline const char *localize(char *x)
{
const char *cp;
	cp = Language.get(x);
	if (cp == x) return "?????"; 
	return cp;
}

#define _(x,y) localize(x)
#else
#define _(x,y) y
#endif

#endif
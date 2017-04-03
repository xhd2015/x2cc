/*
 * test_preprocessors.cpp
 *
 *  Created on: 2017?3?26?
 *      Author: 13774
 */

#define A B
#define V Z
#define STR()	__STR
#define STR1(int) STR() int

#define _STRING(x) #x
#define STRING(x) _STRING(X)

#define CAT(A,B) A##B


int CAT(m,ain)(int v)
{
	printf(STRING(CAT(## is not ##, what\, is ##\", vd))); //we allow \" in macros : \,\\. explicitily speaking,we allow every thing
}


#include "test.h"

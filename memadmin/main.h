#pragma once
#ifndef	__main_h__
#define	__main_h__	2.1
//#ident	"@(#)main.h	2.1	AKK	20120725"

/** @file main.h
 *  @brief The main function where the program starts
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 2.1	2012/07/25
 */

// ------------------------------------------------------
// Stuff that is needed in several places
// Collected here for convenience
// ------------------------------------------------------

// STL includes
#include <list>			// de STL std::list<> container
#include <iostream>		// de STL std::cin, std::cout en std::cerr
using namespace std;

// andere eigen includes
#include "asserts.h"	// require(...), notreached(), etc
#include "ansi.h"		// ansi color codes
#include "Area.h"		// class Area
#include "Allocator.h"	// baseclass Allocator


// Even wat handige afkortingen maken ...
typedef	std::list<Area*>	AreaList;		///< Een "Arealist container"
typedef	AreaList::iterator	ALiterator;		///< Een "AreaList container Iterator"


#endif	/*main_h*/
// vim:sw=4:ai:aw:ts=4:

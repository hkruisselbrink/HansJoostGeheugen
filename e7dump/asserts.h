#pragma once
#ifndef	__asserts_h__
#define	__asserts_h__ 3.1
//#ident "@(#)asserts.h	3.1	AKK	2013/11/16"

/** @file asserts.h
 * This file defines various macro's that can be used as checks in your program.
 * <br>If NDEBUG has been defined the assertions will be totally disabled for that
 * translation unit.
 * @note It relies on the __FILE__, __LINE__ and __PRETTY_FUNCTION__
 *			compiler magic names to actually identify lines of source code.
 * <br>If your compiler does not provide the __PRETTY_FUNCTION__ magic name,
 * 		you can \#define it as __FUNCTION__ (gcc) or __func__ (any compiler).
 * 		You can also \#define it as 0 and the 'assert_error' constructor will
 * 		then ignore that parameter.
 *
 * @author R.A.Akkersdijk@saxion.nl
 * @version 3.1	2013/11/16
 */


// First remove these macro's
#undef	require
#undef	check
#undef	ensure
#undef	notreached


#ifdef	NDEBUG	/* disable asserts? */

// Make sure that another #include "asserts.h" can be used
// to enable assertions again later.
# undef __asserts_h__

// Define some dummy macro's that have no side-effects
# define require(expr)	((void)(0))
# define check(expr)	((void)(0))
# define ensure(expr)	((void)(0))
# define notreached()	((void)(0))


#else /* asserts enabled */


#include "assert_error.h"

/**
 * Verify whether a precondition is met, for instance: require(x>0);
 */
# define	require(condition) \
	if(!(condition)) \
		throw assert_error(__FILE__,__LINE__,__PRETTY_FUNCTION__,"require",#condition)

/**
 * Verify an internal consistenty check, for instance: check(x>0);
 */
# define	check(condition) \
	if(!(condition)) \
		throw assert_error(__FILE__,__LINE__,__PRETTY_FUNCTION__,"check",#condition)

/**
 * Verify whether a postcondition is met, for instance: ensure(x>0);
 */
# define	ensure(condition) \
	if(!(condition)) \
		throw assert_error(__FILE__,__LINE__,__PRETTY_FUNCTION__,"ensure",#condition)

/**
 * Verify that this line of code is not reached. For instance
 * when the 'default' case of a 'switch' should never be reached
 * you can say:
 * 		default: notreached();
 */
# define	notreached() \
	throw assert_error(__FILE__,__LINE__,__PRETTY_FUNCTION__,"notreached","")

#endif	/* not NDEBUG */

#endif	/*asserts_h*/
// vim:sw=4:ai:aw:ts=4:

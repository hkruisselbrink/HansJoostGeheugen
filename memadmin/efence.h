#pragma once
#ifndef	efence_h
#define	efence_h
//#ident "@(#)efence.h	1.0	AKK	20080925"


/** @file efence.h
 * Declarations of efence externals.
 *
 * <a target='info' href='http://en.wikipedia.org/wiki/Electric_Fence'>efence</a> - Electric Fence Malloc Debugger
 *
 * Electric Fence helps you detect two common programming bugs:
 * Software that overruns the boundaries of a malloc() memory
 * allocation, and software that touches a memory allocation
 * that has been released by free().
 *
 * Link your program with: -lefence
 *
 * Optionally set some of the variables (or use environment variables)
 * to modify the behaviour.
 *
 * @note
 * If your platform does not provide 'efence'
 * there are various alternatives to do something similar, e.g.
 * '<a target='info' href='http://en.wikipedia.org/wiki/Valgrind'>valgrind</a>'.
 *
 */


// efence config variables
extern	int	EF_ALIGNMENT;		//!< check for alignment on ... (default = sizeof(int) )
extern	int	EF_PROTECT_BELOW;	//!< protect below allocated area (default = 0 [above])
extern	int	EF_PROTECT_FREE;	//!< inhibit re-use of freed memory (default = 0 [allow])
extern	int	EF_ALLOW_MALLOC_0;	//!< allow malloc(0) (default = 0 [deny])
extern	int	EF_FILL;			//!< pre-fill free memory with ...
								//!< Special case: -1 means: global memory = 0, dynamic memory = garbage

#endif	/*efence_h*/
// vim:sw=4:ai:aw:ts=4:

#ifndef	_ENDIAN_H
#define	_ENDIAN_H	1
// Code borrowed from Fedora 16 gcc 4.6.3

/** @file endian.h
 * An endian.h file for platforms that do not already provided it.
 * This is mostly likely needed for windows/mingw.
 */

 /* Definitions for byte order, according to significance of bytes,
   from low addresses to high addresses.  The value is what you get by
   putting '4' in the most significant byte, '3' in the second most
   significant byte, '2' in the second least significant byte, and '1'
   in the least significant byte, and then writing down one digit for
   each byte, starting with the byte at the lowest address at the left,
   and proceeding to the byte with the highest address at the right.  */

#define	__LITTLE_ENDIAN	1234
#define	__BIG_ENDIAN	4321
#define	__PDP_ENDIAN	3412

// Windows runs on an intel/amd processor which is little endian
#define __BYTE_ORDER __LITTLE_ENDIAN

#endif	/*_ENDIAN_H*/

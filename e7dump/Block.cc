/** @file
 * Implementation of class Block.
 */
#include "asserts.h"	// asserts checking

// Determine the byte-order of this platform.
// Normaly this would be an <endian.h> system include file,
// but when your platform does not provide it you can find
// a replacement endian.h file in the 'diversen' directory.
#include "endian.h"
#ifndef	__BYTE_ORDER
# error	Oops, the byte order of your machine is undefined
#endif


#include "Block.h"


// return an 8 bit byte starting at the given offset
byte	Block::getbyte(off_x offset) const
{
	require((0 <= offset) && (offset < DBLKSIZ));
	return rawdata[offset];
}


// return a 16 bit short int starting at the given offset
short	Block::getshort(off_x offset) const
{
	require((0 <= offset) && ((offset + 1) < DBLKSIZ));
	byte  b1 = rawdata[   offset ];
	byte  b2 = rawdata[ ++offset ];
#if	__BYTE_ORDER == __LITTLE_ENDIAN
	/* big-endian => little-endian (PC) */
	return short((b1 << 8) + b2);
#else
	/* big-endian => big-endian */
	return short((b2 << 8) + b1);
#endif
}


// return a 32 bit long int starting at the given offset
long	Block::getlong(off_x offset) const
{
	require((0 <= offset) && ((offset + 3) < DBLKSIZ));
	byte  b1 = rawdata[   offset ];
	byte  b2 = rawdata[ ++offset ];
	byte  b3 = rawdata[ ++offset ];
	byte  b4 = rawdata[ ++offset ];
#if	__BYTE_ORDER == __LITTLE_ENDIAN
	/* big-endian => little-endian (PC) */
	return long((b1 << 24) + (b2 << 16) + (b3 << 8) + b4);
#else
	/* big-endian => big-endian */
	return long((b4 << 24) + (b3 << 16) + (b2 << 8) + b1);
#endif
}


// get a sequence of 'count' ascii characters as a string starting at the given offset
std::string		Block::getstring(off_x offset, int count) const
{
	std::string	 s;
	for (int  i = 0 ; (i < count) && (rawdata[offset] != 0) ; ++i, ++offset) {
		require((0 <= offset) && (offset < DBLKSIZ));
		s += ((char)(rawdata[offset]));
	}
	return s;
}


// copy a sequence of 'count' ascii characters starting at the given offset
void		Block::getchars(char *dest, off_x offset, int count) const
{
	for (int  i = 0 ; (i < count) ; ++i, ++dest, ++offset) {
		require((0 <= offset) && (offset < DBLKSIZ));
		*dest = ((char)(rawdata[offset]));
	}
}


/*
 * Converts 3-byte disk addresses to array of longs.
 *	offset = position in the block to start
 *	lp = array of 13 longs, the destination
 */
void	Block::l3tol(off_x offset, long lp[]) const
{
	// the 40 bytes used must be within the block
	require((0 <= offset) && ((offset + 40) < DBLKSIZ));

	byte  *a = (byte *) lp;
	byte  *b = (byte *) &rawdata[offset];
	for (int  i = 0 ; i < 13 ; ++i) {
#if	__BYTE_ORDER == __LITTLE_ENDIAN
		/* big-endian => little-endian (PC) */
		a[3] = 0;
		a[2] = b[0];	// least
		a[1] = b[1];
		a[0] = b[2];	// most
		a += 4;
		b += 3;
#else
		/* big-endian => big-endian */
		*a++ = 0;
		*a++ = *b++;	// least
		*a++ = *b++;
		*a++ = *b++;	// most
#endif
	}
}


// Decrement the usage counter
void	Block::release()
{
	require(usage_counter > 0);
	--usage_counter;
}


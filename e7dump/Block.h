#ifndef BLOCK_H
#define BLOCK_H 2.1

/** @file Block.h
 * Definition of class Block.
 */

#include <string>	// for: std::string

// Unix 7th Edition definitions
#include "e7types.h"
#include "e7const.h"


/** @class Block
 * Represents a single datablock from the filesystem device.
 * It contains DBLKSIZ (=512) bytes of raw binairy data.
 */
class	Block
{
	friend class Device;
	// Grants unlimited access rights to class Device.

private:

	// You are not allowed to create or delete Blocks yourself!
	Block() : usage_counter(0) {}

	virtual ~Block() {}
	// ... but class Device can do this because it is a friend.

protected:

	/** The raw data as read from disk */
	byte	rawdata[ DBLKSIZ ];

	/*
	 * You could create derived classes that need access to the raw data of Block.
	 * As long as those classes do NOT add attributes but only non-virtual methods
	 * you can safely static_cast<>() a Block pointer to a your_type pointer.
	 *
	 * 		class MyBlock : public Block {
	 *			only_methods_here
	 * 		};
	 *
	 * 		MyBlock  *mp = static_cast<MyBlock*>( Device.getBlock(...) );
	 *
	 * NOTE: You can not use dynamic_cast<>() because a Block instance
	 * 		 can never truely be a MyBlock instance.
	 *
	 * NOTE: You may not add virtual methods because C++
	 *		 then adds "secret" attribute(s) to the object
	 *		 in which case any form of casting could create
	 *		 an bogus pointer!
	 */

public:

	/** Get an 8 bit byte from this block.
	 * @param[in]	offset	the byte offset within this block where data starts
	 * @pre the data requested must be within the block
	 */
	byte		getbyte(off_x offset)	const;

	/** Get a 16 bit short int from this block.
	 * @param[in]	offset	the byte offset within this block where data starts
	 * @pre the data requested must be within the block
	 */
	short		getshort(off_x offset)	const;

	/** Get a 32 bit long int from this block.
	 * @param[in]	offset	the byte offset within this block where data starts
	 * @pre the data requested must be within the block
	 */
	long		getlong(off_x offset)	const;

	/** Get a sequence of at most 'count' ascii characters as a string.
	 * @param[in]	offset	the byte offset within this block where data starts
	 * @param[in]	count	the number of bytes
	 * @pre the data requested must be within the block
	 */
	std::string	getstring(off_x offset, int count)	const;

	/** Copy a sequence of at most 'count' ascii characters to the given destination.
	 * @param[in]	dest	the destination address
	 * @param[in]	offset	the byte offset within this block where data starts
	 * @param[in]	count	the number of bytes
	 * @pre the data requested must be within the block
	 */
	void		getchars(char *dest, off_x offset, int count)	const;

	/**
	 * Converts 13, 3-byte, disk addresses to an array of longs.
	 * @param[in]	offset	the position in the block where to start reading
	 * @param[out]	dest	a, caller provided, array of 13 longs, the destination
	 * @pre the data requested must be within the block
	 */
	void		l3tol(off_x offset, long dest[])	const;

protected:

	/** Block usage counter.
	 * Keeps track of how often this block is being used at the same time.
	 * Incremented by Device.getBlock() when you ask for a Block.
	 * Decremented by calling Block.release().
	 * Used by Device.getBlock() for a block replacement algoritm
	 * when we limit the size of the block cache.
	 */
	ulong	usage_counter;

public:

	/** Decrements the usage_counter.
	 * @pre The usage_counter must be > 0
	 */
	void	release();
};

#endif // BLOCK_H

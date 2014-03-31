#ifndef E7FILSYS_H
#define E7FILSYS_H 1.2.2

/* @(#)e7filsys.h	1.2.2 */

/** @file
 * Definitions about the super block.
 */

#include "e7types.h"

#define	SUPERB	((daddr_x)1)	/*!< the block number of the super block */
#define	NICFREE	(50)			/*!< the number of free block number slots in the super block */
#define	NICINOD	(100)			/*!< the number of free inode number slots in the super block */


/** @struct filsys
 * Structure of (the first part of) the super block
 * as in appears on disk.
 */
 #pragma pack(1)
struct	filsys
{
	ushort		s_isize;			/*!< size in blocks of the i-list area */
	/*								//  (including the boot block and super block!) */
	daddr_x		s_fsize;			/*!< size in blocks of the entire volume */
	ushort		s_nfree;			/*!< current number of block numbers in s_free */
	daddr_x		s_free[NICFREE];	/*!< free block numbers list (used as a stack) */
	ushort		s_ninode;			/*!< the current number of i-nodes in s_inode */
	ino_x		s_inode[NICINOD];	/*!< free i-node list (used as a stack) */
	byte		s_flock;			/*!< lock flag during free list manipulation */
	byte		s_ilock;			/*!< lock flag during i-list manipulation */
	byte		s_fmod;				/*!< super_block_was_modified flag */
	byte		s_ronly;			/*!< mounted_read-only flag */
	time_x		s_time;				/*!< last super block update timestamp */
	daddr_x		s_tfree;			/*!< total number of free blocks */
	ino_x		s_tinode;			/*!< total number of free inodes */
	short		s_m;				/*!< device optimal interleave factor */
	short		s_n;				/*!< device cylinder size */
	char		s_fname[6];			/*!< file system name */
	char		s_fpack[6];			/*!< file system pack name */
	/*								// total number of bytes used = 440 */
}  __attribute__((__packed__));	// This prevents the compiler from adding padding-bytes between the fields
#pragma pack()


#endif /*E7FILSYS_H*/

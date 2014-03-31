#ifndef E7INO_H
#define E7INO_H 1.1

/** @file
 * Definitions about inodes as they appear on disk.
 */

#include "e7types.h"

/* @(#)e7ino.h	1.1 */

/** @struct dinode
 * Inode structure as it appears in a disk block.
 */
struct dinode
{
	imode_x	di_mode;     	/*!< mode and type of file (see below) */
	/*						// will be 0 when unused (other fields may then contain garbage) */
	short	di_nlink;    	/*!< number of links to this file */
	id_x	di_uid;      	/*!< owner's user id */
	id_x	di_gid;      	/*!< inode's group id */
	off_x	di_size;     	/*!< number of bytes in the file unless it is a special file */
	byte  	di_addr[40];	/*!< disk block addresses (but see below) */
	time_x	di_atime;   	/*!< time file contents were last accessed */
	time_x	di_mtime;   	/*!< time file contents were last modified */
	time_x	di_ctime;   	/*!< time inode created/changed */
};


#define	ROOTINO	((ino_x)2)	/*!< the inode number of the root directory */
/* Note: the first physical inode has number 1 !	        */
/* Inode 1 is the bad-block repository, disguised as a file */
/* Inode 2 is the ROOT directory of the filesystem          */


/* Some handy constants */
#define	INSIZ	(64)	/*!< the size of a dinode in bytes		*/
#define	INOPB	(8)		/*!< there are 8 dinodes per disk block	*/


/* Handy convertion macro's */

/*! convert an inumber to a disk address */
#define	itod(x)	(daddr_x)(((uint)((x)+15)>>3))

/*! convert an inumber to an dinode offset within a block */
#define	itoo(x)	(int)(((x)+15)&07)


/* the encoding of the inode type in di_mode (upper 4 of 16 bits) */
#define	X_IFMT	0170000		/*!< type of file (a mask for the upper 4 bits) */
#define		X_IFIFO 0010000 /*!< FIFO special file */
#define		X_IFCHR	0020000	/*!< character special */
#define		X_IFMPC	0030000	/*!< multiplexed char special */
#define		X_IFDIR	0040000	/*!< directory */
#define		X_IFBLK	0060000	/*!< block special */
#define		X_IFMPB	0070000	/*!< multiplexed block special */
#define		X_IFREG	0100000	/*!< regular data file */
	/* Example:
	 * To determine whether some inode represents a directory use:
	 * 		((inode.di_mode & X_IFMT) == X_IFDIR)
	 */

/* other flags in di_mode (lower 12 bits of 16) */
#define	X_ISUID		0004000	/*!< set user id on execution */
#define	X_ISGID		0002000	/*!< set group id on execution */
#define	X_ISVTX		0001000	/*!< save swapped text even after use */

#define	X_IUREAD	0000400	/*!< read permission, owner */
#define	X_IUWRITE	0000200	/*!< write permission, owner */
#define	X_IUEXEC	0000100	/*!< execute/search permission, owner */

#define	X_IGREAD	0000040	/*!< read permission, group */
#define	X_IGWRITE	0000020	/*!< write permission, group */
#define	X_IGEXEC	0000010	/*!< execute/search permission, group */

#define	X_IOREAD	0000004	/*!< read permission, other */
#define	X_IOWRITE	0000002	/*!< write permission, other */
#define	X_IOEXEC	0000001	/*!< execute/search permission, other */


/*
 * the 40 address bytes in di_addr:
 * 39 bytes are used for 13 disk addresses of 3 bytes each.
 *			 plus one byte for padding.
 *
 * data layout:
 *	addr[0] = most-significant-byte   of daddr[0]
 *	addr[1] = middle-significant-byte of daddr[0]
 *	addr[2] = least-significant-byte  of daddr[0]
 *
 *	addr[3] = most-significant-byte   of daddr[1]
 *	addr[4] = middle-significant-byte of daddr[1]
 *	addr[5] = least-significant-byte  of daddr[1]
 *
 *	etc
 */

#ifndef NADDR
# define NADDR	13	/*!< number of disk addresses in inode */
#endif


/* Info about indirection blocks */
#define NINDIR	(128)	/*!< 128 indirection entries per block */
						/* i.e. 512 bytes per block / 4 bytes per disk address */
#define	NMASK	(0177)	/*!< i.e. (NINDIR-1) */
#define	NSHIFT	(7)		/*!< i.e. LOG2(NINDIR) */




// ========================================================
// The remainder of this file is only relevant when dealing
// with a filesystem that has inodes for special files
// ========================================================

/*
 * Device numbers (addr[1],addr[2]) iff inode is a device
 */

/* given a dev_x, split it into the major/minor device code */
#define	X_major(x)  	(byte)((ushort)(x)>>8)
#define	X_minor(x)  	(byte)((x)&0377)

/* compose a dev_x from major/minor device code */
#define	X_makedev(x,y)	(dev_t)(((x)<<8)|(y))


#endif /*E7INO_H*/

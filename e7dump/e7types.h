#ifndef E7TYPES_H
#define E7TYPES_H 1.2.3

/* @(#)e7types.h	1.2.3 2006/04/03 */

/** @file
 * Supplies the platform dependent mappings from logical to physical types.
 * You may have to alter these if you have some esotoric computer.
 */
typedef unsigned char	byte;	/*!< 1 byte : general type */
typedef unsigned short	ushort;	/*!< 2 byte : general type */
typedef unsigned int	uint;	/*!< 4 byte : general type */
typedef unsigned long	ulong;	/*!< 4 byte : general type */

// Normaliter (conventie) eindigen alle typenamen op _t
// maar op linux geeft dat conflicten omdat soortgelijke
// pseudo-types ook al op systeem niveau gedefinieerd zijn.
// Dus gebruiken we hier onze eigen ..._x variant.
typedef long	daddr_x;  	/*!< 4 byte : a disk address */
							// NOTE:
							//	In an inode_on_disk
							// 	these numbers are encoded as 24 bit quantities.
							// 	Anywhere else they are 32 bit daddr_x entities.
typedef ushort	ino_x;     	/*!< 2 byte : an i-node number */
typedef long	time_x;   	/*!< 4 byte : a timestamp in seconds */
typedef ushort	dev_x;		/*!< 2 byte : a device code */
typedef long	off_x;    	/*!< 4 byte : an offset in a file */
typedef ushort	id_x;		/*!< 2 byte : an identification type */
typedef ushort	imode_x;	/*!< 2 byte : the type of the i_mode field */


#endif /*E7TYPES_H*/

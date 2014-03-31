// -----------------------------------------------------
//
// Chart the usage of blocks on a 7th edition filesystem
//
// NOTE: First look at "1.voorbeeld.cc" !
//
// -----------------------------------------------------
// Our own includes
#include "ansi.h"		// for: ansi color code strings
#include "asserts.h"	// for: notreached()

// C/C++/STL specific
#include <ctime>		// for: ctime()
#include <cstdio>	    // for: printf()
#include <cstddef>		// for: offsetof(type,member)
#include <cstdlib>		// for: EXIT_SUCCESS, EXIT_FAILURE
#include <iostream>	    // for: std::cout
using namespace std;

// The include files for the unix 7-th Edition Filesystem
#include "e7filsys.h"	// the description of the information in the superblock
#include "e7fblk.h"		// a "free-block" description
#include "e7ino.h"	    // the description of the on-disk version of an inode

// Our own classes
#include "Device.h"		// the "device driver"
#include "Block.h"		// the "data blocks"


// ================================================================
// To register what blocks are used for ...


/** Codes for blocktypes */
enum BlkUse
{
	B_UNKNOWN,								// default status
	B_BOOT, B_SUPER, B_INODE,				// covers the first part of the medium
	B_DATA, B_INDIR1, B_INDIR2, B_INDIR3,	// known used blocks in the data area
	B_FREE									// known unused blocks in the data area
};


// A table to keep track of what a block is used for
map<daddr_x,BlkUse>  usage;


// To register what a block is used for
void	useBlock(daddr_x bno, BlkUse use)
{
	if (usage[bno] == B_UNKNOWN) {		// we have learned something
		usage[bno] = use;
	} else
	if (usage[bno] != use) {			// changing usage to something else ?
		printf(AC_BROWN"\nUsage: %ld was %d, now %d?\n"AA_RESET, bno, usage[bno], use);
	}
}


// Report the usage of all blocks
void	reportUsage()
{
	char  codes[] = "?BSID123.";

	printf("BlockUsage: %d blocks charted\n", usage.size());

	printf(AC_BLUE);

	daddr_x	  n = 0;
	map<daddr_x,BlkUse>::iterator  i;
	// Note: Iterating thru a c++ map goes in ascendig key order
	for (i = usage.begin(); i != usage.end(); ++i, ++n) {
		if (i->first != n) {
			n = i->first;
			printf("Map starts at block %ld\n", n);
		}
		printf("%c", codes[i->second]);
	}

	printf(AA_RESET "\n");
}


// ================================================================
// The code to examine the disk


// Read super-block data from the device into 'fs'.
// We only need: s_isize, s_nfree and s_free[]
// @param		device	The device "driver"
// @param[out]	fs		The filsys struct to be partially filled.
void	readSuperblock(Device& device, filsys& fs)
{
	// Fetch the block containing the super-block
	Block  *sp = device.getBlock(SUPERB);
	useBlock(SUPERB, B_SUPER);	// register it's role

	// Convert some of the data to the "native" type

	// Use this for a title  ...
	string  s_fname = sp->getstring(offsetof(filsys, s_fname), 6);
	string  s_fpack = sp->getstring(offsetof(filsys, s_fpack), 6);
	printf("Examine: s_fname='%.6s', s_fpack='%.6s', ", s_fname.c_str(), s_fpack.c_str());

	// Get the filesystem parameters
	fs.s_isize = sp->getshort(offsetof(filsys, s_isize));	// inode space (blocks)
	fs.s_fsize = sp->getlong(offsetof(filsys, s_fsize));	// total space (blocks)
	printf("s_isize=%d s_fsize=%ld\n", fs.s_isize, fs.s_fsize);

	// Limit the range of further allowed device I/O
	device.setLimit( fs.s_fsize );

	// Get the data about free blocks
	fs.s_nfree = sp->getshort(offsetof(filsys, s_nfree));
	//printf("s_nfree=%d:", fs.s_nfree);//DEBUG

	// Get the free_list table
	off_x  offset = offsetof(filsys, s_free);	// where the table begins
	for (int  i = 0; i < NICFREE; ++i) {
		fs.s_free[i] = sp->getlong(offset);
		offset += sizeof(daddr_x);
		//printf(" %ld", fs.s_free[i]);//DEBUG
	}
	//printf("\n");//DEBUG

	sp->release();	// no longer need this block
}


// Read the entire free_list.
// Needs: fs.s_nfree, fs.s_free[]
// @param	device		The device driver
// @param	fs			The filesystem description
void	readFreeList(Device& device, filsys& fs)
{
	//cout << "readFreeList:\n";//DEBUG

	// Free blocks registered in the superblock
	//cout << "Superblock:\n";//DEBUG
	for (int  i = 0; i < fs.s_nfree ; ++i) {
		daddr_x addr = fs.s_free[i];
		//printf(" %ld", addr);//DEBUG
		if (addr != 0)
			useBlock(addr, B_FREE);
	}
	//printf("\n");//DEBUG

	// Follow the freelist chain
	while (fs.s_free[0] != 0)
	{
		//cout << "FreeBlock: " << fs.s_free[0] << endl;//DEBUG
		Block  *bp = device.getBlock(fs.s_free[0]);

		// Where do we start reading in this block
		off_x  offset = 0;

		// Read a new s_nfree value
		fs.s_nfree = bp->getlong(offset);
		offset += sizeof(daddr_x);
		//printf("s_nfree=%d:", fs.s_nfree);//DEBUG

		// Copy a new table to s_free[]
		for (int  i = 0; i < NICFREE; ++i) {
			daddr_x addr = bp->getlong(offset);
			fs.s_free[i] = addr;
			offset += sizeof(daddr_x);
			//printf(" %ld", addr);//DEBUG
			if (addr != 0)
				useBlock(addr, B_FREE);
		}
		//printf("\n");//DEBUG

		bp->release();	// block no longer needed
	}
}


// - - - - - inode stuff - - - - - -


// forward declarations:
void	registerBlocks(Device& device, daddr_x da[], off_x size);
void	registerIndir(Device& device, daddr_x addr, int level, off_x& size);


// Read inode 'inum' from the device.
// @param		device	The device to read from
// @param		inum	The inode number
void	readInode(Device& device, ino_x inum)
{
	dinode	di;			// stores "current" inode info

	//printf("---------------\n");//DEBUG

	// Fetch the block containing that inode
	Block  *ip = device.getBlock(itod(inum));
	useBlock(itod(inum), B_INODE);			// and register it's usage

	// Determine where that inode begins in this block
	off_x  offset = itoo(inum) * INSIZ;		// relative index * object size
	//printf( "inode %d begins at offset %ld\n", inum, offset );//DEBUG

	// Convert some data to native type
	di.di_mode  = ip->getshort(offset + offsetof(dinode, di_mode));
	//printf("inode %d: di_mode=%#07o ", inum, di.di_mode);//DEBUG

	if (di.di_mode != 0)	// Is this inode being used ?
	{
		// Does this kind of inode have datablocks?
		bool hasData =  (  ((di.di_mode & X_IFMT) == X_IFREG)		// a regular file
						|| ((di.di_mode & X_IFMT) == X_IFDIR)		// a directory
						);

		if (hasData) {
			// Get the file size
			di.di_size = ip->getlong(offset + offsetof(dinode, di_size));
			//printf("di_size=%ld ", di.di_size);//DEBUG

			// Convert the 13, 24-bit, blocknumbers in that inode
			// into ordinary 32-bit daddr_x longs.
			daddr_x  da[NADDR];
			ip->l3tol(offset + offsetof(dinode,di_addr), da);

			// Register the blocks used by this inode
			registerBlocks(device, da, di.di_size);
		}
	}
	//printf("\n");//DEBUG

	ip->release();
}


// Register the 13 blocks registered with an inode
// and handle the indirection tree as well.
// @param[i]		device	The device to read
// @param[in]		da[]	The 13 block numbers
// @param[in,out]	size	The (remaining) filesize unaccounted for
void	registerBlocks(Device& device, daddr_x da[], off_x size)
{
	//printf("diskaddr: ");//DEBUG
	for (int  i = 0 ; (i < NADDR) && (size > 0) ; ++i)
	{
		daddr_x	 addr = da[i];
		//printf(" %ld", addr);//DEBUG
		switch (i)
		{
				// NOTE: this is a non-standard 'case' (a GCC extension)
			case 0 ... 9:	// the direct blocks
				if (addr != 0) {	// not a hole?
					useBlock(addr, B_DATA);
				}
				size -= DBLKSIZ;
				break;

			case 10:		// the top indir 1 block
				if (addr != 0) {	// not a hole?
					useBlock(addr, B_INDIR1);
					registerIndir(device, addr, 1, size);
				} else {
					size -= 128 * DBLKSIZ;
				}
				break;

			case 11:		// the top indir 2 block
				if (addr != 0) {	// not a hole?
					useBlock(addr, B_INDIR2);
					registerIndir(device, addr, 2, size);
				} else {
					size -= 128 * 128 * DBLKSIZ;
				}
				break;

			case 12:		// the top indir 3 block
				if (addr != 0) {	// not a hole?
					useBlock(addr, B_INDIR3);
					registerIndir(device, addr, 3, size);
				} else {
					size -= 128 * 128 * 128 * DBLKSIZ;
				}
				break;

			default:
				notreached();
		}
	}
	//printf("\n");//DEBUG
}


// Register data about indirection blocks of an inode
// @param[in]		device	The device to read from
// @param[in]		addr	The address of this indirection block
// @param[in]		level	The indirection level of this block
// @param[in,out]	size	The (remaining) filesize unaccounted for
void	registerIndir(Device& device, daddr_x addr, int level, off_x& size)
{
	Block  *bp = device.getBlock(addr);
	// Note: The usage of this block will be registered by the caller!

	off_x	offset = 0; // where we start reading in this block
	for (uint  i = 0; (i < (DBLKSIZ / sizeof(daddr_x))) && (size > 0) ; ++i)
	{
		// Fetch a blocknumber
		daddr_x  refs = bp->getlong(offset);
		offset += sizeof(daddr_x);

		switch (level)
		{
			case 1:		// indir 1 => data
				if (refs != 0) {	// not a hole?
					useBlock(refs, B_DATA);
				}
				size -= DBLKSIZ;
				break;

			case 2:		// indir 2 => indir 1
				if (refs != 0) {	// not a hole?
					useBlock(refs, B_INDIR1);
					registerIndir(device, refs, 1, size);
				} else {
					size -= 128 * DBLKSIZ;
				}
				break;

			case 3:		// indir 3 => indir 2
				if (refs != 0) {	// not a hole?
					useBlock(refs, B_INDIR2);
					registerIndir(device, refs, 2, size);
				} else {
					size -= 128 * 128 * DBLKSIZ;
				}
				break;

			default:
				notreached();
		}
	}

	bp->release();
}


// - - - - - - - - - -


// The main charter functions
void	charter( const char  *floppie )
{
	cout << "Opening " << floppie << "\n";
	Device  device(floppie);		// 

	useBlock(0, B_BOOT);			// register that 0 is the bootblock

	filsys	fs;						// to hold some information from the superblock

	readSuperblock(device, fs);		// gather some data

	readFreeList(device, fs);		// inspect the free block list

	// how many inodes are there
	ino_x	ninode = (fs.s_isize - 2) * INOPB;	// -2 (bootblock + superblock)
	printf("has %d inodes\n", ninode);

	// handle all inodes
	for (ino_x inum = 1; inum < ninode; ++inum) {
		readInode(device, inum);
	}

	reportUsage();
}

// - - - - - - - - - -

// Print sizes to verify our code is indeed binary compatible
void	checkTypes()
{
	printf("Check datatypesizes:\n");
	printf("sizeof ushort=%d (expect 2) uint=%d (expect 4) ulong=%d (expect 4)\n",
					sizeof(ushort), sizeof(uint), sizeof(ulong));
	printf("sizeof daddr_x=%d (expect 4)\n", sizeof(daddr_x));
	printf("sizeof filsys=%d (expect 440)\n", sizeof(filsys));
	printf("sizeof fblk=%d (expect 204)\n", sizeof(fblk));
	printf("sizeof dinode=%d (expect %d)\n", sizeof(dinode), INSIZ);
	printf("sizeof Block=%d (expect %d)\n", sizeof(Block),
					DBLKSIZ + sizeof(size_t) + sizeof(void*) );
	// If the numbers don't match, change the specs in e7types.h
}


// Main is just the TUI
int  main(int argc, const char *argv[])
{
	checkTypes();
	try {
		// a given parameter or use the default ?
		charter( (argc > 1) ? argv[1] : "floppie.img" );
		return EXIT_SUCCESS;
	} catch(const std::exception& e) {
		std::cerr << AC_RED "OOPS: " << e.what() << AA_RESET << std::endl;
		return EXIT_FAILURE;
	} catch(...) {
		std::cerr << AC_RED "OOPS: something went wrong" AA_RESET << std::endl;
		return EXIT_FAILURE;
	}
}


// vim:aw:ai:ts=4:


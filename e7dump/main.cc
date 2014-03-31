/*
   Dump some information about a 7-th edition filesystem.

   Because the floppy was made on a different kind of
   system than this one, we can not simply use the e7*.h
   header files as normal datastructures.
   Therefor we have to convert the data_as_on_disk into
   data_as_on_this_machine.

 */

#include "ansi.h"		// for: ANSI color codes
#include "asserts.h"	// for: asserts checking
#include "unix_error.h"	// for: system error reporting

// C++ hook to C functions
#include <ctime>		// for: ctime()
						// Converts a unix timestamp (seconds since 1970)
						// into human readable text e.g. "Wed Jun 30 21:49:08 1993\n".
						// The total length will always be 25+1 characters.
#include <cstddef>		// for: offsetof(datatype,attribute)
						// With 'offsetof' you can get the relative position
						// of an attribute of a class.
						// Handy when converting raw diskdata into whatever that
						// data represents.
#include <cstdio>	    // for: printf()
#include <cstdlib>		// for: EXIT_SUCCESS, EXIT_FAILURE

// C++/STL specific
#include <iostream>	    // for: std::cout, std::endl

// The include files for the unix 7-th Edition Filesystem
#include "e7filsys.h"	// the description of the information in the superblock
#include "e7ino.h"	    // the description of the on-disk version of an inode

// Our own classes
#include "Device.h"		// the "device driver"
#include "Block.h"		// the "datablocks"


/* An example Block derived class so we can peek a little. */
class MyBlock : public Block
{
	public:
		/* To peek at the usage counter */
		size_t getUsage() const { return usage_counter; }
};
/* NOTE: It may not have any attributes ! */


// - - - - - - - - - -


// Dump some information from the given "device"
void	dump(const char *floppie)
{
	std::cout << "Opening device '" << floppie << "'\n";
	Device  device(floppie);

	// - - - - - - - - - - -
	// read SUPERBLOCK data
	// Also see: e7filsys.h

	// Object 'fs' will hold the converted data.
	filsys	fs;

	// Fetch the block containing the super-block
	Block  *sp = device.getBlock(SUPERB);

	// Convert some of the raw data to our "native" type

	// Inode space size (this count includes the bootblock and the superblock!)
	fs.s_isize = sp->getshort( offsetof(filsys, s_isize) );
	// Filesystem size of this filesystem (in blocks)
	fs.s_fsize = sp->getlong(  offsetof(filsys, s_fsize) );

	// You can print data with a cstdio function ...
	printf("printf     fs.s_isize=%d fs.s_fsize=%ld\n", fs.s_isize, fs.s_fsize);
	// see also: man 3 printf

	// ... or use iostream operators
	std::cout << "std::cout  fs.s_isize=" << fs.s_isize << " fs.s_fsize=" << fs.s_fsize << std::endl;
	// see also: boek/diktaat c++ etc.

	// Convert a few other superblock attributes.

	// The superblock-last-update timestamp
	fs.s_time = sp->getlong( offsetof(filsys, s_time) );
	printf("fs.s_time=%.24s\n", ctime(&fs.s_time) );   			// see: man 3 ctime

	// The filesystem name
	std::string  s_fname = sp->getstring( offsetof(filsys, s_fname), 6 );
	// The filesystem pack
	std::string  s_fpack = sp->getstring( offsetof(filsys, s_fpack), 6 );

	std::cout << "fs.s_fname=" << s_fname << ", fs.s_fpack=" << s_fpack << "\n";

	sp->release();	// We no longer need this block


	// - - - - - - - - - - - - -
	// read INODE's from disk
	// Also see: e7ino.h

	// Object 'di' will hold some of that "disk-inode" data.
	dinode	di;

	// Fetch the block containing the root inode
	Block*  ip = device.getBlock( itod(ROOTINO) );

	// Determine where the bytes for the root-inode begin in this inode block
	off_x  offset = itoo(ROOTINO) * INSIZ;  // i.e. "dino_index" * "dino_size"
	printf("data for inode %d begins at offset %ld (expect %d)\n", ROOTINO, offset, INSIZ);

	// Convert some of that raw data to our native type

	// The inode type + protection flags
	// If (di_mode == 0) then this inode is not used
	// and the remaining attributes will be garbage.
	di.di_mode  = ip->getshort( offset + offsetof(dinode, di_mode) );
	printf("inode %d mode = %#o (expect 040777)\n", ROOTINO, di.di_mode);

	// Verify this is a directory inode
	if((di.di_mode & X_IFMT) == X_IFDIR) {
		printf(AC_GREEN	"Good: it is a directory\n"	AA_RESET);
	} else {
		printf(AC_RED	"Oops: it is not a directory\n"	AA_RESET);
	}

	// Convert the 13, 24-bits, blocknumbers in the inode
	// to normal 32-bit daddr_x values (only valid for DIR or REG type)
	daddr_x  diskaddrs[NADDR];		// 13 blocknumbers
	ip->l3tol( offset + offsetof(dinode, di_addr), diskaddrs );
	printf("diskaddr: ");
	for(int  i = 0 ; i < NADDR ; ++i) {
		printf(" %ld", diskaddrs[i]);
	}
	printf("\n");

	ip->release();	// We no longer need this block

	// - - - - - - - - - -
	// TypeCasting test

	Block	*bp = device.getBlock(0);

	// Now pretend it is a MyBlock instance
	MyBlock *mbp = static_cast<MyBlock*>(bp);	// brute-force type casting

	printf("compare mbp=%p with bp=%p\n", mbp, bp);	// should be the same?
	check(mbp == bp);

	// Try to use the "derived class" pointer for operations on the Block baseclass
	printf("usage count before release is %d\n", mbp->getUsage());
	mbp->release();
	printf("usage count  after release is %d\n", mbp->getUsage());
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
		dump( (argc > 1) ? argv[1] : "floppie.img" );
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


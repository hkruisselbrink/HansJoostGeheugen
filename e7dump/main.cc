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
#include "e7dir.h"

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
	std::cout << "Opening device '" << floppie << "'\n\n";
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
	// The filesystem name
	std::string  s_fname = sp->getstring( offsetof(filsys, s_fname), 6 );
	// The filesystem pack
	std::string  s_fpack = sp->getstring( offsetof(filsys, s_fpack), 6 );

   	fs.s_nfree = sp->getshort(  offsetof(filsys, s_nfree) );

    fs.s_ninode = sp->getshort(  offsetof(filsys, s_ninode) );

    fs.s_tinode = sp->getlong( offsetof(filsys, s_tinode) );

    fs.s_flock = sp->getbyte( offsetof(filsys, s_flock) );

    fs.s_ilock = sp->getbyte( offsetof(filsys, s_ilock) );

    fs.s_fmod = sp->getbyte( offsetof(filsys, s_fmod) );

    fs.s_ronly = sp->getbyte( offsetof(filsys, s_ronly) );

    fs.s_time = sp->getlong( offsetof(filsys, s_time) );

    fs.s_tfree = sp->getlong( offsetof(filsys, s_tfree) );

    fs.s_tinode = sp->getshort( offsetof(filsys, s_tinode) );

    fs.s_m = sp->getshort( offsetof(filsys, s_m) );

    fs.s_n = sp->getshort( offsetof(filsys, s_n) );

    off_x offset0 = offsetof(filsys, s_free);
    for(int i = 0; i < NICFREE; ++i) {
        fs.s_free[i] = sp->getlong(offset0);
        //std::cout << fs.s_free[i] + " ";
        offset0 += sizeof(daddr_x);
    }

    off_x offset1 = offsetof(filsys, s_inode);
    for(int i = 0; i < NICINOD; ++i) {
        fs.s_inode[i] = sp->getshort(offset1);
        //std::cout << fs.s_free[i] + " ";
        offset1 += sizeof(ino_x);
    }


	// ... or use iostream operators
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "Dump of superblock on " << s_fname << "." << s_fpack << std::endl;
	std::cout << "---------------------------------------" << std::endl;
	std::cout << "number of blocks in i-list is: " << fs.s_isize << std::endl;
	std::cout << "number of blocks on volume is: " << fs.s_fsize << std::endl;
	std::cout << "number of freeblocks in free[] is: " << fs.s_nfree << " " << std::endl;
	for(int i = 0; i < fs.s_nfree; i++) {
        std::cout << fs.s_free[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "number of freeblocks in inode[] is: " << fs.s_ninode << " " << std::endl;
    for(int i = 0; i < fs.s_ninode; i++) {
        std::cout << fs.s_inode[i] << " ";
    }
    std::cout << std::endl;
    std::cout << "freelist lock flag is ";
    if(fs.s_flock == 0) {
        std::cout << "not set" << std::endl;
    } else {
        std::cout << "set" << std::endl;
    }
    std::cout << "i-list lock flag is ";
    if(fs.s_ilock == 0) {
        std::cout << "not set" << std::endl;
    } else {
        std::cout << "set" << std::endl;
    }
    std::cout << "superblock is ";
    if(fs.s_fmod == 0) {
        std::cout << "not modified" << std::endl;
    } else {
        std::cout << "modified" << std::endl;
    }
    std::cout << "filesystem was ";
    if(fs.s_ronly == 0) {
        std::cout << "not not mounted readonly" << std::endl;
    } else {
        std::cout << "was mounted readonly" << std::endl;
    }
    printf("Last update was %.24s\n", ctime(&fs.s_time) );
    std::cout << "total number of free blocks is: " << fs.s_tfree << std::endl;
    std::cout << "total number of free inodes is: " << fs.s_tinode << std::endl;
    std::cout << "interleave factors are: " << fs.s_m << " and " << fs.s_n << std::endl;
    std::cout << "File system name is: " << s_fname << std::endl;
    std::cout << "File system pack is: " << s_fpack << std::endl;
    std::cout << "---------------------------------------" << std::endl;
    std::cout << "Dump of freelist on" << s_fname << "." << s_fpack << std::endl;
    std::cout << "---------------------------------------" << std::endl << "In superblock:" << std::endl;
    printf("Holds %d entries:", fs.s_nfree);//DEBUG
        std::cout << std::endl;
    for(int i = 0; i < fs.s_nfree; i++) {
        std::cout << fs.s_free[i] << " ";
    }
    std::cout << std::endl;

    while (fs.s_free[0] != 0)
	{
		std::cout << "Fetching freeblock: " << fs.s_free[0] << std::endl;

		Block  *bp = device.getBlock(fs.s_free[0]);

		// Where do we start reading in this block
		off_x  offset = 0;

		// Read a new s_nfree value
		fs.s_nfree = bp->getlong(offset);
		offset += sizeof(daddr_x);
		printf("Holds %d entries:", fs.s_nfree);
        std::cout << std::endl;
		// Copy a new table to s_free[]
		for (int  i = 0; i < NICFREE; ++i) {
			daddr_x addr = bp->getlong(offset);
			fs.s_free[i] = addr;
			offset += sizeof(daddr_x);
			printf(" %ld", addr);
		}
		std::cout << std::endl;

		bp->release();	// block no longer needed
	}
    std::cout << "---------------------------------------" << std::endl << "Inode info on floppy:" << std::endl << "---------------------------------------" << std::endl;
    ino_x	ninode = (fs.s_isize - 2) * INOPB;
    for (ino_x inum = 1; inum < ninode; ++inum) {
        dinode	di;
        Block  *ip = device.getBlock(itod(inum));
        off_x  offset = itoo(inum) * INSIZ;

        di.di_mode  = ip->getshort(offset + offsetof(dinode, di_mode));
        di.di_nlink = ip->getshort(offset + offsetof(dinode, di_nlink));
        di.di_uid = ip->getshort(offset + offsetof(dinode, di_uid));
        di.di_gid = ip->getshort(offset + offsetof(dinode, di_gid));
        di.di_atime = ip->getlong(offset + offsetof(dinode, di_atime) );
        di.di_ctime = ip->getlong(offset + offsetof(dinode, di_ctime) );
        di.di_mtime = ip->getlong(offset + offsetof(dinode, di_mtime) );


        if (di.di_mode != 0)	// Is this inode being used ?
        {
            bool hasData =  (  ((di.di_mode & X_IFMT) == X_IFREG)		// a regular file
						|| ((di.di_mode & X_IFMT) == X_IFDIR)		// a directory
						);

            bool isDir = (di.di_mode & X_IFMT) == X_IFDIR;

            if (hasData) {

                std::cout << "Inode: " << inum  << std::endl;
                std::cout << "mode= " << di.di_mode  << std::endl;
                std::cout << "nlink=" << di.di_nlink << " uid=" << di.di_uid  << " gid=" << di.di_gid  << std::endl;
                // Get the file size
                di.di_size = ip->getlong(offset + offsetof(dinode, di_size));
                //printf("di_size=%ld ", di.di_size);//DEBUG
                std::cout << "size=" << di.di_size;

                // Convert the 13, 24-bit, blocknumbers in that inode
                // into ordinary 32-bit daddr_x longs.
                daddr_x  da[NADDR];
                ip->l3tol(offset + offsetof(dinode,di_addr), da);
                printf(" addr=");
                    for(int  i = 0 ; i < NADDR ; ++i) {
                        printf(" %ld", da[i]);
                    }
                printf("\n");

                off_x size = di.di_size;

                if(size > 0) {
                    std::cout << "Direct blocks: ";
                }

                for (int  i = 0 ; (i < NADDR) && (size > 0) ; ++i)
                {
                    daddr_x	 addr = da[i];
                    //printf(" %ld", addr);//DEBUG
                    switch (i)
                    {
                            // NOTE: this is a non-standard 'case' (a GCC extension)
                        case 0 ... 9:	// the direct blocks
                            if (addr != 0) {	// not a hole?
                               std::cout << addr << " ";
                            }
                            size -= DBLKSIZ;
                            break;

                        case 10:		// the top indir 1 block
                            if (addr != 0) {	// not a hole?
                                std::cout << std::endl << "Indirect 1 blocks: " << addr << ":";
                            } else {
                                size -= 128 * DBLKSIZ;
                            }
                            break;

                        case 11:		// the top indir 2 block
                            if (addr != 0) {	// not a hole?
                                std::cout << std::endl << "Indirect 2 blocks: " << addr << ":";
                            } else {
                                size -= 128 * 128 * DBLKSIZ;
                            }
                            break;

                        case 12:		// the top indir 3 block
                            if (addr != 0) {	// not a hole?
                                std::cout << std::endl << "Indirect 3 blocks: " << addr << ":";
                            } else {
                                size -= 128 * 128 * 128 * DBLKSIZ;
                            }
                            break;

                        default:
                            notreached();
                    }
                }
                std::cout << std::endl;

                printf("atime=%.24s\n", ctime(&di.di_atime) );
                printf("ctime=%.24s\n", ctime(&di.di_ctime) );
                printf("mtime=%.24s\n", ctime(&di.di_mtime) );

                std::cout << "---------------------------------------" << std::endl;
                // Register the blocks used by this inode
                //registerBlocks(device, da, di.di_size);
            }

        }
	}

	sp->release();	// We no longer need this block
}

// - - - - - - - - - -



// Main is just the TUI
int  main(int argc, const char *argv[])
{
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


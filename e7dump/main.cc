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
#include <fstream>
#include <vector>
#include <bitset>
using namespace std;

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

void	registerIndir(Device& device, daddr_x addr, int level, off_x& size, ostream& out, vector<daddr_x>& blokken);
void	readInode(Device& device, ino_x inum, ostream& out);

// Read super-block data from the device into 'fs'.
// We only need: s_isize, s_nfree and s_free[]
// @param		device	The device "driver"
// @param[out]	fs		The filsys struct to be partially filled.
void	readSuperblock(Device& device, filsys& fs, ostream& out)
{
	Block  *sp = device.getBlock(SUPERB);

	fs.s_isize = sp->getshort( offsetof(filsys, s_isize) );
	fs.s_fsize = sp->getlong(  offsetof(filsys, s_fsize) );
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
	string  s_fname = sp->getstring( offsetof(filsys, s_fname), 6 );
	string  s_fpack = sp->getstring( offsetof(filsys, s_fpack), 6 );

	sp->release();

	int fnamesize = s_fname.size();
	for(int i = 0; i<=fnamesize;i++) {
        fs.s_fname[i] = s_fname[i];
	}

	int fpacksize = s_fpack.size();
	for(int i = 0; i<=fpacksize;i++) {
        fs.s_fpack[i] = s_fpack[i];
	}

    off_x offset0 = offsetof(filsys, s_free);
    for(int i = 0; i < NICFREE; ++i) {
        fs.s_free[i] = sp->getlong(offset0);
        offset0 += sizeof(daddr_x);
    }

    off_x offset1 = offsetof(filsys, s_inode);
    for(int i = 0; i < NICINOD; ++i) {
        fs.s_inode[i] = sp->getshort(offset1);
        offset1 += sizeof(ino_x);
    }

	out << "---------------------------------------" << endl;
	out << "Dump of superblock on " << fs.s_fname << "." << fs.s_fpack << endl;
	out << "---------------------------------------" << endl;
	out << "number of blocks in i-list is: " << fs.s_isize << endl;
	out << "number of blocks on volume is: " << fs.s_fsize << endl;
	out << "number of freeblocks in free[] is: " << fs.s_nfree << " " << endl;

	for(int i = 0; i < fs.s_nfree; i++) {
        out << fs.s_free[i] << " ";
    }
    out << endl;

    out << "number of freeblocks in inode[] is: " << fs.s_ninode << " " << endl;
    for(int i = 0; i < fs.s_ninode; i++) {
        out << fs.s_inode[i] << " ";
    }
    out << endl;

    out << "freelist lock flag is ";
    if(fs.s_flock == 0) {
        out << "not set" << endl;
    } else {
        out << "set" << endl;
    }

    out << "i-list lock flag is ";
    if(fs.s_ilock == 0) {
        out << "not set" << endl;
    } else {
        out << "set" << endl;
    }

    out << "superblock is ";
    if(fs.s_fmod == 0) {
        out << "not modified" << endl;
    } else {
        out << "modified" << endl;
    }

    out << "filesystem was ";
    if(fs.s_ronly == 0) {
        out << "not mounted readonly" << endl;
    } else {
        out << "was mounted readonly" << endl;
    }

    out << "Last update was " << ctime(&fs.s_time);
    out << "total number of free blocks is: " << fs.s_tfree << endl;
    out << "total number of free inodes is: " << fs.s_tinode << endl;
    out << "interleave factors are: " << fs.s_m << " and " << fs.s_n << endl;
    out << "File system name is: " << fs.s_fname << endl;
    out << "File system pack is: " << fs.s_fpack << endl;
}

// Read the entire free_list.
// Needs: fs.s_nfree, fs.s_free[]
// @param	device		The device driver
// @param	fs			The filesystem description
void	readFreeList(Device& device, filsys& fs, ostream& out)
{
    out << "---------------------------------------" << endl;
    out << "Dump of freelist on " << fs.s_fname << "." << fs.s_fpack << endl;
    out << "---------------------------------------" << endl << "In superblock:" << endl;
    out << "Holds " << fs.s_nfree << " entries:" << endl;
    for(int i = 0; i < fs.s_nfree; i++) {
        out << fs.s_free[i] << " ";
    }
    out << endl;

    while (fs.s_free[0] != 0)
	{
		out << "Fetching freeblock: " << fs.s_free[0] << endl;

		Block  *bp = device.getBlock(fs.s_free[0]);

		off_x  offset = 0;

		fs.s_nfree = bp->getlong(offset);
		offset += sizeof(daddr_x);
		out << "Holds " << fs.s_nfree << " entries:" << endl;
		for (int  i = 0; i < NICFREE; ++i) {
			daddr_x addr = bp->getlong(offset);
			fs.s_free[i] = addr;
			offset += sizeof(daddr_x);
			out << " " << addr;
		}
		out << endl;

		bp->release();
	}
}

void readInodes(Device& device, filsys& fs, ostream& out) {
    out << "---------------------------------------" << endl
        << "Inode info on floppy:" << endl
        << "---------------------------------------" << endl;
    ino_x	ninode = (fs.s_isize - 2) * INOPB;
    for(ino_x inum = 1; inum < ninode; ++inum) {
        readInode(device, inum, out);
    }
}

void	readInode(Device& device, ino_x inum, ostream& out) {
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
    di.di_size = ip->getlong(offset + offsetof(dinode, di_size));

    if (di.di_mode != 0) {
        bool hasData = (((di.di_mode & X_IFMT) == X_IFREG) || ((di.di_mode & X_IFMT) == X_IFDIR));
        bool isDir = (di.di_mode & X_IFMT) == X_IFDIR;
        if (hasData) {

            //PRINT DATA
            out << "Inode: " << inum  << endl;

            std::bitset<16> modebits(di.di_mode);
            string mode = "(";
            if(isDir) {
                mode += "d";
            } else {
                mode += "-";
            }
            for(int i = 8; i >= 0; i--) {
                if(modebits[i] == 1) {
                    int m = i%3;
                    switch(m) {
                    case 0:
                        mode += "x";
                        break;
                    case 1:
                        mode += "w";
                        break;
                    case 2:
                        mode += "r";
                        break;
                    }
                } else {
                    mode += "-";
                }
            }
            out << "mode= " << mode << ") ";

            int type = 0;
            int value = 8;
            for(int i = 15; i > 11; i--) {
                if(modebits[i] == 1) {
                    type += value;
                }
                value = value/2;
            }
            out << "type= " << type << endl;

            out << "nlink=" << di.di_nlink << " uid=" << di.di_uid  << " gid=" << di.di_gid  << endl;
            out << "size=" << di.di_size;

            //PRINT BLOCKS
            daddr_x  da[NADDR];
            ip->l3tol(offset + offsetof(dinode,di_addr), da);
            out << " addr=";
            for(int  i = 0 ; i < NADDR ; ++i) {
                out << " " << da[i];
            }
            out << endl;

            //PRINT INDIRECTION
            vector<daddr_x> blokken;
            off_x size = di.di_size;
            if(size > 0) {
                out << "Direct blocks: ";
                for (int  i = 0 ; (i < NADDR) && (size > 0) ; ++i) {
                    daddr_x	 addr = da[i];
                    switch (i) {
                        case 0 ... 9:	// the direct blocks
                            if (addr != 0) {
                                out << addr << " ";
                                blokken.push_back(addr);
                            }
                            size -= DBLKSIZ;
                            break;

                        case 10:		// the top indir 1 block
                            if (addr != 0) {
                                out << endl << "Indirect 1 blocks: " << addr << ":";
                                registerIndir(device, addr, 1, size, out, blokken);
                            } else {
                                size -= 128 * DBLKSIZ;
                            }
                            break;

                        case 11:		// the top indir 2 block
                            if (addr != 0) {
                                out << endl << "Indirect 2 blocks: " << addr << ":";
                                registerIndir(device, addr, 2, size, out, blokken);
                            } else {
                                size -= 128 * 128 * DBLKSIZ;
                            }
                            break;

                        case 12:		// the top indir 3 block
                            if (addr != 0) {
                                out << endl << "Indirect 3 blocks: " << addr << ":";
                                registerIndir(device, addr, 3, size, out, blokken);
                            } else {
                                size -= 128 * 128 * 128 * DBLKSIZ;
                            }
                            break;

                        default:
                            break;
                    }
                }
                out << endl;
            }
            out << "atime=" << ctime(&di.di_atime);
            out << "ctime=" << ctime(&di.di_ctime);
            out << "mtime=" << ctime(&di.di_mtime);

            // if DIR then print dir contents
            if(isDir) {
                //print dir contents

            }
            out << "---------------------------------------" << endl;
        }
    }
}

void registerIndir(Device& device, daddr_x addr, int level, off_x& size, ostream& out, vector<daddr_x>& blokken) {
    Block  *bp = device.getBlock(addr);

	off_x	offset = 0;
	for (uint  i = 0; (i < (DBLKSIZ / sizeof(daddr_x))) && (size > 0) ; ++i)
	{
		daddr_x  refs = bp->getlong(offset);
		offset += sizeof(daddr_x);

		switch (level)
		{
			case 1:		// indir 1 => data
				if (refs >= 0) {
					out << refs << " ";
					blokken.push_back(refs);
				}
				size -= DBLKSIZ;
				break;

			case 2:		// indir 2 => indir 1
				if (refs != 0) {
					out << "[" << refs << "] ";
					registerIndir(device, refs, 1, size, out, blokken);
				} else {
				    out << "[" << refs << "] ";
					size -= 128 * DBLKSIZ;
				}
				break;

			case 3:		// indir 3 => indir 2
				if (refs != 0) {
					out << "[[" << refs << "]] ";
					registerIndir(device, refs, 2, size, out, blokken);
				} else {
				    out << "[[" << refs << "]] ";
					size -= 128 * 128 * DBLKSIZ;
				}
				break;

			default:
				break;
		}
	}

	bp->release();
}

void	dump(const char *floppie, ostream& out)
{
	Device  device(floppie);
    filsys	fs;

    readSuperblock(device, fs, out);
    readFreeList(device, fs, out);
    readInodes(device, fs, out);
}

int  main(int argc, const char *argv[])
{
	try {

		ofstream file ("output.txt");
		if(file.is_open()) {
            dump( (argc > 1) ? argv[1] : "floppie.img" , file);
		}
		file.close();

		return EXIT_SUCCESS;
	} catch(const exception& e) {
		cerr << AC_RED "OOPS: " << e.what() << AA_RESET << endl;
		return EXIT_FAILURE;
	} catch(...) {
		cerr << AC_RED "OOPS: something went wrong" AA_RESET << endl;
		return EXIT_FAILURE;
	}
}


// vim:aw:ai:ts=4:


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
	filsys	fs;

	// Fetch the block containing the super-block
	Block  *sp = device.getBlock(SUPERB);

	fs.s_isize = sp->getshort( offsetof(filsys, s_isize) );

	std::cout << fs.s_isize << std::endl;

}


// Main is just the TUI
int  main(int argc, const char *argv[])
{
	//checkTypes();
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


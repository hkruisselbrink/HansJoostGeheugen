/** @file NextFit.cc
 * De implementatie van BestFit.
 */

#include "main.h"
#include "BestFit.h"

// Clean up dead stuff
BestFit::~BestFit()
{
    while (!areas.empty()) {
		Area  *ap = areas.back();
		areas.pop_back();
		delete ap;
	}
}

// Print the current freelist for debugging
void	BestFit::dump()
{
	std::cerr << AC_BLUE << type << "::areas";
	for (ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
		std::cerr << ' ' << **i;
	}
	std::cerr << AA_RESET << std::endl;
}

// Iemand vraagt om 'wanted' geheugen
Area  *BestFit::alloc(int wanted)
{
	require(wanted > 0);		// minstens "iets",
	require(wanted <= size);	// maar niet meer dan we kunnen hebben.

	updateStats();				// update resource map statistics

	// Search thru all available free areas
	Area  *ap = 0;
	ap = searcher(wanted);		// first attempt
	if (ap) {					// success ?
		return ap;
	}

	// Alas, failed to allocate anything
	//dump();//DEBUG
	return 0;
}

// Application returns an area no longer needed
void	BestFit::free(Area *ap)
{
	require(ap != 0);
	if (cflag) {
		// EXPENSIVE: check for overlap with already registered free areas
		for (ALiterator  i = areas.begin() ; i != areas.end() ; ++i) {
			check(!ap->overlaps(*i));    // sanity check
		}
	}
	areas.push_back(ap);	// de lazy version
}

// ----- hulpfuncties -----

Area  *BestFit::searcher(int wanted)
{
    require(wanted > 0);		// minstens "iets",
	require(wanted <= size);	// maar niet meer dan we kunnen hebben.

    return 0;
}


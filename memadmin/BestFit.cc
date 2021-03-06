/** @file NextFit.cc
 * De implementatie van BestFit.
 */

#include "main.h"
#include "BestFit.h"

// Clean up dead stuff
BestFit::~BestFit()
{
    while (!areas.empty())
    {
        Area  *ap = areas.back();
        areas.pop_back();
        delete ap;
    }
}

// Initializes how much memory we own
void  BestFit::setSize(int new_size)
{
    require(areas.empty());					// prevent changing the size when the freelist is nonempty
    Fitter::setSize(new_size);
    areas.push_back(new Area(0, new_size));	// and create the first free area (i.e. "all")
}

// Print the current freelist for debugging
void	BestFit::dump()
{
    std::cerr << AC_BLUE << type << "::areas";
    for (ALiterator  i = areas.begin() ; i != areas.end() ; ++i)
    {
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
    if (ap)  					// success ?
    {
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

    if (cflag)
    {

        // EXPENSIVE: check for overlap with already registered free areas
        for (ALiterator  i = areas.begin() ; i != areas.end() ; ++i)
        {
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
    require(!areas.empty());

    ALiterator  j = areas.end();    // where did we find the
    Area *currentBest = 0;          // best candidate

    for(ALiterator  i = areas.begin() ; i != areas.end() ; ++i)
    {
        Area  *ap = *i;
        if((ap->getSize() >= wanted)
           && (!currentBest || (ap->getSize() < currentBest->getSize())))
        {
            currentBest = ap;
            j = i;
        }
    }
    if (!currentBest)
        return 0;

    ALiterator next = areas.erase(j);
    if(currentBest->getSize() > wanted)
    {
        Area *rp = currentBest->split(wanted);
        areas.insert(next, rp);
    }

    return currentBest;
}

// Try to join fragmented freespace
bool	BestFit::reclaim()
{
    bool  changed = false;	// did we change anything ?

    // Sort resource map by area address
    areas.sort( Area::orderByAddress() );	// WARNING: expensive N*log(N) operation !

    // Search thru all available areas for matches
    ALiterator  i = areas.begin();
    Area  *ap = *i;				// the current candidate ...
    for (++i ; i != areas.end() ;)
    {
        Area  *bp = *i;			// ... match ap with ...
        if (bp->getBase() == (ap->getBase() + ap->getSize()))
        {
            // oke, bp matches ap ...
            ALiterator  next = areas.erase(i);	// remove bp from the list
            ap->join(bp);			// append area bp to ap (and destroy bp)
            ++mergers;				// update statistics
            changed = true;			// yes we changed something
            i = next;				// revive the 'i' iterator
        }
        else
        {
            ap = bp;				// move on to next area
            ++i;
        }
    }
    if (changed)  					// iff we have changed some area's the
    {
        cursor = areas.begin();		// next search should start at the (new) front
    }
    ++reclaims;						// update statistics
    return changed;
}

// Update statistics
void	BestFit::updateStats()
{
    ++qcnt;									// number of 'alloc's
    qsum  += areas.size();					// length of resource map
    qsum2 += (areas.size() * areas.size());	// same: squared
}


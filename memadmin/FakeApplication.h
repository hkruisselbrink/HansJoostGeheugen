#pragma once
#ifndef	__FakeApplication_h__
#define	__FakeApplication_h__
//#ident	"@(#)FakeApplication.h	2.1	AKK	20090222"

/** @file FakeApplication.h
 *  @brief The FakeApplication class.
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 2.1	2009/02/22
 */

// onze eigen includes
#include "Allocator.h"	// baseclass Allocator
#include "Area.h"		// class Area

/// @class FakeApplication
/// De namaak applicatie/tester/performance meter class.
class FakeApplication
{
private:
	Allocator	*beheerder;	// de huidige geheugenbeheers module
	int			 size;		// de omvang van het beheerde geheugen

	AreaList	 objecten;	// de lijst van de gekregen gebieden

	bool		 vflag;		// "verbose" mode;
							// true als we willen zien wat er gebeurt
	bool		 tflag;		// "test" mode;
							// true als we de code willen "testen"
							// anders gaan we "performance meten".
							// (NOTE: iff true, it turns off the vflag)

public:

	/// Maak een "applicatie" die geheugen vraagt aan de gegeven beheerder,
	/// die op zijn beurt de beschikking heeft over 'size' eenheden geheugen.
	/// @param	beheerder
	/// @param	size
	FakeApplication(Allocator *beheerder, int size);

	~FakeApplication();			///< cleanup things

	void testing();			///< run a few test cases

	/// Voer een random scenario uit
	/// @param	aantal	hoe vaak wordt er alloc of free gedaan
	/// @param	vflag	true=vertel wat er allemaal gebeurt (kost wel performance)
	void randomscenario(int aantal, bool vflag);

	// voer een minder random scenario uit(webbrowserish)
	void minderRandomScenario(int aantal, bool vflag);

	//
	// voeg hier straks je eigen scenario(s) toe
	//

private:

	// interne hulpjes
	void	vraagGeheugen(int omvang);
	void	vergeetOudste();
	void	vergeetRandom();
	int kiesServlet(int nummer);

	// for statistics
	int		err_teller; // Errors teller
	int		oom_teller; // Out-Of-Memory teller
};

#endif	/*FakeApplication_h*/
// vim:sw=4:ai:aw:ts=4:

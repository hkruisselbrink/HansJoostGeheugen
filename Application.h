#pragma once
#ifndef	__Application_h__
#define	__Application_h__
//#ident	"@(#)Application.h	2.1	AKK	20090222"

/** @file Application.h
 *  @brief The Application class.
 *  @author R.A.Akkersdijk@saxion.nl
 *  @version 2.1	2009/02/22
 */

// onze eigen includes
#include "Allocator.h"	// baseclass Allocator
#include "Area.h"		// class Area

/// @class Application
/// De namaak applicatie/tester/performance meter class.
class Application
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
	Application(Allocator *beheerder, int size);

	~Application();			///< cleanup things

	void testing();			///< run a few test cases

	/// Voer een random scenario uit
	/// @param	aantal	hoe vaak wordt er alloc of free gedaan
	/// @param	vflag	true=vertel wat er allemaal gebeurt (kost wel performance)
	void randomscenario(int aantal, bool vflag);

	//
	// voeg hier straks je eigen scenario(s) toe
	//

private:

	// interne hulpjes
	void	vraagGeheugen(int omvang);
	void	vergeetOudste();
	void	vergeetRandom();

	// for statistics
	int		err_teller; // Errors teller
	int		oom_teller; // Out-Of-Memory teller
};

#endif	/*Application_h*/
// vim:sw=4:ai:aw:ts=4:

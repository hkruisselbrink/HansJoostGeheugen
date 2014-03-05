#ident	"@(#)main.cc	2.1	AKK	20120721"
/** @file main.cc
 * Het hoofdprogramma.
 */

// Unix/Linux includes
#include <ctime>	// time(2)
#include <csignal>	// signal(2) or signal(3)
#include <cstdlib>	// exit(2), atexit(3), atol(3), EXIT_SUCCESS, EXIT_FAILURE
#include <getopt.h>	// int getopt(3) en char *optarg
#include <unistd.h>
// Zie ook manuals: signal(2), exit(3), atol(3) en getopt(3)
#if defined(__MINGW_H)
# include <process.h>	// A non-standard include for: getpid(2)
#endif

// En onze eigen includes
#include "ansi.h"	// ansi color code strings
#include "main.h"	// includes several other includes

// ===================================================================

// Informatie over de diverse geheugenbeheer algoritmes:
#include "RandomFit.h"	// de RandomFit allocator
#include "FirstFit.h"	// de FirstFit allocator (lazy version)
#include "FirstFit2.h"	// de FirstFit2 allocator (eager version)
#include "NextFit.h"	// de NextFit allocator (lazy version)
#include "NextFit2.h"	// de NextFit2 allocator (eager version)
// .... voeg hier je eigen variant(en) toe ....
// bijvoorbeeld:
//#include "BestFit.h"		// pas de naam aan aan jouw versie
//#include "BestFit2.h"		// pas de naam aan aan jouw versie
//#include "WorstFit.h"		// pas de naam aan aan jouw versie
//#include "WorstFit2.h"		// pas de naam aan aan jouw versie
//#include "PowerOfTwo.h"	// pas de naam aan aan jouw versie
//#include "McKusickK.h"	// pas de naam aan aan jouw versie
//#include "Buddy.h"		// pas de naam aan aan jouw versie
//enz


// ===================================================================
// Het hoofdprogramma.
// ===================================================================
// Handelt de meegegeven opties af, kies een geheugen beheerder, enz
// en dan gaan we wat spelen.

// Globale hulpvariabelen voor 'main' en 'doOptions'
Allocator	 *beheerder = 0;		///< de gekozen Allocator
int			  size = 10240;			///< de omvang van het beheerde geheugen
int			  aantal = 10000;		///< hoe vaak doen we iets met dat geheugen
bool		  tflag = false;		///< 'true' als we de code willen "testen"
									///< anders wordt er "gemeten"
bool		  vflag = false;		///< vertel wat er gebeurt
bool		  cflag = false;		///< laat de allocator foute 'free' acties detecteren
									///< (voor sommige algorithmes is dit duur)


/// Vertel welke opties dit programma kent
void	tellOptions(const char *progname)
{
	cout << "Usage: " << progname << " "
	     AS_UNDERLINE"options"AA_RESET ", valid options are:" << endl;

	// Algemeen
	cout << "\t-s size\t\tsize of memory being administrated\n";
	cout << "\t-a count\tnumber of actions (current=" << aantal << ")\n";
	cout << "\t-t\t\ttoggle test mode (current=" << (tflag ? "on" : "off") << ")\n";
	cout << "\t-v\t\ttoggle verbose mode (current=" << (vflag ? "on" : "off") << ")\n";
	cout << "\t-c\t\ttoggle check mode (current=" << (cflag ? "on" : "off") << ")\n";

	// De fitter groep
	cout << "\t-r\t\tuse the random allocator\n";
	cout << "\t-f\t\tuse the first fit allocator (lazy)\n";
	cout << "\t-F\t\tuse the first fit allocator (eager)\n";
	cout << "\t-n\t\tuse the next fit allocator (lazy)\n";
	cout << "\t-N\t\tuse the next fit allocator (eager)\n";
	//cout << "\t-b\t\tuse the best fit allocator (lazy)\n";
	//cout << "\t-B\t\tuse the best fit allocator (eager)\n";
	//cout << "\t-w\t\tuse the worst fit allocator (lazy)\n";
	//cout << "\t-W\t\tuse the worst fit allocator (eager)\n";

	// De power-of-2 groep
	//cout << "\t-p\t\tuse power of 2 allocator\n";
	//cout << "\t-m\t\tuse mckusick/karols allocator\n";
	//cout << "\t-2\t\tuse buddy algorithm\n";

}


/// Analyseert de opties die de gebruiker mee gaf.
/// Kan/zal diverse globale variabelen veranderen !
void	doOptions(int argc, char *argv[])
{
	char  options[] = "s:a:tvcrfFnN"; // De opties die we willen herkennen
	//
	// Als je algoritmes toevoegt dan moet je de string hierboven uitbreiden.
	// (Vergeet niet tellOptions ook aan te passen)
	//
	// Als je alle algoritmes zou realiseren dan wordt
	// het iets in de trant van: "s:a:tvcrfFnNbBwWpm2"
	//
	// Algemene opties:
	// "s:" staat voor: -s xxx = omvang van het beheerde geheugen (in "eenheden")
	// "a:" staat voor: -a xxx = aantal alloc/free acties (aka new/delete)
	// "t"  staat voor: -t = code testen (i.p.v. performance meten)
	// "v"  staat voor: -v = verbose mode (vertel wat er gebeurt)
	// "c"  staat voor: -c = check mode (bewaak 'free' acties)
	//
	// Opties om een beheeralgoritme uit te kiezen ...
	// r  staat voor: -r = random-fit allocator
	// f  staat voor: -f = first-fit allocator (lazy)
	// F  staat voor: -F = first-fit allocator (eager)
	// n  staat voor; -n = next-fit allocator (lazy)
	// N  staat voor; -N = next-fit allocator (eager)
	// b  staat voor; -b = best-fit allocator (lazy)
	// B  staat voor; -b = best-fit allocator (eager)
	// w  staat voor; -w = worst-fit allocator (lazy)
	// W  staat voor; -w = worst-fit allocator (eager)
	//  enz
	// 2  staat voor: -2 = buddy allocator
	//
	// Voor meer informatie, zie: man 3 getopt
	//

	int  opt = 0; // Het "huidige" optie karakter (of -1).

	do {
		// Haal een optie uit argc/argv
		// (en zet zonodig het bijbehorende argument in 'optarg')
		opt = getopt(argc, argv, options);
		switch (opt) {	// welke optie is dit?
				// ALGEMEEN
			case 's': // the size of the (imaginary) memory being managed
				size = atol(optarg);   // atol = "ascii to long"
				break;
			case 'a': // the number of alloc/free actions
				aantal = atol(optarg);
				break;
			case 't': // toggle test mode
				tflag = !tflag;
				break;
			case 'v': // toggle verbose mode
				vflag = !vflag;
				break;
			case 'c': // toggle check mode
				cflag = !cflag;
				if (beheerder)
					beheerder->setCheck(cflag);
				break;

				// ALGORITMES
			case 'r': // -r = RandomFit allocator gevraagd
				require(beheerder == 0);   // eens gekozen, blijft gekozen
				beheerder = new RandomFit(cflag);
				break;
			case 'f': // -f = FirstFit allocator gevraagd (lazy)
				require(beheerder == 0);
				beheerder = new FirstFit(cflag);
				break;
			case 'F': // -F = FirstFit allocator gevraagd (eager)
				require(beheerder == 0);
				beheerder = new FirstFit2(cflag);
				break;
			case 'n': // -n = NextFit allocator gevraagd
				require(beheerder == 0);
				beheerder = new NextFit(cflag);
				break;
			case 'N': // -n = NextFit2 allocator gevraagd
				require(beheerder == 0);
				beheerder = new NextFit2(cflag);
				break;
			/*
			case 'b': // -b = BestFit allocator gevraagd
				require(beheerder == 0);
				beheerder = new BestFit(cflag);
				break;
			case 'B': // -B = BestFit2 allocator gevraagd
				require(beheerder == 0);
				beheerder = new BestFit2(cflag);
				break;
			case 'w': // -w = WorstFit allocator gevraagd
				require(beheerder == 0);
				beheerder = new WorstFit(cflag);
				break;
			case 'W': // -W = WorstFit2 allocator gevraagd
				require(beheerder == 0);
				beheerder = new WorstFit2(cflag);
				break;
				// enz
			case '2':	// -2 = buddy allocator gevraagd
				require(beheerder == 0);
				beheerder = new ...(cflag);
				break;
			*/

			case -1: // = einde opties
				return; // klaar met optie analyze

			default: // eh? iets onbekends gevonden (of zelf een case vergeten!)
				// foutmelding geven ...
				cerr << AC_RED "Found unknown option '" << char(optopt) << "'" AA_RESET << endl;
				tellOptions(argv[0]);
				exit(EXIT_FAILURE);
		}
	} while (opt != -1);     // tot einde opties
}


// ===================================================================
// A function to prevent our output window from disappearing to soon.
// Needed on some platforms, e.g. windows.

// Niet nodig voor linux of Code::Blocks
// want het programma wordt dan uitgevoerd
// in een gewoon terminal venster.
// Wel nodig voor DevC++ onder windows,
// anders is je venster meteen weg.
// Eclipse vangt alle uitvoer zelf af
// waarbij de volgorde van cerr en cin acties
// helaas ook nog veranderd kan worden.

#if	__DEVCPP__
//#include <conio.h>	// "console I/O"
# include <windows.h>	// non-standard include voor _kbhit(), etc
#endif


/// A function to wait for the user to hit the enter key.
/// This function could be needed on some platforms where otherwise
/// your window disappears immediately on program exit.
void	waitForUserOke()
{
#if		0				/* Needed for DOS ? */
	cout << "Hit ANY key to continue ..." << std::flush;
	if (_kbhit()) {
		char ch = _getch();
	}
#elif	__DEVCPP__		/* Needed for DevC++ on WIN32 */
	cout << "Hit RETURN to continue ..." << std::flush;
	// if 'enter' is already pressed,
	// wait for it to be released
	while (GetAsyncKeyState(VK_RETURN) & 0x8000) {
		;
	}
	// then wait for 'enter' to be pressed
	while (!(GetAsyncKeyState(VK_RETURN) & 0x8000)) {
		;
	}
#else   /* linux of Code::Blocks (not really needed) */
	//std::cout <<"Hit return to continue ..."<<std::flush;
	//std::string  s;
	//std::getline(std::cin, s);
#endif
}


// ===================================================================
#include "Application.h"	// De pseudo applicatie



/// Programma executie begint hier ...
int  main(int argc, char *argv[])
{
	// Dit zorgt ervoor dat 'waitForUserOke' altijd wordt aangeroepen bij programma einde
	atexit(waitForUserOke);

#ifndef unix
	_set_error_mode(_OUT_TO_MSGBOX); // MS magic
#endif

	// Eerst even de random generator wat opschudden
	// Zie manuals: rand(3), time(2) en getpid(2)
	srand(time(0) ^ getpid());
	// Maar kijk ook naar Application::randomscenario() !

	// Hier begint het echte werk
	try {
		// De meegegeven opties afhandelen.
		// neveneffect: zal diverse globale variabelen veranderen!
		doOptions(argc, argv);

		// Is er wel een geheugen-beheerder module gekozen ?
		if (!beheerder) {
			// Ahum, gebruiker's foutje ...
			cerr << AC_RED "Oeps, geen geheugen beheerder gekozen ...." AA_RESET "\n";
			tellOptions(argv[0]);	// vertel wat kan.
			exit(EXIT_FAILURE);
		}

		// Omvang van het beheerde geheugen controleren
		check(size > 0);

		// Vertel het aan de geheugen-beheerder ...
		beheerder->setSize(size);

		// ... en maak dan de pseudo-applicatie
		Application  *mp = new Application(beheerder, size);

		if (tflag) {    // De -t optie gezien ?
			cerr << AC_BLUE "Testing " << beheerder->getType()
			     << " with " << size << " units\n" AA_RESET;
			mp->testing(); // ga dan de code testen
		} else {
			// Het aantal allocaties moet minstens 2 zijn
			// (ivm berekening standaard deviatie!)
			// Bij een random-scenario is dit helaas geen garantie
			// van correctheid omdat er ook nog 'free' acties zijn.
			if (aantal < 2) {
				cerr << AC_RED "Het aantal moet minstens 2 zijn" AA_RESET "\n";
				exit(EXIT_FAILURE);
			}
			cerr << AC_BLUE "Measuring " << beheerder->getType()
			     << " doing " << aantal << " calls on " << size << " units\n" AA_RESET;
			mp->randomscenario(aantal, vflag);
			/// .. vervang straks 'randomscenario' door iets toepasselijkers
			/// zodat je ook voorspelbare scenarios kan afhandelen.
		}

		// Nu alles weer netjes opruimen
		delete  mp;
		delete  beheerder;

	} catch (const char *error) {
		cerr << AC_RED"OEPS: " << error << AA_RESET << endl;
		return EXIT_FAILURE;
	} catch (const std::exception& error) {
		cerr << error.what() << endl;
		return EXIT_FAILURE;
	} catch(...) {
		cerr << AC_RED "OOPS: something went wrong" AA_RESET << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

// vim:sw=4:ai:aw:ts=4:

#include "assert_error.h"
#include "ansi.h"   // for: ansi color strings
#include <string>
#include <sstream>	// for: ostringstream
using namespace std;

/** @file assert_error.cc
 * This file implements the assert_error exception class.
 *
 * @author R.A.Akkersdijk@saxion.nl
 * @version 3.1	2013/11/10
 */


static
// The assertion was not met, inform the user what is wrong.
string make_message(const char *file, const int line, const char *func,
							const char *type, const char *expr)
{
	ostringstream  ss;
	ss << AC_RED<<file<<'['<<line<<"]";
	if (func)
		ss << " in "<<func;
	ss << "\n\t"<<type<<'('<<AA_RESET<<expr<<AC_RED<<") failed"AA_RESET"\n";
	return ss.str();
}


/** @class assert_error
 * If an assertion fails an instance of assert_error is thrown.
 */
assert_error::assert_error(const char *file, const int line, const char *func,
							const char *type, const char *expr)
	: logic_error( make_message(file,line,func,type,expr) ) {}

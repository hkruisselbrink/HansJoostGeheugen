#pragma once
#ifndef	__assert_error_h
#define	__assert_error_h 3.1
//#ident "@(#)assert_error.h	3.1	AKK	2013/11/03"


/** @file assert_error.h
 * This file defines the assert_error exception class.
 *
 * @author R.A.Akkersdijk@saxion.nl
 * @version 3.1	2013/11/10
 */

#include <stdexcept>	// std::exception, std::logic_error


/** @class assert_error
 * If an assertion fails an instance of assert_error is thrown.
 */
class assert_error : public std::logic_error
{
public:
	/**
	 * Constructs the exception object with a message string composed of:
	 *   the file-name, the line number, the function where the error occured,
	 *   the type of the exception and the test that failed.
	 */
	assert_error(const char *file, const int line, const char *func,
					const char *type, const char *expr);

	/** Constructs the exception object with what_arg as explanatory
	 * string that can be accessed through what().
	 */
	explicit assert_error(const std::string& what_arg)
				: logic_error(what_arg) {}

	/** Constructs the exception object with what_arg as explanatory
	 * string that can be accessed through what().
	 */
	explicit assert_error(const char *what_arg)
				: logic_error(what_arg) {}
};

#endif	/*__assert_error_h*/
// vim:sw=4:ai:aw:ts=4:

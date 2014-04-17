/** @file Command.cc
 * Implementation of class Command.
 */
#include <iostream>
#include <cstdlib>		// for: getenv()
#include <unistd.h>		// for: getcwd(), close(), execv(), access()
#include <limits.h>		// for: PATH_MAX
#include <fcntl.h>		// for: O_RDONLY, O_CREAT, O_WRONLY, O_APPEND
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "asserts.h"
#include "unix_error.h"
#include "Command.h"
using namespace std;


// Iff PATH_MAX is not defined in limits.h
#ifndef	PATH_MAX
# define	PATH_MAX (4096)	/* i.e. virtual pagesize */
#endif


Command::Command()
	: append(false)
{
}


void	Command::addWord(string& word)
{
	words.push_back(word);
}


void	Command::setInput(std::string& the_input)
{
	require(input.empty());		// catch multiple inputs
	input = the_input;
}

void	Command::setOutput(std::string& the_output)
{
	require(output.empty());	// catch multiple outputs
	output = the_output;
	append = false;
}

void	Command::setAppend(std::string& the_output)
{
	require(output.empty());	// catch multiple outputs
	output = the_output;
	append = true;
}

// A true "no-nothing" command?
bool	Command::isEmpty() const
{
	return input.empty() && output.empty() && words.empty();
}

bool    Command::hasCd()
{
    for (int i = 0 ; i < words.size() ; ++i) {
        if(words[i] == "cd")
            return true;
    }
    return false;
}

bool Command::hasExit()
{
    for (int i = 0 ; i < words.size() ; ++i) {
        if(words[i] == "exit")
            return true;
    }
    return false;
}


// ===========================================================


// Execute a command
void	Command::execute()
{
    //Parameters converten
    char *args[ words.size() + 1 ];
    for(std::size_t i = 0; i != words.size(); ++i) {
        args[i] = &words[i][0];
    }
    args[ words.size() ] = 0;

    //Path bepalen


    //Command uitvoeren
    if(output.length() > 0) {
        char *fileName = (char*)output.c_str();
        int fd;

        if(!append) {
            fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        } else {
            fd = open(fileName, O_APPEND | O_RDWR, S_IRUSR | S_IWUSR);
        }

        dup2(fd, 1);
        dup2(fd, 2);
        close(fd);
        execvp(args[0], args);

	} else if (input.length() > 0) {

        char *fileName = (char*)input.c_str();
        int fd = open(fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        dup2(fd, 0);
        close(fd);
        execvp(args[0], args);

	} else if(hasCd()) {
        if (chdir(args[1]) < 0)
            cerr << "No such file or directory" << endl;

    } else if(hasExit()) {
        exit(0);
    } else {
        char* path = getenv("PATH");
        strtok(path, ":");
        strcat(path, "/");
        strcat(path, args[0]);

        //cerr << "PATH="<< path << endl;
        char dir[80];
        getcwd(dir, 80);
        strcat(dir, "/");
        strcat(dir, args[0]);
        //cerr << "DIR=" << dir << endl;

        execv(path, args);
        cerr << "command failed";
        exit(0);
    }

	// TODO:	Handle I/O redirections.
	// TODO:	Convert the words vector<string> to: array of (char*) as expected by 'execv'.
	//			NOTE: Make sure the last element of that array will be a 0 pointer!
	// TODO:	Determine the full name of the program to be executed.
	// 			If the name begins with '/' it already is an full name.
	// 			If the name begins with "./" it is expressed relative to the
	// 			current directory.
	// 			Otherwise it is to be searched for using the PATH
	// 			environment variable.
	// TODO:	Execute the program passing the arguments array.
	// Also see: close(2), open(2), getcwd(3), getenv(3), access(2), execv(2), exit(2)

#if 0	/* DEBUG code: Set to 0 to turn off the next block of code */
	cerr <<"Command::execute ";
	// Show the I/O redirections first
	if (!input.empty())
		cerr << " <"<< input;
	if (!output.empty()) {
		if (append)
			cerr << " >>"<< output;
		else
			cerr << " >"<< output;
	}
	// Then show the command & parameters to execute
	if (words.empty())
		cerr << "\t(DO_NOTHING)";
	else
		cerr << "\t";
	for (vector<string>::iterator  i = words.begin() ; i != words.end() ; ++i)
		cerr << " " << *i;
	cerr << endl;
#endif	/* end DEBUG code */

	// TODO

}


// vim:ai:aw:ts=4:sw=4:


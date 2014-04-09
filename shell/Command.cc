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


// ===========================================================


// Execute a command
void	Command::execute()
{
	cerr << "Command:execute\n";//DEBUG

    //Path variabele instellen
    string env = getenv("PATH");
    char* pPath = new char[env.size()+1];
    pPath[env.size()]=0;
    memcpy(pPath,env.c_str(),env.size());
    strtok(pPath, ":");
    strcat(pPath, {"/"});

    //Working directory bepalen
    long size;
    char *buf;
    char *ptr;

    size = pathconf(".", _PC_PATH_MAX);

    if ((buf = (char *)malloc((size_t)size)) != NULL) {
        ptr = getcwd(buf, (size_t)size);
        strcat(ptr, {"/"});
    }

    //Parameters converten
    vector<char *> args(words.size() + 1);
    for(std::size_t i = 0; i != words.size(); ++i) {
        args[i] = &words[i][0];
    }

    //Wordt cd aangeroepen?
    if(strcmp(args[0], "cd") == 0) {
        strcat(ptr, args[1]);
        if(chdir (ptr) == -1) {
            cerr << "chdir failed" << endl;
        } else {
            return;
        }
    } else if (strcmp(args[0], "cd..") == 0) {
        if(chdir ("..") == -1) {
            cerr << "chdir failed" << endl;
        } else {
            return;
        }
    }


    int cid = fork();
    if(cid == 0) {
        execl(strcat(pPath, args[0]), args[0], args[1], NULL);
        cerr << "execl failed" << endl;
    } else if(cid > 0) {
        wait( (int*) 0);
        cerr << "Is completed" << endl;
        //exit(0);
    } else
        cerr << "fork failed" << endl;



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


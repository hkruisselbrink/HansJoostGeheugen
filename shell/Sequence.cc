/** @file Sequence.cc
 * Implementation of class Sequence.
 */
#include <iostream>
#include <sys/wait.h>		// for: wait(), WIF...(status)
#include <unistd.h>			// for: fork(), nice()
#include <cstring>			// for: strsignal()
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "asserts.h"
#include "unix_error.h"
#include "Sequence.h"
using namespace std;


void  Sequence::addPipeline(Pipeline *pp)
{
	require(pp != 0);
	commands.push_back(pp);
}


Sequence::~Sequence()
{
	for (vector<Pipeline*>::iterator  i = commands.begin() ; i != commands.end() ; ++i)
		delete  *i;
}


bool	Sequence::isEmpty()	const
{
	return commands.empty();
}


// TODO:	Lookout somewhere for special commands such as 'exit',
//			'logout', 'cd', etc, which may have to be
//			done by the shell itself.


// Execute the pipelines in this sequence one by one
void	Sequence::execute()
{
	//cerr << "Sequence::execute\n";//DEBUG
    int status;
	// Execute each pipeline in turn.
	// Also see: fork(2), nice(2), wait(2), WIF...(2), strsignal(3)
	size_t  j = commands.size();			// for count-down
	for (vector<Pipeline*>::iterator  i = commands.begin() ; i != commands.end() ; ++i, --j)
	{
		Pipeline  *pp = *i;
		if (!pp->isEmpty())
		{
			if (j == commands.size()) {//DEBUG
				//cerr << "Sequence::FIRST PIPELINE\n";//DEBUG
			}//DEBUG
			// if (pp->isBuiltin()) ...

            if (!pp->hasCd() && !pp->hasExit()) {
                int pid = fork();
                if(pid == 0) {
                    pp->execute();
                } else if (pid > 0) {
                    if(!pp->isBackground() || j == 1) {
                        wait(NULL);
                    }

                }
            } else {
                pp->execute();
            }

			// TODO
			if (j == 1) {//DEBUG
				//cerr << "Sequence::LAST PIPELINE\n";//DEBUG
			} else {//DEBUG
				//cerr << "Sequence::WAIT FOR PIPELINE\n";//DEBUG
			}//DEBUG
		}
		// else ignore empty pipeline
	}
}


// vim:ai:aw:ts=4:sw=4:


/** @file Pipeline.cc
 * Implementation of class Pipeline.
 */
#include <iostream>
#include <unistd.h>		// for: pipe(), fork(), dup2(), close()
#include <fcntl.h>		// for: O_RDONLY, O_CREAT, O_WRONLY, O_APPEND
#include <signal.h>		// for: signal, SIG*
#include <stdlib.h>
#include <sys/wait.h>
#include "asserts.h"
#include "unix_error.h"
#include "Pipeline.h"
using namespace std;


Pipeline::Pipeline()
	: background(false)
{
}


void	Pipeline::addCommand(Command *cp)
{
	require(cp != 0);
	commands.push_back(cp);
}


Pipeline::~Pipeline()
{
	for (vector<Command*>::iterator  i = commands.begin() ; i != commands.end() ; ++i)
		delete  *i;
}


bool	Pipeline::isEmpty()	const
{
	return commands.empty();
}

void    my_handler(int signum)
{
    exit(signum);
}

bool    Pipeline::hasCd()
{
    for (vector<Command*>::iterator  i = commands.begin() ; i != commands.end() ; ++i) {
        Command *cp = *i;
        if(cp->hasCd())
            return true;
    }
    return false;
}

bool Pipeline::hasExit()
{
    for (vector<Command*>::iterator  i = commands.begin() ; i != commands.end() ; ++i) {
        Command *cp = *i;
        if(cp->hasExit())
            return true;
    }
    return false;
}

void (*signal (int sig, void (*func)(int)))(int);

// Execute the commands in this pipeline in parallel
void	Pipeline::execute()
{
	//cerr << "Pipeline::execute\n";//DEBUG

	// Because we want the shell to wait on the rightmost process only
	// we must created the various child processes from the right to the left.
	// Also see: pipe(2), fork(2), dup2(2), dup(2), close(2), open(2), signal(2).
	// Maybe also usefull for debugging: getpid(2), getppid(2).
    if(background == 0) {
        signal(SIGINT, my_handler);
        signal(SIGQUIT, my_handler);
    }

	size_t	 j = commands.size();		// for count-down
    int fd[2];
    //pipe(fd);

	for (vector<Command*>::reverse_iterator  i = commands.rbegin() ; i != commands.rend() ; ++i, --j)
	{
		Command  *cp = *i;
		if (j == commands.size()) {//DEBUG
			//cerr << "Pipeline::RIGHTMOST PROCESS\n";//DEBUG

		}

		if(commands.size() > 1) {
            if(j == 1) {
                cp->execute();
            } else {
                if(pipe(fd) == -1) {
                    cerr << "pipe failed" << endl;
                }

                switch(fork()) {
                    case -1:
                        cerr << "fork failed" << endl;
                    case 0:
                        if(dup2(fd[1], 1) < 0) {
                            cerr << "Err dup2 in child" << endl;
                        }
                        close(fd[0]);
                        close(fd[1]);
                        break;
                    default:
                        if(dup2(fd[0], 0) < 0) {
                            cerr << "Err dup2 in parent" << endl;
                        }
                        close(fd[0]);
                        close(fd[1]);
                        cp->execute();
                        break;
                }
            }
        } else if(commands.size() == 1){
            cp->execute();
        }

		if (j == 1) {//DEBUG
			//cerr << "Pipeline::LEFTMOST PROCESS\n";//DEBUG
			//cp->execute();
		} else {//DEBUG
			//cerr << "Pipeline::CONNECT TO PROCESS\n";//DEBUG
		}//DEBUG
	}
}

// vim:ai:aw:ts=4:sw=4:


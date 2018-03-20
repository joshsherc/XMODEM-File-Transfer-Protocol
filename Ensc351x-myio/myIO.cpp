//============================================================================
//
//% Student Name 1: Joshua Shercliffe
//% Student 1 #: 301294276
//% Student 1 userid (email): jshercli (jshercli@sfu.ca)
//
//% Student Name 2: Layomi Dele-Dare
//% Student 2 #: 301136462
//% Student 2 userid (email): ldeledar (ldeledar@sfu.ca)
//
//% Below, edit to list any people who helped you with the code in this file,
//%      or put 'None' if nobody helped (the two of) you.
//
// Helpers: Maryam the TA helped a lot with this code.__
//
// Also, list any resources beyond the course textbooks and the course pages on Piazza
// that you used in making your submission.
//
// Resources:  ___________
//
//%% Instructions:
//% * Put your name(s), student number(s), userid(s) in the above section.
//% * Also enter the above information in other files to submit.
//% * Edit the "Helpers" line and, if necessary, the "Resources" line.
//% * Your group name should be "P2_<userid1>_<userid2>" (eg. P1_stu1_stu2)
//% * Form groups as described at:  https://courses.cs.sfu.ca/docs/students
//% * Submit files to courses.cs.sfu.ca
//
// File Name   : myIO.cpp
// Version     : October, 2017
// Description : Wrapper functions for ENSC-351 -- Starting point for ENSC 351 Project Part 3
// Original portions Copyright (c) 2017 Craig Scratchley  (wcs AT sfu DOT ca)
//============================================================================

#include <unistd.h>			// for read/write/close
#include <fcntl.h>			// for open/creat
#include <sys/socket.h> 		// for socketpair
#include "SocketReadcond.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <iostream>
#include <sys/ioctl.h>
using namespace std;

class socketPairClass
{
public:
	std::condition_variable cond_var;
	int buffered;
	int socketPair;
	std::mutex local_mutex;
};

std::vector<socketPairClass*> mutexes;

int mySocketpair( int domain, int type, int protocol, int des[2] )
{
	//create objects
	int returnValue = socketpair(domain, type, protocol, des);
	mutexes.resize(max(des[0], des[1])+1);
	mutexes[des[0]] = new socketPairClass;
	mutexes[des[1]] = new socketPairClass;
	mutexes[des[0]]->socketPair = des[1];
	mutexes[des[1]]->socketPair = des[0];
	return returnValue;
}

int myTcdrain(int des)
{

	int currentlyPairedSocket = mutexes[des]-> socketPair;
	//lock the mutexes
	std::unique_lock<mutex>lk(mutexes[currentlyPairedSocket]->local_mutex);
	//wait
	mutexes[currentlyPairedSocket]->cond_var.wait(lk, [=]{return ((mutexes[currentlyPairedSocket]->buffered) <=0);});
	//unlock
	lk.unlock();
	return 0;
}


int myOpen(const char *pathname, int flags, mode_t mode)
{
	return open(pathname, flags, mode);
}

int myCreat(const char *pathname, mode_t mode)
{
	return creat(pathname, mode);
}


int myReadcond(int des, void * buf, int n, int min, int time, int timeout)
{

	int bytesSent = 0;
	int myReadReturnValue = 0;
	ioctl(des, FIONREAD, &bytesSent);
	while(bytesSent < (min) )
    {
		//lock
		std::lock_guard<std::mutex> lock(mutexes[des]->local_mutex);
		mutexes[des]->buffered -= bytesSent;
		//notify  all
    	mutexes[des]->cond_var.notify_all();
		ioctl(des, FIONREAD, &bytesSent);
    }
	//lock
	std::lock_guard<std::mutex> lock(mutexes[des]->local_mutex);
	myReadReturnValue = wcsReadcond(des, buf, n, min, time, timeout );
	//decrement buffer until reaches zero
	mutexes[des]-> buffered -= myReadReturnValue;
	//when it has reached zero, notify
	if(mutexes[des]->buffered == 0)
	{
		mutexes[des]->cond_var.notify_all();
	}
	return myReadReturnValue;
}


ssize_t myRead( int fildes, void* buf, size_t nbyte )
{
	//calling myreadcond
	ssize_t myReadReturnValue =  myReadcond(fildes, buf, nbyte, 1, 0, 0);
	return myReadReturnValue;
}

ssize_t myWrite( int fildes, const void* buf, size_t nbyte )
{
	ssize_t myWriteReturnValue;
	int currentlyPairedSocket = mutexes[fildes]->socketPair;
	//lock
	std::unique_lock<std::mutex> lk(mutexes[fildes]->local_mutex);
	myWriteReturnValue = write(fildes, buf, nbyte);
	//increment buffer - (counting number of bytes)
	mutexes[currentlyPairedSocket]-> buffered += myWriteReturnValue;
	return myWriteReturnValue;
}

int myClose( int fd )
{
	//closes the sockets
	auto pntr = mutexes[fd];
	mutexes[fd]=NULL;
	delete pntr;
	return close(fd);
}
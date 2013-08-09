/*
 * MIMTLU.h
 *
 *  Created on: May 7, 2013
 *      Author: mbenoit
 */

#ifndef MIMTLU_H_
#define MIMTLU_H_

#include <sys/socket.h> 
#include <netdb.h>     
#include <iostream>
#include <fstream>
#include <cstring> 
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>

struct mimtlu_event
{
	unsigned long int timestamp;
	unsigned char track;
	unsigned int tlu;
	mimtlu_event(unsigned long int _timestamp,unsigned char _track,	unsigned int _tlu)
	{
		timestamp=_timestamp;
		track=_track;
		tlu=_tlu;
	}
	mimtlu_event(const unsigned char *buf)
	{
		from_string(buf);
	}

	void from_string(const unsigned char *buf)
	{
	}
};

class MIMTLU {
public:
	MIMTLU();
	int               Connect(char* IP,char* port);
	std::vector<mimtlu_event> GetEvents(void);
	void              Arm(void);
	void              SetNumberOfTriggers(const unsigned int n);


private :
	int NTrigger;
	int status;
	struct addrinfo host_info;       
	struct addrinfo *host_info_list; 
	int socketfd ; 
	int len, bytes_sent;
	ssize_t bytes_recieved;
	char msg [1024];
	char incoming_data_buffer[40*9];
	unsigned long int tluevtnr;
};

#endif /* MIMTLU_H_ */

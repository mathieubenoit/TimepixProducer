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



class MIMTLU {
public:
	MIMTLU();
	int Connect(char* IP,char* port);
	int Arm();
	int GetEvent();


private :

	int status;
	struct addrinfo host_info;       
	struct addrinfo *host_info_list; 
	int socketfd ; 
	int len, bytes_sent;
	ssize_t bytes_recieved;
	char msg [1024];
	char incoming_data_buffer[1024];
	unsigned long int tluevtnr;



};

#endif /* MIMTLU_H_ */

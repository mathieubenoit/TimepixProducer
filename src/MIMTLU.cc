#include "MIMTLU.h"


using namespace std;

MIMTLU::MIMTLU() {
	// TODO Auto-generated constructor stub

}



int MIMTLU::Connect(char* IP,char* port){

	memset(&host_info, 0, sizeof host_info);
	host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
	host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.


	// Address
	status = getaddrinfo(IP,port, &host_info, &host_info_list);
	if (status != 0)
		{
		cout << "[MIMTLU] getaddrinfo error" << gai_strerror(status) << endl;
		return -1;
		};

	// Socket
	socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
			host_info_list->ai_protocol);

	if (socketfd == -1) {
				std::cout << "[MIMTLU] Socket error " << endl;
				return -2;
	};

	// Connection
	status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
	if (status == -1)  {
		std::cout << "connect error " << gai_strerror(status) << " " << host_info.ai_flags << endl;
		return -3;
	}
	else {
		std::cout << "[MIMTLU] connect established " << endl;
		return 1;

	}




}


int MIMTLU::Arm(){

#ifdef DEBUGPROD	
	std::cout << "ARM"<<std::endl;
#endif
        memset(msg,0,1024);
	sprintf(msg,"READ");
	len = sizeof(msg);
	bytes_sent = send(socketfd,msg, 8, 0);
	
//	incoming_data_buffer= 0;
	bytes_recieved = recv(socketfd, incoming_data_buffer,8, 0);

//std::cout << incoming_data_buffer<<std::endl;

	// If no data arrives, the program will just wait here until some data arrives.
	if (bytes_recieved == 0) {
		std::cout << "host shut down." << std::endl ;
		return -100;
	}
	else if (bytes_recieved == -1){
		std::cout << "recieve error!" << std::endl ;
		return -200;
	}
	else{
	//std::cout << incoming_data_buffer<<std::endl;
		return 0;
	}

}

int MIMTLU::GetEvent(){

        memset(msg,0,1024);
#ifdef DEBUGPROD		
	std::cout << "READ"<<std::endl;
#endif
//	sprintf(msg,"READ");
//	len = sizeof(msg);
//	bytes_sent = send(socketfd,msg, 8, 0);

	//incoming_data_buffer= 0;
	bytes_recieved = recv(socketfd, &incoming_data_buffer,18, 0);
	// If no data arrives, the program will just wait here until some data arrives.

#ifdef DEBUGPROD		
	std::cout << "buffer : " << incoming_data_buffer<<std::endl;
#endif
	
	if (bytes_recieved == 0) {
		std::cout << "host shut down." << std::endl ;
		return -100;
	}
	else if (bytes_recieved == -1){
		std::cout << "recieve error!" << std::endl ;
		return -200;
	}
	else{
		sscanf(incoming_data_buffer,"%x",&tluevtnr);
		//std::cout << "read "<<tluevtnr<<std::endl;
		
		return tluevtnr;
	}
	
	
}

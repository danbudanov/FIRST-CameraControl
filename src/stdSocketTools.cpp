#include "stdSocketTools.h"

std::string incomingAddr(struct sockaddr_in ADDR){
	//gets the address of the incoming connection
	//http://stackoverflow.com/questions/2064636/getting-the-source-address-of-an-incoming-socket-connection
	char ipstr[INET6_ADDRSTRLEN + 1];

	// deal with both IPv4 and IPv6:
	if (ADDR.sin_family == AF_INET) {
		//printf("AF_INET\n");
	    inet_ntop(AF_INET, &ADDR.sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
		//WARNING: THIS MAY BREAK
		//this ipv6 implementation may be broken
		//this is NOT tested
		//printf("AF_INET6\n");
	    inet_ntop(AF_INET6, &ADDR.sin_addr, ipstr, sizeof ipstr);
	}

	std::string temp=ipstr;
	return temp;
}
int simpleOpenSocket(){
	return socket(AF_INET, SOCK_STREAM, 0);
}
int simpleOpenListenSocket(int port){
	//opens a socket that is listening on a certain port

	struct sockaddr_in serv_addr;
	int listenFD;

	listenFD = socket(AF_INET, SOCK_STREAM, 0);
	if (listenFD < 0){
        //error("ERROR opening socket");
        return -1;
    }
    serv_addr.sin_family = AF_INET; //internet domain, not unix domain
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); //allow us to listen to all interfaces
	serv_addr.sin_port = htons(port); //change order of the bytes from the host specific to the network order (so dont worry about Big or Little endian)
	if (bind(listenFD, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		//error("ERROR on binding");
		return -1;
	}
	listen(listenFD,15);//start listening so that we can get new connections
	return listenFD;
}

struct newConnectionInfo simpleAccept(int socketFD){
	//listens to a port until a person connects
	//this then sends back the basic info
	//    file descriptor of the new connection
	//    IP address of the incomming person
	struct sockaddr_in cli_addr;
	socklen_t clientLength = sizeof(cli_addr);//size of the struct
	int newSockFD = accept(socketFD, (struct sockaddr *) &cli_addr, &clientLength); //get the FD of the new client connection
	if (newSockFD < 0) 
	    printf("ERROR on accept");

	struct newConnectionInfo peer;
	peer.FD=newSockFD;
	strcpy(peer.address,incomingAddr(cli_addr).c_str());
	return peer;
}

int simpleConnectToHost(std::string ADDR,int port){
	//given a destination, will return a socket to talk with
	int socketFD=simpleOpenSocket();
	if(socketFD<0) return -1; //error

	struct hostent *server = gethostbyname(ADDR.c_str());
	if(server==NULL){
		close(socketFD);
		return -1; //error
	}

	struct sockaddr_in serv_addr;
	//setup the connection info
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(port);

    if (connect(socketFD,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0){
		close(socketFD);
		return -1; //error
	}

	return socketFD;
}

int simpleOpenSocket_UDP(int port){
	int socketFD=socket(AF_INET,SOCK_DGRAM,0);
	if(allowBroadcast_UDP(socketFD))
		printf("ERROR getting broadcast permissions\n");
	struct sockaddr_in currentMachine;
	bzero((char*)&currentMachine,sizeof(currentMachine));
	currentMachine.sin_family=AF_INET; //internet domain
	currentMachine.sin_addr.s_addr=htonl(INADDR_ANY); //any random address
	currentMachine.sin_port=htons(port);
	bind(socketFD,(struct sockaddr*)&currentMachine,sizeof(currentMachine));
	return socketFD;
}

int allowBroadcast_UDP(int socketFD){
	int broadcastEnable=1;
	char interface[]="eth0";
	setsockopt(socketFD, SOL_SOCKET, SO_BINDTODEVICE, interface, sizeof(interface));
	return setsockopt(socketFD, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));
}

int sendBroadcast_UDP(int socketFD,int port,std::string message){
	//itterates over ALL interfaces to send the message

	struct sockaddr_in bcast;
	bzero((char*)&bcast,sizeof(bcast));
	bcast.sin_family=AF_INET;
	bcast.sin_port=htons(port);
	bcast.sin_addr.s_addr=htonl(INADDR_BROADCAST);

	//host lookup
	//struct hostent *host;
	//host = gethostbyname("255.255.255.255");
	//memcpy((void *)&bcast.sin_addr, host->h_addr_list[0], host->h_length);
	return sendto(socketFD,message.c_str(),message.length(),0,(struct sockaddr*)&bcast,sizeof(bcast));
}
int send_UDP(int socketFD,int port,std::string hostName,std::string message){
	//host lookup
	struct hostent *host;
	host = gethostbyname(hostName.c_str());

	//sending the stuff
	struct sockaddr_in servaddr;
	bzero((char*)&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	memcpy((void *)&servaddr.sin_addr, host->h_addr_list[0], host->h_length);
	return sendto(socketFD,message.c_str(),message.length(),0,(struct sockaddr*)&servaddr,sizeof(servaddr));
}

struct newConnectionInfo listen_UDP(int socketFD){
	struct newConnectionInfo peer;

	struct sockaddr_in bcast;
	bzero((char*)&bcast,sizeof(bcast));
	char buf[1024];
	unsigned int bcastLength=sizeof(bcast);
	unsigned int charsRead = recvfrom(socketFD,buf,1023,0,(struct sockaddr*)&bcast,&bcastLength);

	buf[charsRead]=0; //end the string with a zero
	peer.FD=0;
	bcast.sin_family = AF_INET;
	strcpy(peer.address,incomingAddr(bcast).c_str());
	//printf("listen_UPD:  %s\n",peer.address);
	peer.message=buf;

	return peer;
}

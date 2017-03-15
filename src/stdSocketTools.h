#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <pthread.h>
#include <string>
#ifndef standardSocketTools
#define standardSocketTools

/*
This provides several common basic tools that dont deserve their own headers
This makes it a collection of smaller functions
*/

//TCP
std::string incomingAddr(struct sockaddr_in ADDR); //gives the IP address of the connection
int simpleOpenSocket();
int simpleOpenListenSocket(int port); //opens a socket ready to accept connections on a certain port
struct newConnectionInfo simpleAccept(int socketFD); //on a socket that is listening, blocks for a new connection
int simpleConnectToHost(std::string ADDR,int port); //simlpe connect to a host on a given port

//UDP
int simpleOpenSocket_UDP(int port);
int allowBroadcast_UDP(int socketFD);
int sendBroadcast_UDP(int socketFD,int port,std::string message);
int send_UDP(int socketFD,int port,std::string hostName,std::string message);
struct newConnectionInfo listen_UDP(int socketFD);

struct newConnectionInfo{
	int FD;
	char address[16]; //just bearly big enough for IPv4 addresses
	std::string message;
};

#endif
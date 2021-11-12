
#include<chrono>
#include "../headers/common.hpp"

using namespace std::chrono;

int localClock = 0;

int generateRandomClockNumber(){
	return (rand() % 10) * getpid();
}

int main(int argc, char **argv) {

    // exit the program if invalid arguments are passed
	if(argc != 3 ){
		cout<<"ERROR: Invalid Arguments! Please run the program with args as ./<exe file> <IP address> <server port>"<<endl;
		exit(EXIT_FAILURE);
	}

	int clientFd;
	int serverPort = stoi(argv[2]);
	char* serverAddress = argv[1];
	struct sockaddr_in serverSocketAddr;

    localClock = generateRandomClockNumber();

	// Create socket (IPv4, stream-based, protocol set to TCP)
	if((clientFd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		cout<<"ERROR:: Client failed to create socket"<<endl;
		exit(EXIT_FAILURE);
	}
	
	// Configure server socket address structure (init to zero, IPv4,
	// network byte order for port and address) 
	bzero(&serverSocketAddr, sizeof(serverSocketAddr));
	serverSocketAddr.sin_family = AF_INET;
	serverSocketAddr.sin_port = htons(serverPort);
	serverSocketAddr.sin_addr.s_addr = inet_addr(serverAddress);

	// Connect socket to server
	if(connect(clientFd, (struct sockaddr *)&serverSocketAddr, sizeof(serverSocketAddr)) < 0){
		cout<<"ERROR:: Client failed to connect to "<<serverAddress<<" : "<<serverPort<<endl;
		close(clientFd);
		exit(EXIT_FAILURE);
	} else {
		cout<<"SUCCESS:: Client connected to "<<serverAddress<<" : "<<serverPort<<endl;
		
        char buffer[1024];
        bzero(buffer,1024);
		int serverClock = 0;
		int valread = recv(clientFd, (void*) &buffer, sizeof(buffer), 0);

		if(valread > 0){
			cout<<"SUCCESS:: Received data from daemon"<<endl;
			cout<<"My local time:: "<<localClock<<endl;
			serverClock = stoi(buffer);
			cout<<"Server's local time:: "<<serverClock<<endl;
			//calculateAndSendOffset(serverClock, clientFd);
			int difference = localClock - serverClock;
			cout<<"clock difference:: "<<difference<<endl;

			cout<<"sending clock difference "<<difference<<" to the daemon"<<endl;
            bzero(buffer,1024);
			strcpy(buffer,to_string(difference).c_str());
			int valsend = send(clientFd, buffer, sizeof(buffer), 0);
			cout<<"valsend:: "<<valsend<<endl;
			if(valsend < 0){
				cout<<"ERROR:: Cannot send clock difference to the daemon !"<<endl;
			}

			bzero(buffer,1024);
			int valread2 = recv(clientFd, (void*) &buffer, sizeof(buffer), 0);
			if(valread2 > 0){
				cout<<"FINAL:: SUCCESS:: Received data from daemon"<<endl;
				cout<<"int valread2:: "<<valread2<<endl;
				cout<<"buffer is :: "<<buffer<<endl;
				int adjustClk = stoi(buffer);
				cout<<"clock offset to be adjusted by:: "<<adjustClk<<endl;
				localClock += adjustClk;
				cout<<"FINAL SYNC'D TIME:: "<<localClock<<endl;
			}
            bzero(buffer,1024);
		}
	}

	// Close the socket and return 0
	close(clientFd);
	return 0; 
}
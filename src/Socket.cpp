#include "Server.hpp"

void	Server::createSocket(void) {
	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(_port);
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1) {
		std::cout << "ERROR: Socket creation failed.\n";
		exit(1);
	}
	if (bind(_serverSocket, (sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1) {
		close (_serverSocket);
		std::cout << "ERROR: Socket binding failed.\n";
		exit(1);
	}
	if (listen(_serverSocket, _maxOnline) == -1) {
		close(_serverSocket);
		std::cout << "ERROR: Socket listening failed.\n";
		exit (1);
	}
}
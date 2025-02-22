#include "Server.hpp"

void	Server::createSocket(void) {
	int	tmp = 1;

	_serverAddr.sin_family = AF_INET;
	_serverAddr.sin_port = htons(_port);
	_serverAddr.sin_addr.s_addr = INADDR_ANY;
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1) {
		std::cout << "ERROR: Socket creation failed.\n";
		return;
	}
	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) == -1) {
		close(_serverSocket);
		std::cout << "Server fcntl failed\n";
		return;
	}
	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp)) == -1) {
		close(_serverSocket);
		std::cout << "ERROR: Setsockopt failed.\n";
		return;
	}
	if (bind(_serverSocket, (sockaddr *)&_serverAddr, sizeof(_serverAddr)) == -1) {
		close (_serverSocket);
		std::cout << "ERROR: Socket binding failed.\n";
		return;
	}
	if (listen(_serverSocket, _maxOnline) == -1) {
		close(_serverSocket);
		std::cout << "ERROR: Socket listening failed.\n";
		return;
	}
	_listening = true;
}
#include "Server.hpp"

Server::Server(): _port(0), _password("0000"), _maxOnline(5), _currentOnline(0), _pfd(NULL), _serverSocket(0), _serverAddr() {}

Server::Server(int port, char *password, int maxOnline): _port(port), _password(password) {
	_maxOnline = maxOnline + 2;
	createSocket();
	_pfd = new pollfd[_maxOnline];
	_pfd[0].fd = _serverSocket;
	_pfd[0].events = POLLIN;
	_pfd[1].fd = 0;
	_pfd[1].events = POLLIN;
	_currentOnline = 2;
}

Server::~Server() {
	std::cout << "[ Server destructor called ]" << std::endl;
}

void	Server::removeClient(int i) {
	std::string	channelName = _serverClients[_pfd[i].fd]->getChannel();

	if (channelName != "\0")
		_serverChannels[channelName]->removeClient(_pfd[i].fd);
	_serverClients.erase(_pfd[i].fd);
	close(_pfd[i].fd);
	_pfd[i] = _pfd[_currentOnline - 1];
	_currentOnline--;
}

void	Server::newConnection(void) {
	sockaddr_in	clientAddr;
	socklen_t	clientLen = sizeof(clientAddr);

	int	cfd = accept(_serverSocket, (sockaddr *)&clientAddr, &clientLen);
	if (cfd == -1) {
		std::cout << "Client connection failed\n";
		return ;
	}
	if (_currentOnline == _maxOnline) {
		std::cout<< "Client [" << cfd << "] : connection failed (server is full).\n";
		send(cfd, "ERROR: Server is full.\n", 23, 0);
		close (cfd);
		return ;
	}
	_pfd[_currentOnline].fd = cfd;
	_pfd[_currentOnline].events = POLLIN;
	_currentOnline++;
	_serverClients.insert(std::pair<int, Client *>(cfd, new Client(cfd)));
	connectionMsgServ(cfd);
	connectionMsgClient(cfd);
}

void	Server::startServer(void) {
	int	tmpPfd;

	while (1) {
		tmpPfd = poll(_pfd, _currentOnline, -1);
		if (tmpPfd < 0) {
			std::cout << "ERROR: Poll error.\n";
			exit (1);
		}
		for (int i = 0; i < _currentOnline; ++i) {
			if (_pfd[i].revents & POLLIN) {
				if (_pfd[i].fd == _serverSocket) {
					newConnection();
				}
				else if (_pfd[i].fd == 0 && i == 1) {
					serverInput();
				}
				else
					clientInput(i);
			}
		}
	}
}
#include "Server.hpp"

Server::Server(): _name("ft_irc"), _port(0), _password("0000"), _maxOnline(5), _currentOnline(0), _pfd(NULL), _serverSocket(0), _serverAddr(), _listening(false) {}

Server::Server(int port, char *password, int maxOnline): _name("ft_irc"), _port(port), _password(password) {
	_maxOnline = maxOnline + 2;
	_listening = false;
	createSocket();
	_pfd = new pollfd[_maxOnline];
	_pfd[0].fd = _serverSocket;
	_pfd[0].events = POLLIN;
	_pfd[1].fd = 0;
	_pfd[1].events = POLLIN;
	_currentOnline = 2;
}

Server::~Server() {
	for (std::map<std::string, Channel *>::const_iterator it = _serverChannels.begin(); it != _serverChannels.end(); ++it) {
		delete(it->second);
	}
	for (std::map<int, Client *>::const_iterator it = _serverClients.begin(); it != _serverClients.end(); ++it) {
		delete(it->second);
	}
	for (int i = 0; i < _maxOnline; ++i) {
		close(_pfd[i].fd);
	}
	delete [] _pfd;
	close(_serverSocket);
}

bool	Server::getListening(void) const { return(_listening); }

void	Server::removeClient(int cfd) {
	int									i = 0;
	std::string							channelName = _serverClients[cfd]->getChannel();
	std::map<int, Client*>::iterator	it = _serverClients.find(cfd);

	if (cfd < 0) {
		std::cout << "Error: Can't remove client - " << cfd << "." << std::endl;
		return;
	}
	if (channelName != "\0" && _serverChannels.find(channelName) != _serverChannels.end()) {
		_serverChannels[channelName]->removeClientCh(cfd);
		if (_serverChannels[channelName]->getOnline() == 0) {
			delete(_serverChannels[channelName]);
			_serverChannels.erase(channelName);
		}
		_serverClients[cfd]->setChannel("\0");
	}
	if (_ClientsID.find(_serverClients[cfd]->getNickname()) != _ClientsID.end())
		_ClientsID.erase(_serverClients[cfd]->getNickname());
	if (it != _serverClients.end()) {
		delete it->second;
		_serverClients.erase(it);
	}
	while (i < _currentOnline) {
		if (_pfd[i].fd == cfd)
			break;
		++i;
	}
	close(_pfd[i].fd);
	_pfd[i] = _pfd[_currentOnline - 1];
	--_currentOnline;
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
		sendMessage(cfd, ERR_SERVICESFULL, "");
		close (cfd);
		return ;
	}
	_pfd[_currentOnline].fd = cfd;
	_pfd[_currentOnline].events = POLLIN;
	_currentOnline++;
	std::cout << "New connection on " << cfd << std::endl;
	_serverClients.insert(std::pair<int, Client *>(cfd, new Client(cfd)));
}

void	Server::startServer(void) {
	int	tmpPfd;

	while (1) {
		tmpPfd = poll(_pfd, _currentOnline, -1);
		if (tmpPfd < 0) {
			std::cout << "ERROR: Poll error.\n";
			return;
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
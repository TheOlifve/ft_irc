#include "Server.hpp"

/*
This file handles the IRC client authorization process, which consists of 3 steps:

1. Password verification (PASS command):
   - Checks if client provides correct password format and matches server password
   - Disconnects client if password is invalid
   - Sets pass flag if successful

2. Nickname registration (NICK command): 
   - Validates nickname format and availability
   - Disconnects client if nickname is invalid
   - Sets nickname if successful

3. User registration (USER command):
   - Validates user registration parameters
   - Disconnects if parameters are invalid
   - Sets user info and marks client as fully authorized if successful

The authorization flow is sequential - each step must be completed in order.
If any step fails, the client is disconnected.
*/

void	Server::authorization(const int &cfd, const std::vector<std::string> &tokens) {
	if (!_serverClients[cfd]->getPass()) {
		if (tokens.size() != 2) {
			sendMessage(cfd, ERR_NEEDMOREPARAMS, "PASS");
			std::cout << "Client [" << cfd << "] : disconnected (wrong pass params)" << std::endl;
			removeClient(cfd);
			return;
		} else if (tokens[0].compare("PASS") || tokens[1].compare(_password)) {
			sendMessage(cfd, ERR_PASSWDMISMATCH, "");
			removeClient(cfd);
			return;
		}
		_serverClients[cfd]->setPass(true);
		return;
	}
	if (!_serverClients[cfd]->getName()) {
		if (tokens[0].compare("NICK") || tokens.size() != 2) {
			sendMessage(cfd, ERR_NEEDMOREPARAMS, "NICK");
			removeClient(cfd);
			return;
		}
		cmdNick(cfd, tokens);
		if (!_serverClients[cfd]->getName()) {
			std::cout << "Client [" << cfd << "] : disconnected (wrong nick params)" << std::endl;
			removeClient(cfd);
			return;
		}
		return;
	}
	else {
		if (tokens[0].compare("USER") || tokens.size() < 2) {
			sendMessage(cfd, ERR_NEEDMOREPARAMS, "USER");
			std::cout << "Client [" << cfd << "] : disconnected (wrong user params)" << std::endl;
			removeClient(cfd);
			return;
		}
		cmdUser(cfd, tokens);
	}
	sendMessage(cfd, RPL_WELCOME, "");
	_serverClients[cfd]->setAuthorized(true);
}
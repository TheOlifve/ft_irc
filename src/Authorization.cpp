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
	// NICK and USER can come in any order before PASS
	if (!_serverClients[cfd]->getPass()) {
		if (tokens[0] == "PASS") {
			if (tokens.size() < 2) {
				sendMessage(cfd, ERR_NEEDMOREPARAMS, "PASS");
				return;
			}
			if (tokens[1] != _password) {
				sendMessage(cfd, ERR_PASSWDMISMATCH, "");
				removeClient(cfd);
				return;
			}
			_serverClients[cfd]->setPass(true);
		}
		else if (tokens[0] == "NICK") {
			handleNick(cfd, tokens);
		}
		else if (tokens[0] == "USER") {
			handleUser(cfd, tokens);
		}
		return;
	}

	// Once we have all required info, send welcome messages
	if (_serverClients[cfd]->getPass() && 
		_serverClients[cfd]->getName() && 
		_serverClients[cfd]->getUsername() != "\0") {
		
		sendMessage(cfd, RPL_WELCOME, "");
		sendMessage(cfd, RPL_YOURHOST, "");
		sendMessage(cfd, RPL_CREATED, "");
		sendMessage(cfd, RPL_MYINFO, "");
		// Send MOTD
		sendMessage(cfd, RPL_MOTDSTART, "");
		sendMessage(cfd, RPL_MOTD, "Welcome to ft_irc!");
		sendMessage(cfd, RPL_ENDOFMOTD, "");
		
		_serverClients[cfd]->setAuthorized(true);
	}
}

void Server::handleNick(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 2) {
		sendMessage(cfd, ERR_NONICKNAMEGIVEN, "");
		return;
	}
	
	std::string nickname = tokens[1];
	
	// Check if nickname is already in use
	if (_ClientsID.find(nickname) != _ClientsID.end()) {
		sendMessage(cfd, ERR_NICKNAMEINUSE, nickname);
		return;
	}
	
	// Set the nickname
	_serverClients[cfd]->setNickname(nickname);
	_serverClients[cfd]->setName(true);
	_ClientsID[nickname] = cfd;
}

void Server::handleUser(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 5) {  // USER requires 4 parameters
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "USER");
		return;
	}
	
	if (_serverClients[cfd]->getUsername() != "\0") {
		sendMessage(cfd, ERR_ALREADYREGISTERED, "");
		return;
	}
	
	// tokens[1] = username
	// tokens[2] = hostname
	// tokens[3] = servername
	// tokens[4] = realname (can contain spaces)
	_serverClients[cfd]->setUsername(tokens[1]);
}
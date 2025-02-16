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
	// NICK and USER can come in any order, but both are needed before sending welcome
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

	// Send welcome sequence once we have all required info
	if (_serverClients[cfd]->getPass() && 
		_serverClients[cfd]->getName() && 
		_serverClients[cfd]->getUsername() != "\0" &&
		!_serverClients[cfd]->getAuthorized()) {
		
		std::string nick = _serverClients[cfd]->getNickname();
		std::string user = _serverClients[cfd]->getUsername();
		
		// Send welcome messages
		sendMessage(cfd, RPL_WELCOME, "Welcome to the Internet Relay Network " + nick + "!" + user + "@ft_irc.local");
		sendMessage(cfd, RPL_YOURHOST, "Your host is ft_irc.local, running version 1.0");
		sendMessage(cfd, RPL_CREATED, "This server was created today");
		sendMessage(cfd, RPL_MYINFO, "ft_irc.local 1.0 itkol mtov");
		
		// Send MOTD
		sendMessage(cfd, RPL_MOTDSTART, "- ft_irc Message of the Day -");
		sendMessage(cfd, RPL_MOTD, "Welcome to ft_irc!");
		sendMessage(cfd, RPL_ENDOFMOTD, "End of /MOTD command");
		
		_serverClients[cfd]->setAuthorized(true);
		std::cout << "Client " << nick << "[" << cfd << "] : fully registered and authorized" << std::endl;
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
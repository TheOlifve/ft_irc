#include "Server.hpp"

void	Server::addNickname(const int &cfd, const std::string &nickname) {
	std::string	text;

	if (_ClientsID.find(nickname) != _ClientsID.end()) {
		sendMessage(cfd, ERR_NICKNAMEINUSE, nickname);
		return;
	}
	if (!(nickname.compare("\0")))
		_serverClients[cfd]->setNickname("Guest");
	else
		_serverClients[cfd]->setNickname(nickname);
	_serverClients[cfd]->setName(true);
	_ClientsID.insert(std::pair<std::string, int>(nickname, cfd));
}

void	Server::authorization(const int &cfd, const std::vector<std::string> &tokens) {
	if (!_serverClients[cfd]->getPass()) {
		if (tokens[0].compare("PASS")) {
			sendMessage(cfd, ERR_PASSWDMISMATCH, "");
			return;
		} else if (tokens.size() != 2) {
			sendMessage(cfd, ERR_NEEDMOREPARAMS, "NICK");
			return;
		}
		if (tokens[1].compare(_password)) {
			sendMessage(cfd, ERR_PASSWDMISMATCH, "");
			return;
		}
		_serverClients[cfd]->setPass(true);
		return;
	}
	if (!_serverClients[cfd]->getName()) {
		if (tokens[0].compare("NICK")) {
			sendMessage(cfd, ERR_NEEDMOREPARAMS, "NICK");
			return;
		} else if (tokens.size() != 2) {
			sendMessage(cfd, ERR_NEEDMOREPARAMS, "NICK");
			return;
		}
		addNickname(cfd, tokens[1]);
		return;
	}
	else {
		if (tokens[0].compare("USER")) {
			sendMessage(cfd, ERR_NEEDMOREPARAMS, "USER");
			return;
		} else if (tokens.size() < 2) {
			sendMessage(cfd, ERR_NEEDMOREPARAMS, "USER");
			return;
		}
		if (!(tokens[1].compare("\0")))
			_serverClients[cfd]->setNickname("Guest");
		else
			_serverClients[cfd]->setUsername(tokens[1]);
	}
	sendMessage(cfd, RPL_WELCOME, "");
	_serverClients[cfd]->setAuthorized(true);
}
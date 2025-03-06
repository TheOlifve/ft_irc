#include "Server.hpp"

void	Server::cmdHelp(const int &cfd, const std::vector<std::string> &tokens) {
	std::string	text("HELP, JOIN, PART, QUIT, USER, NICK, PRIVMSG");

	if (tokens.size() != 1) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "HELP");
		return;
	}
	if (_serverClients[cfd]->getOp()) {
		text.append(", KICK, INVITE, TOPIC");
	}
	sendMessage(cfd, RPL_HELPTXT, text);
	sendMessage(cfd, RPL_ENDOFHELP, text);
}

void	Server::cmdQuit(const int &cfd, const std::vector<std::string> &tokens) {
	std::string	text;
	if (tokens.size() != 1 && tokens.size() != 2) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "QUIT");
		return;
	}
	if (tokens.size() == 1)
		text = "QUIT";
	else if (tokens.size() == 2 && tokens[1].size() == 1 && tokens[1][0] == ':')
		text = "QUIT";
	else
		text = tokens[1];
	if (_serverClients[cfd]->getChannel() != "\0") {
		_serverChannels[_serverClients[cfd]->getChannel()]->channelMessage(*_serverClients[cfd], RPL_QUIT, text);
	}
	sendMessage(cfd, RPL_QUIT, text);
	removeClient(cfd);
	std::cout << "Client [" << cfd << "] : disconnected (QUIT)" << std::endl;
}

void	Server::cmdUser(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 2) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "USER");
		if (_serverClients[cfd]->getUsername() == "\0")
			_serverClients[cfd]->setUsername("user");
		return;
	}
	if (!nickValidation(tokens[1])) {
		sendMessage(cfd, ERR_ERRONEUSUSERNAME, tokens[1]);
		if (_serverClients[cfd]->getUsername() == "\0")
			_serverClients[cfd]->setUsername("user");
		return;
	}
	if (tokens[1] != "\0")
		_serverClients[cfd]->setUsername(tokens[1]);
	else
		_serverClients[cfd]->setUsername("user");
}


void	Server::cmdList(const int &cfd, const std::vector<std::string> &tokens) {
	std::string											text;
	std::map<std::string, Channel *>::const_iterator	it = _serverChannels.begin();

	if (tokens.size() != 1) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "LIST");
	}
	sendMessage(cfd, RPL_LISTSTART, "");
	for (; it != _serverChannels.end(); ++it) {
		text = it->second->getName();
		text.append(" ");
		text.append(itos(it->second->getOnline()));
		text.append(" :");
		text.append(it->second->getTopic());
		sendMessage(cfd, RPL_LIST, text);
	}
	sendMessage(cfd, RPL_LISTEND, "");
}

bool	nickValidation(const std::string &token) {
	std::string	all = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

	for (size_t i = 0; i < token.size(); ++i) {
		if (all.find(token[i]) == std::string::npos) {
			return false;
		}
	}
	return true;
}

void	Server::cmdNick(const int &cfd, const std::vector<std::string> &tokens) {
	std::string	oldNick = "\0";

	if (tokens.size() != 2) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "NICK");
		return;
	}
	if (_ClientsID.find(tokens[1]) != _ClientsID.end()) {
		sendMessage(cfd, ERR_NICKNAMEINUSE, tokens[1]);
		return;
	}
	if (!nickValidation(tokens[1])) {
		sendMessage(cfd, ERR_ERRONEUSNICKNAME, tokens[1]);
		return;
	}
	if (_serverClients[cfd]->getName() == true) {
		_ClientsID.erase(_serverClients[cfd]->getNickname());
		oldNick = _serverClients[cfd]->getNickname();
	}
	if (!(tokens[1].compare("\0")))
		_serverClients[cfd]->setNickname("Guest");
	else
		_serverClients[cfd]->setNickname(tokens[1]);
	if (_serverClients[cfd]->getChannel() != "\0")
		_serverChannels[_serverClients[cfd]->getChannel()]->channelMessage(*_serverClients[cfd], RPL_NICK, oldNick);
	_serverClients[cfd]->setName(true);
	_ClientsID.insert(std::pair<std::string, int>(tokens[1], cfd));
}

std::string	itos(int number) {
	std::stringstream tmp;

	tmp << number;
	return tmp.str();
}

void	Server::cmdPart(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens.size() != 2 && tokens.size() != 3) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "PART");
		return;
	}
	if (_serverChannels.find(tokens[1]) == _serverChannels.end()) {
		sendMessage(cfd, ERR_NOSUCHCHANNEL, tokens[1]);
		return;
	}
	if (_serverClients[cfd]->getChannel() == "\0" ||
			_serverClients[cfd]->getChannel().compare(tokens[1])) {
		sendMessage(cfd, ERR_NOTONCHANNEL, tokens[1]);
		return;
	}
	if (tokens.size() == 2) {
		sendMessage(cfd, RPL_PART, "Goodbye everyone!");
		_serverChannels[tokens[1]]->channelMessage(*_serverClients[cfd], RPL_PART, "Goodbye everyone!");
	}
	else {
		sendMessage(cfd, RPL_PART, tokens[2]);
		_serverChannels[tokens[1]]->channelMessage(*_serverClients[cfd], RPL_PART, tokens[2]);
	}
	_serverChannels[tokens[1]]->removeClientCh(cfd);
	if (_serverChannels[tokens[1]]->getOnline() == 0) {
		delete(_serverChannels[tokens[1]]);
		_serverChannels.erase(tokens[1]);
	}
	_serverClients[cfd]->setOp(false);
	_serverClients[cfd]->setChannel("\0");
}

const std::string	buildMessage(const int from, const std::vector<std::string> &tokens) {
	int			to = tokens.size();
	bool		j = false;
	std::string	text;

	if (tokens.size() > 2 && tokens[2].size() > 0 && tokens[2][0] == ':')
		j = true;
	for (int i = from; i < to; ++i) {
		text.append(tokens[i]);
		if (i != to - 1)
			text.append(" ");
		if (i == from && !j)
			text.append(":");
	}
	return text;
}

void	Server::cmdPrivmsg(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 3) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "PRIVMSG");
		return;
	}
	if (tokens.size() > 3 && tokens[2].size() > 0 && tokens[2][0] != ':') {
		sendMessage(cfd, ERR_TOOMANYTARGETS, buildMessage(1, tokens));
		return;
	}
	if (tokens.size() == 3 && tokens[2].size() == 1 && tokens[2][0] == ':') {
		sendMessage(cfd, ERR_NOTEXTTOSEND, "\0");
		return;
	}
	if (tokens[1].size() > 0 && tokens[1][0] == '#') {
		if (_serverChannels.find(tokens[1]) == _serverChannels.end()) {
			sendMessage(cfd, ERR_NOSUCHCHANNEL, tokens[1]);
			return;
		}
		if (_serverClients[cfd]->getChannel() == "\0" ||
			_serverClients[cfd]->getChannel().compare(tokens[1])) {
			sendMessage(cfd, ERR_NOTONCHANNEL, tokens[1]);
			return;
		}
		_serverChannels[tokens[1]]->channelMessage(*_serverClients[cfd], RPL_PRIVMSG, buildMessage(1, tokens));
		return;
	}
	if (tokens[1].size() > 0 && tokens[1][0] != '#') {
		if (_ClientsID.find(tokens[1]) == _ClientsID.end()) {
			sendMessage(cfd, ERR_NOSUCHNICK, tokens[1]);
			return;
		}
		sendMessage(cfd, RPL_PRIVMSG, buildMessage(1, tokens));
		return;
	}
	else
		sendMessage(cfd, ERR_NORECIPIENT, "\0");
}

void	Server::cmdParsing(const int &cfd, const std::vector<std::string> &tokens) {
	if (!tokens[0].compare("HELP"))
		cmdHelp(cfd, tokens);
	else if (!tokens[0].compare("JOIN"))
		cmdJoin(cfd, tokens);
	else if (!tokens[0].compare("PART"))
		cmdPart(cfd, tokens);
	else if (!tokens[0].compare("QUIT"))
		cmdQuit(cfd, tokens);
	else if (!tokens[0].compare("LIST"))
		cmdList(cfd, tokens);
	else if (!tokens[0].compare("USER"))
		cmdUser(cfd, tokens);
	else if (!tokens[0].compare("NICK"))
		cmdNick(cfd, tokens);
	else if (!tokens[0].compare("PRIVMSG"))
		cmdPrivmsg(cfd, tokens);
	else if (!tokens[0].compare("MODE"))
		cmdMode(cfd, tokens);
	else if (!tokens[0].compare("TOPIC"))
		cmdTopic(cfd, tokens);
	else if (!tokens[0].compare("KICK"))
		cmdKick(cfd, tokens);
	else if (!tokens[0].compare("INVITE"))
		cmdInvite(cfd, tokens);
	else if (!tokens[0].compare("PONG"))
		;
	else
		sendMessage(cfd, ERR_UNKNOWNCOMMAND, tokens[0]);
}

void	replaceWhiteSpaces(std::string &str) {
	size_t pos = 0;

	while ((pos = str.find_first_of("\t\n\r\v\f", pos)) != std::string::npos) {
		str[pos] = ' ';
		pos++;
	}
}

void	Server::clientInput(int i) {
	int							bytesRead;
	int							cfd = _pfd[i].fd;
	char						buff[1024];
	std::string					text;
	std::vector<std::string>	tokens;

	bytesRead = read(cfd, buff, sizeof(buff) - 1);
	if (bytesRead > 1024) {
		sendMessage(cfd, ERR_MSGTOOLONG, "");
		return;
	} else if (bytesRead > 0) {
		buff[bytesRead - 1] = '\0';
		std::cout << "From client [" << cfd << "] - " <<  buff << std::endl;
		tokens = split(buff);
		if (tokens.size() <= 0) {
			return;
		}
		if (!_serverClients[cfd]->getAuthorized())
			authorization(cfd, tokens);
		else
			cmdParsing(cfd, tokens);
	} else if (bytesRead == 0) {
		if (_serverClients[cfd]->getAuthorized()) {
			sendMessage(cfd, RPL_QUIT, "QUIT");
			if (_serverClients[cfd]->getChannel() != "\0")
				_serverChannels[_serverClients[cfd]->getChannel()]->channelMessage(*_serverClients[cfd], RPL_QUIT, "QUIT");
		}
		removeClient(cfd);
		std::cout << "Client [" << cfd << "] : disconnected (EOF on input)" << std::endl;
		return;
	}
	memset(buff, 0, 1024);
}

int	Server::createChannel(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens[1][0] != '#' || tokens[1].size() < 2) {
		sendMessage(cfd, ERR_NOSUCHCHANNEL, tokens[1]);
		return (1);
	}
	if (tokens.size() == 2) {
		_serverChannels.insert(std::pair<std::string, Channel *>(tokens[1], new Channel(tokens[1], "\0")));
		_serverChannels[tokens[1]]->setK(false);
	}
	else {
		_serverChannels.insert(std::pair<std::string, Channel *>(tokens[1], new Channel(tokens[1], tokens[2])));
		_serverChannels[tokens[1]]->setK(true);
	}
	std::cout << "Channel " << tokens[1] << " successfully created." << std::endl;
	std::cout << "CHANNELS: " << _serverChannels.size() << std::endl;
	return (0);
}

std::vector<std::string> split(std::string str) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end;

	replaceWhiteSpaces(str);
	while (start < str.length()) {
		end = str.find(' ', start);
		if (end == std::string::npos) {
			if (end > start)
				tokens.push_back(str.substr(start));
			break;
		}
		if (end > start) {
			tokens.push_back(str.substr(start, end - start));
		}
		start = end + 1;
	}
	return tokens;
}

void	Server::cmdJoin(const int &cfd, const std::vector<std::string> &tokens) {
	if (!(tokens.size() == 3 || tokens.size() == 2)) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "JOIN");
		return;
	}
	if (_serverChannels.find(tokens[1]) == _serverChannels.end())
		if (createChannel(cfd, tokens))
			return;
	if (_serverClients[cfd]->getChannel() == tokens[1]) {
		sendMessage(cfd, ERR_USERONCHANNEL, tokens[1]);
		return;
	}
	if (_serverChannels[tokens[1]]->getI() == true && !_serverChannels[tokens[1]]->invited(cfd)) {
		sendMessage(cfd, ERR_INVITEONLYCHAN, "JOIN");
		return;
	}
	if (_serverChannels[tokens[1]]->getK() == true && tokens.size() != 3) {
		sendMessage(cfd, ERR_BADCHANNELKEY, tokens[1]);
		return;
	}
	if (_serverChannels[tokens[1]]->getK() == false && tokens.size() != 2) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "JOIN");
		return;
	}
	if (_serverChannels[tokens[1]]->getK() == true &&
			_serverChannels[tokens[1]]->getKey().compare(tokens[2])) {
		sendMessage(cfd, ERR_BADCHANNELKEY, tokens[1]);
		return;
	}
	if (_serverClients[cfd]->getChannel() != "\0") {
		_serverClients[cfd]->setOp(false);
		_serverChannels[_serverClients[cfd]->getChannel()]->removeClientCh(cfd);
	}
	if (!_serverChannels[tokens[1]]->joinChannel(*_serverClients[cfd], tokens)) {
		sendMessage(cfd, ERR_CHANNELISFULL, tokens[1]);
		return;
	}
	_serverClients[cfd]->setChannel(tokens[1]);
	sendMessage(cfd, RPL_JOINCHANNEL, tokens[1]);
	if (_serverChannels[tokens[1]]->getT() == true)
		sendMessage(cfd, RPL_TOPIC, _serverChannels[tokens[1]]->getTopic());
	else
		sendMessage(cfd, RPL_NOTOPIC, "\0");
	sendMessage(cfd, RPL_NAMREPLY, _serverChannels[tokens[1]]->usersList());
	sendMessage(cfd, RPL_ENDOFNAMES, "\0");
}

void	Server::parseMode(const int &cfd, const std::vector<std::string> &tokens, bool condition) {

	if (tokens[2][1] == 'i') {
		_serverChannels[tokens[1]]->setI(condition);
		if (condition)
			sendMessage(cfd, RPL_INVITEONLY, "");
		else
			sendMessage(cfd, RPL_INVITEFREE, "");
	}
	else if (tokens[2][1] == 't') {
		_serverChannels[tokens[1]]->setT(condition);
		if (condition)
			sendMessage(cfd, RPL_TOPICOPONLY, "");
		else
			sendMessage(cfd, RPL_TOPICOPANYONE, "");
	}
	else if (tokens[2][1] == 'k') {
		if (tokens.size() != 4 && condition)
		{
			sendMessage(cfd, ERR_KEYPARAMS, tokens[1].substr(1));
			return;
		}
		_serverChannels[tokens[1]]->setK(condition);
		if (condition && tokens.size() == 4)
			_serverChannels[tokens[1]]->setKey(tokens[3]);
		else
			_serverChannels[tokens[1]]->setKey("");
		if (condition)
			sendMessage(cfd, RPL_CHANNELPASS, "");
		else
			sendMessage(cfd, RPL_CHANNELPASSLESS, "");
	}
	else if (tokens[2][1] == 'o') {

		if (!_ClientsID[tokens[3]]) {
			sendMessage(cfd, ERR_NOSUCHNICK, tokens[3]);
			return ;
		}
		else if(_serverChannels[tokens[1]]->getUsers()[_ClientsID[tokens[3]]] == NULL &&
		_serverChannels[tokens[1]]->getOps()[_ClientsID[tokens[3]]] == NULL) {
			sendMessage(cfd, ERR_USERNOTINCHANNEL, tokens[3]);
			return ;
		}
		if (tokens.size() == 4) {
			if (condition) {
				_serverChannels[tokens[1]]->setOp(_ClientsID[tokens[3]], _serverClients[_ClientsID[tokens[3]]]);
				sendMessage(cfd, RPL_ASSIGNOP, tokens[3]);
				sendMessage(_ClientsID[tokens[3]], RPL_CHANNELMODEOP, tokens[1].substr(1));
			}
			else {
				if (_serverChannels[tokens[1]]->getOps()[_ClientsID[tokens[3]]] == false) {
					sendMessage(cfd, ERR_NOTOPERATOR, tokens[3]);
					return ;
				}
				_serverChannels[tokens[1]]->removeOp(_ClientsID[tokens[3]]);
				sendMessage(cfd, RPL_REMOVEOPERATOR, tokens[3]);
				sendMessage(_ClientsID[tokens[3]], RPL_REMOVEDOP, tokens[1].substr(1));
			}
		}
		else
			sendMessage(cfd, ERR_OPPARAMS, tokens[1].substr(1));
	}
	else if (tokens[2][1] == 'l') {
		if (tokens.size() == 4 && condition && atoi(tokens[3].c_str()) > 0) {
			_serverChannels[tokens[1]]->setLimit(atoi(tokens[3].c_str()));
			_serverChannels[tokens[1]]->setL(condition);
		}
		else if (tokens.size() == 3 && !condition) {
			_serverChannels[tokens[1]]->setLimit(0);
			_serverChannels[tokens[1]]->setL(false);
		}
		else {
			sendMessage(cfd, ERR_USERLIMITPARAMS, tokens[1].substr(1));
			return;
		}
		if (condition)
			sendMessage(cfd, RPL_USERLIMITSET, tokens[1].substr(1));
		else
			sendMessage(cfd, RPL_USERLIMITREMOVED, tokens[1].substr(1));
	}
	else
		sendMessage(cfd, ERR_WRONGMODE, tokens[2]);
}

void	Server::cmdMode(const int &cfd, const std::vector<std::string> &tokens) {
	bool	condition;

	if (tokens.size() == 2 && _serverChannels.find(tokens[1]) != _serverChannels.end()) {
			std::string modeStr = "+";

			if (_serverChannels[tokens[1]]->getI())
				modeStr.append("i");
			if (_serverChannels[tokens[1]]->getT())
				modeStr.append("t");
			if (_serverChannels[tokens[1]]->getK())
				modeStr.append("k");
			if (_serverChannels[tokens[1]]->getL())
				modeStr.append("l");
			sendMessage(cfd, RPL_CHANNELMODEIS, modeStr);
		return;
	}
	if (tokens.size() > 1 && _serverChannels.find(tokens[1]) != _serverChannels.end()
		&& _serverChannels[tokens[1]]->getOps()[cfd] == NULL) {
		sendMessage(cfd, ERR_NOPERMISSION, tokens[1].substr(1));
		return ;
	}
	if ((tokens.size() != 3 && tokens.size() != 4) || tokens[1][0] != '#'
		|| tokens[2].length() != 2 || (tokens[2][0] != '+' && tokens[2][0] != '-')) {
		sendMessage(cfd, ERR_MODEPARAMS, "");
		return ;
	}
	else {
		if (_serverChannels.find(tokens[1]) == _serverChannels.end() || _serverChannels[tokens[1]] == NULL) {
			sendMessage(cfd, ERR_NOSUCHCHANNEL, "MODE");
			return ;
		}
		if (tokens[2][0] == '+')
			condition = true;
		else
			condition = false;
		parseMode(cfd, tokens, condition);
	}

}

void	Server::cmdTopic(const int &cfd, const std::vector<std::string> &tokens) {
	if ((tokens.size() != 3 && tokens.size() != 2) || tokens[1][0] != '#') {
		sendMessage(cfd, ERR_TOPICPARAMS, tokens[1]);
		return;
	}
	if (_serverClients[cfd]->getChannel() == "\0" ||
		_serverClients[cfd]->getChannel().compare(tokens[1])) {
		sendMessage(cfd, ERR_NOTONCHANNEL, tokens[1]);
		return;
	}
	if (_serverChannels.find(tokens[1]) == _serverChannels.end()) {
		sendMessage(cfd, ERR_NOSUCHCHANNEL, tokens[1]);
		return;
	}
	if (tokens.size() == 3 && _serverChannels[tokens[1]]->getT() == false) {
		_serverChannels[tokens[1]]->setTopic(tokens[2]);
		_serverChannels[tokens[1]]->setT(true);
		sendMessage(cfd, RPL_TOPICSET, tokens[2]);
		_serverChannels[tokens[1]]->channelMessage(*_serverClients[cfd], RPL_USERTOPICSET, tokens[1]);
	}
	else if (tokens.size() == 3 && _serverChannels[tokens[1]]->getT() == true
		&& _serverChannels[tokens[1]]->getOps()[cfd] != NULL) {
		_serverChannels[tokens[1]]->setTopic(tokens[2]);
		_serverChannels[tokens[1]]->setT(true);
		sendMessage(cfd, RPL_TOPICSET, tokens[2]);
		_serverChannels[tokens[1]]->channelMessage(*_serverClients[cfd], RPL_USERTOPICSET, tokens[1]);
	}
	if (tokens.size() == 2)
	{
		sendMessage(cfd, RPL_TOPIC, _serverChannels[tokens[1]]->getTopic());
		return;
	}
	else
		sendMessage(cfd, ERR_NOTOPERATOR, tokens[1]);
}

void	Server::cmdKick(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens.size() < 3) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "KICK");
		return;
	}

	if (_serverChannels.find(tokens[1]) == _serverChannels.end()) {
		sendMessage(cfd, ERR_NOSUCHCHANNEL, tokens[1]);
		return;
	}

	if (_serverClients[cfd]->getChannel() != tokens[1]) {
		sendMessage(cfd, ERR_NOTONCHANNEL, tokens[1]);
		return;
	}

	if (!_serverChannels[tokens[1]]->getOps()[cfd]) {
		sendMessage(cfd, ERR_CHANOPRIVSNEEDED, tokens[1]);
		return;
	}

	std::map<std::string, int>::iterator it = _ClientsID.find(tokens[2]);
	if (it == _ClientsID.end()) {
		sendMessage(cfd, ERR_NOSUCHNICK, tokens[2]);
		return;
	}
	int targetFd = it->second;

	if (_serverClients[targetFd]->getChannel() != tokens[1]) {
		sendMessage(cfd, ERR_USERNOTINCHANNEL, tokens[2]);
		return;
	}

	std::string kickMsg;

	if (tokens.size() > 3)
		kickMsg = buildMessage(2, tokens);
	else
		kickMsg =_serverClients[targetFd]->getNickname();

	_serverChannels[tokens[1]]->channelMessage(*_serverClients[cfd], RPL_KICK, kickMsg);
	_serverChannels[tokens[1]]->removeClientCh(targetFd);
	_serverClients[targetFd]->setChannel("\0");
	_serverClients[targetFd]->setOp(false);
	_serverChannels[tokens[1]]->removeInvitelist(targetFd);
}

std::string buildInviteMessage(const std::string &inviter, const std::vector<std::string> &tokens) {
	std::string msg = ":";

	msg.append(inviter);
	msg.append(" INVITE ");
	msg.append(tokens[1]);
	msg.append(" ");
	msg.append(tokens[2]);
	msg.append("\r\n");
	return msg;
}

void	Server::cmdInvite(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens.size() != 3) {
		sendMessage(cfd, ERR_NEEDMOREPARAMS, "INVITE");
		return;
	}

	std::map<std::string, int>::iterator it = _ClientsID.find(tokens[1]);
	if (it == _ClientsID.end()) {
		sendMessage(cfd, ERR_NOSUCHNICK, tokens[1]);
		return;
	}
	int targetFd = it->second;

	if (_serverChannels.find(tokens[2]) == _serverChannels.end()) {
		sendMessage(cfd, ERR_NOSUCHCHANNEL, tokens[2]);
		return;
	}

	if (_serverClients[cfd]->getChannel() != tokens[2]) {
		sendMessage(cfd, ERR_NOTONCHANNEL, tokens[2]);
		return;
	}

	if (_serverClients[targetFd]->getChannel() == tokens[2]) {
		sendMessage(cfd, ERR_USERONCHANNEL, tokens[1]);
		return;
	}

	if (_serverChannels[tokens[2]]->getI() && !_serverChannels[tokens[2]]->getOps()[cfd]) {
		sendMessage(cfd, ERR_CHANOPRIVSNEEDED, tokens[2]);
		return;
	}

	sendMessage(targetFd, RPL_INVITE, buildInviteMessage(_serverClients[cfd]->getNickname(), tokens));
	sendMessage(cfd, RPL_INVITING, tokens[1]);

	_serverChannels[tokens[2]]->addInvitelist(targetFd);
}

void	Server::serverInput(void) {
	int			bytesRead;
	char		buff[1024];
	std::string	message;

	bytesRead = read(0, buff, sizeof(buff) - 1);
	if (bytesRead == 0)
		throw std::runtime_error("EXIT: EOF on input!");
}

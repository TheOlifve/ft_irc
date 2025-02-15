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

void	Server::message(const int &cfd, const std::vector<std::string> &tokens) {
	std::string	text("Wrong input.\n");

	if (tokens[0].length() <= 0) {
		send(cfd, text.c_str(), text.length(), 0);
		return;
	} else if (tokens[0][0] == '/') {
		text = "Unknown command.\n";
		send(cfd, text.c_str(), text.length(), 0);
		return;
	} else if (_serverClients[cfd]->getChannel() == "\0") {
		text = "You have not joined a channel. Use /JOIN <#channel> [<password>] to join a channel.\n";
		send(cfd, text.c_str(), text.length(), 0);
		return;
	} else {
		text = "\0";
		for (size_t i = 0; i < tokens.size(); ++i) {
			text.append(tokens[i]);
			text.append(" ");
		}
		text.erase(text.find(" ", text.length() - 1));
		text.append("\n");
		_serverChannels[_serverClients[cfd]->getChannel()]->channelMessage(*_serverClients[cfd], 0, "");
	}
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

void	Server::cmdPing(const int &cfd) {
	sendMessage(cfd, RPL_PING, "\0");
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
	// else if (!tokens[0].compare("MODE"))
	// 	cmdMode(cfd, tokens);
	else if (!tokens[0].compare("STATUS"))
		cmdStatus(cfd, tokens);
	else if (!tokens[0].compare("LIST"))
		cmdList(cfd, tokens);
	else if (!tokens[0].compare("USER"))
		cmdUser(cfd, tokens);
	else if (!tokens[0].compare("NICK"))
		cmdNick(cfd, tokens);
	else if (!tokens[0].compare("PONG"))
		cmdPing(cfd);
	else
		message(cfd, tokens);
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
		std::cout << "Wrong channel name: Usage /CREATE <#channel> [<password>]." << std::endl;
		sendMessage(cfd, ERR_NOSUCHCHANNEL, tokens[1]);
		return (1);
	}
	if (tokens.size() == 2) {
		_serverChannels.insert(std::pair<std::string, Channel *>(tokens[1], new Channel(tokens[1], "")));
		_serverChannels[tokens[1]]->setK(false);
	}
	else {
		_serverChannels.insert(std::pair<std::string, Channel *>(tokens[1], new Channel(tokens[1], tokens[2])));
		_serverChannels[tokens[1]]->setK(false);
	}
	std::cout << "Channel " << tokens[1] << " successfully created." << std::endl;
	return (0);
}

void	Server::assignOperator(const std::vector<std::string> &tokens) {
	if (tokens.size() != 3 || tokens[1][0] != '#') {
		std::cout << "Wrong number of parameters: Usage /OPERATOR '#CHANNEL NAME' 'USER NAME'." << std::endl;
		return;
	}
	if (_serverChannels.find(tokens[1].substr(1)) == _serverChannels.end()) {
		std::cout << "ERROR: A server doesn't exist." << std::endl;
		return;
	}
	else if (_ClientsID.find(tokens[2]) == _ClientsID.end())
	{
		std::cout << "ERROR: A user doesn't exist." << std::endl;
		return;
	}
	if (_serverChannels[tokens[1].substr(1)]->getOps()[_ClientsID[tokens[2]]]) {
		std::cout << "ERROR: User is already an operator in this channel." << std::endl;
		return;
	}
	if (_serverChannels[tokens[1].substr(1)]->getUsers()[_ClientsID[tokens[2]]] == NULL) {
		std::cout << "ERROR: User is not in this channel." << std::endl;
		return;
	}
	_serverChannels[tokens[1].substr(1)]->setOp(_ClientsID[tokens[2]], _serverClients[_ClientsID[tokens[2]]]);
	std::cout << "User " << tokens[2] << " successfully assigned as operator in channel " << tokens[1].substr(1) << "." << std::endl;
	send(_ClientsID[tokens[2]], "You have been assigned as operator in the channel ", 50, 0);
	send(_ClientsID[tokens[2]], tokens[1].substr(1).c_str(), tokens[1].substr(1).size(), 0);
	send(_ClientsID[tokens[2]], ".\n", 2, 0);
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
	_serverClients[cfd]->setChannel(tokens[1]);
	_serverChannels[tokens[1]]->joinChannel(*_serverClients[cfd], tokens);
	sendMessage(cfd, RPL_JOINCHANNEL, tokens[1]);
	if (_serverChannels[tokens[1]]->getT() == true)
		sendMessage(cfd, RPL_TOPIC, _serverChannels[tokens[1]]->getTopic());
	else
		sendMessage(cfd, RPL_NOTOPIC, "");
	sendMessage(cfd, RPL_NAMREPLY, _serverChannels[tokens[1]]->usersList());
	sendMessage(cfd, RPL_ENDOFNAMES, "");
}

void	Server::parseMode(const int &cfd, const std::vector<std::string> &tokens, bool condition) {

	if (tokens[2][1] == 'i') {
		_serverChannels[tokens[1].substr(1)]->setI(condition);
		if (condition)
			send(cfd, "You have set the channel to invite only.\n", 41, 0);
		else
			send(cfd, "You set the channel to be invite-free.\n", 39, 0);
	}
	else if (tokens[2][1] == 't') {
		_serverChannels[tokens[1].substr(1)]->setT(condition);
		if (condition)
			send(cfd, "You have set the channel's topic to be set by operators only.\n", 62, 0);
		else
			send (cfd, "You have set the channel's topic to be set by anyone.\n", 54, 0);
	}
	else if (tokens[2][1] == 'k') {
		if (tokens.size() != 4)
		{
			send(cfd, "ERROR: Wrong number of parameters. Set a password with /MODE '#channel' '+/-k' 'password'.\n", 91, 0);
			return;
		}
		_serverChannels[tokens[1].substr(1)]->setK(condition);
		if (condition && tokens.size() == 4)
			_serverChannels[tokens[1].substr(1)]->setKey(tokens[3]);
		else
			_serverChannels[tokens[1].substr(1)]->setKey("\0");
		if (condition)
			send(cfd, "You have set the channel to be password protected.\n", 51, 0);
		else
			send(cfd, "You made the channel passwordless.\n", 35, 0);
	}
	else if (tokens[2][1] == 'o') { //seg
		_serverChannels[tokens[1].substr(1)]->setO(condition);
		if (tokens.size() == 4) {
			for (size_t i = 0; i < _serverChannels[tokens[1].substr(1)]->getUsers().size(); ++i) {
				if (_serverChannels[tokens[1].substr(1)]->getUsers()[i]->getNickname() == tokens[3]) { //seg
					_serverClients[_serverChannels[tokens[1].substr(1)]->getUsers()[i]->getUserFd()]->setOp(condition);
					break;
				}
			}
			if (condition)
				send(cfd, "You have set the user to be an operator.\n", 41, 0);
			else
				send(cfd, "You have removed the user from the operator list.\n", 50, 0);
		}
		else
			send(cfd, "ERROR: Wrong number of parameters. Usage /MODE '#channel' '+/-o' 'username'.\n", 77, 0);
	}
	else if (tokens[2][1] == 'l') {
		_serverChannels[tokens[1].substr(1)]->setL(condition);
		if (tokens.size() == 4 && condition && atoi(tokens[3].c_str()) > 0)
			_serverChannels[tokens[1].substr(1)]->setLimit(atoi(tokens[3].c_str()));
		else if (tokens.size() != 4)
			send(cfd, "ERROR: Wrong number of parameters. Usage /MODE '#channel' '+/-l' 'limit'.\n", 77, 0);
		else
			_serverChannels[tokens[1].substr(1)]->setLimit(0);
		if (condition)
			send(cfd, "You have set the channel to have a user limit.\n", 47, 0);
		else
			send(cfd, "You have removed the user limit from the channel.\n", 50, 0);
	}
	else
		send(cfd, "ERROR: Wrong mode.\n", 19, 0);
}

void	Server::cmdMode(const int &cfd, const std::vector<std::string> &tokens) {
	bool	condition;

	if (_serverChannels[tokens[1].substr(1)]->getOps()[cfd] == NULL) {
		send(cfd, "ERROR: You don't have permission to change the mode.\n", 53, 0);
		return ;
	}
	else if (_serverChannels[tokens[1].substr(1)]->getUsers()[cfd] == NULL) {
		send(cfd, "ERROR: You are not in this channel.\n", 36, 0);
		return ;
	}
	if ((tokens.size() != 3 && tokens.size() != 4) || tokens[1][0] != '#'
		|| tokens[2].length() != 2 || (tokens[2][0] != '+' && tokens[2][0] != '-')) {
		send(cfd, "Wrong number of parameters: Usage /MODE '#channel' '+/-mode' 'parameter'.\n", 74, 0);
		return ;
	}
	else {
		if (_serverChannels.find(tokens[1].substr(1)) == _serverChannels.end()) {
			send(cfd, "ERROR: A server with this name doesn't exist.\n", 46, 0);
			return ;
		}
		if (tokens[2][0] == '+')
			condition = true;
		else
			condition = false;
		parseMode(cfd, tokens, condition);
	}

}
//change to send
void	Server::cmdStatus(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens.size() != 2 || tokens[1][0] != '#') {
		send(cfd, "Wrong number of parameters: Usage /STATUS '#CHANNEL NAME'.\n", 59, 0);
		return;
	}
	if (_serverChannels.find(tokens[1].substr(1)) == _serverChannels.end()) {
		std::cout << "ERROR: A server doesn't exist." << std::endl;
		return;
	}
	if (_serverChannels[tokens[1].substr(1)]->getOps()[cfd] == false) {
		send(cfd, "ERROR: You don't have permission to view the status.\n", 53, 0);
		return ;
	}
	std::cout << "Current status of the channel " << tokens[1].substr(1) << ":\n";
	std::cout << "Invite only: " << _serverChannels[tokens[1].substr(1)]->getI() << std::endl;
	std::cout << "Topic set by operator: " << _serverChannels[tokens[1].substr(1)]->getT() << std::endl;
	std::cout << "Channel topic: " << _serverChannels[tokens[1].substr(1)]->getTopic() << std::endl;
	std::cout << "Password protected: " << _serverChannels[tokens[1].substr(1)]->getK() << std::endl;
	// std::cout << "Operators: ";
	// if (_serverChannels.size() && _serverChannels[tokens[1].substr(1)]) {
	// 	std::map<const int, const Client *>::iterator it;
	// 	for (it = _serverChannels[tokens[1].substr(1)]->getOps().begin(); it != _serverChannels[tokens[1].substr(1)]->getOps().end(); ++it)
	// 		std::cout << it->second->getName() << " ";
	// }
	std::cout << std::endl;
	std::cout << "User limit: " << _serverChannels[tokens[1].substr(1)]->getL() << std::endl;
	// std::cout << "Current users: ";
	// if (_serverChannels.size() && _serverChannels[tokens[1].substr(1)]) {
	// 	std::map<const int, const Client *>::iterator it;
	// 	for (it = _serverChannels[tokens[1].substr(1)]->getUsers().begin(); it != _serverChannels[tokens[1].substr(1)]->getUsers().end(); ++it)
	// 		std::cout << it->second->getName() << " ";
	// }
	// std::cout << std::endl;
}

void	Server::serverInput(void) {
	int			bytesRead;
	char		buff[1024];
	std::string	message;

	bytesRead = read(0, buff, sizeof(buff) - 1);
	if (bytesRead == 0)
		throw std::runtime_error("EXIT: EOF on input!");
}

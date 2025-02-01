#include "Server.hpp"

void	Server::cmdHelp(int cfd, std::vector<std::string> &tokens) {
	std::string	text("/CHANNELS - See available channels.\n/JOIN 'CHANNEL NAME' 'KEY ()' - Join to a channel.\n/DC - Disconnect from current channel.\n");

	if (tokens.size() != 1) {
		text = "Warning : Wrong number of parameters /HELP.\n";
		send(cfd, text.c_str(), text.length(), 0);
		return;
	}
	text.append("/UN 'New username' - Change username.\n/NN 'New nickname' - Change nickname.\n/PM 'Username' - Send private message.\n/EXIT - Leave server.\nType without quotes.\n");
	if (_serverClients[cfd]->getOp()) {
		text.append("KICK - Eject a client from the channel.\nINVITE - Invite a client to a channel.\n/TOPIC - Change or view the channel topic.\n/MODE - Change the channel’s mode.\n");
	}
	send(cfd, text.c_str(), text.length(), 0);
	text.clear();
}

void	Server::cmdQuit(int cfd, std::vector<std::string> &tokens) {
	std::string	text;

	if (tokens.size() != 1) {
		text = "Warning : Wrong number of parameters /Q.\n";
		send(cfd, text.c_str(), text.length(), 0);
		return;
	}
	if (_serverClients[cfd]->getChannel() == "\0") {
		text = "Warning : You are not in any channel.\n";
		send(cfd, text.c_str(), text.length(), 0);
		return;
	}
	_serverChannels[_serverClients[cfd]->getChannel()]->removeClient(cfd);
	text = "You left the channel ";
	text.append(_serverClients[cfd]->getChannel());
	text.append(".\n");
	_serverClients[cfd]->setChannel("\0");
	send(cfd, text.c_str(), text.length(), 0);
}

std::string itos(int number) {
	std::stringstream tmp;

	tmp << number;
	return tmp.str();
}

void	Server::cmdChannels(int cfd, std::vector<std::string> &tokens) {
	std::string											text;
	std::map<std::string, Channel *>::const_iterator	channels;

	if (tokens.size() != 1) {
		text = "Warning : Wrong number of parameters /CHANNELS.\n";
		send(cfd, text.c_str(), text.length(), 0);
		return;
	}
	text = "Channels\n_________________________\n";
	send(cfd, text.c_str(), text.length(), 0);
	channels = _serverChannels.begin();
	for (; channels != _serverChannels.end(); ++channels) {
		text = channels->first;
		if (channels->second->getK() == true)
			text.append(" | (K)");
		if (channels->second->getI())
			text.append(" | (I)");
		text.append(" | (");
		text.append(itos(channels->second->getOnline()));
		if (channels->second->getL()) {
			text.append("/");
			text.append(itos(channels->second->getLimit()));
		}
		text.append(")\n");
		send(cfd, text.c_str(), text.length(), 0);
		text.erase();
	}
	text = "_________________________\n";
	send(cfd, text.c_str(), text.length(), 0);
}

void	Server::cmdParsing(int cfd, std::vector<std::string> &tokens) {
	if (!tokens[0].compare("/HELP"))
		cmdHelp(cfd, tokens);
	else if (!tokens[0].compare("/JOIN"))
		cmdJoin(cfd, tokens);
	else if (!tokens[0].compare("/MODE"))
		cmdMode(cfd, tokens);
	else if (!tokens[0].compare("/STATUS"))
		cmdStatus(cfd, tokens);
	else if (!tokens[0].compare("/Q"))
		cmdQuit(cfd, tokens);
	else if (!tokens[0].compare("/CHANNELS"))
		cmdChannels(cfd, tokens);
	else
		send(cfd, "Unknown command.\n", 17, 0);
}

void	Server::clientInput(int i) {
	int							bytesRead;
	int							cfd = _pfd[i].fd;
	char						buff[1024];
	std::string					text;
	std::vector<std::string>	tokens;

	bytesRead = read(cfd, buff, sizeof(buff) - 1);
	if (bytesRead > 1024) {
		text = "ERROR: Out of buffer(1024).\n";
		send(cfd, text.c_str(), text.length(), 0);
		return;
	} else if (bytesRead > 0) {
		buff[bytesRead - 2] = '\0';
		tokens = split(buff);
		text = "ERROR: Too short.\n";
		if (tokens.size() <= 0)
			send(cfd, text.c_str(), text.length(), 0);
		if (!_serverClients[cfd]->getAuthorized())
			authorization(cfd, i, tokens[0]);
		else if (!_serverClients[cfd]->getName())
			addUsername(cfd, tokens[0]);
		else
			cmdParsing(cfd, tokens);
	} else if (bytesRead == 0) {
		text = "EXIT: EOF on input.\n";
		send(cfd, text.c_str(), text.length(), 0);
		std::cout << "Client [" << cfd << "] : disconnected (EOF on input)" << std::endl;
		removeClient(i);
		return;
	}
	memset(buff, 0, 1024);
}

void	Server::createChannel(const std::vector<std::string> &tokens) {
	if (tokens.size() != 3 && tokens.size() != 2) {
		std::cout << "Wrong number of parameters: Usage /CREATE 'NAME' 'KEY' (optional)." << std::endl;
		return;
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

std::vector<std::string> split(const std::string& str) {
	int							pos = 1;
	std::string					tStr(str);
	std::vector<std::string>	tokens;

	while (pos > 0) {
		pos = tStr.find(' ');
		tokens.push_back(tStr.substr(0, pos));
		tStr.erase(0, pos + 1);
	}
	return tokens;
}

void	Server::listAll(const std::vector<std::string> &tokens) {
	if (tokens.size() != 1) {
		std::cout << "Warning : Wrong number of parameters /LIST." << std::endl;
		return;
	}
	std::map<int, Client *>::const_iterator				itClients = _serverClients.begin();
	std::map<std::string, Channel *>::const_iterator	itChannels = _serverChannels.begin();

	std::cout << "Clients Online\n________________________\n";
	while (itClients != _serverClients.end()) {
		std::cout << itClients->second->getUsername() << " (" << itClients->second->getUserFd() << ")." << std::endl;
		++itClients;
	}
	std::cout << "________________________\n";
	std::cout << "Channels\n________________________\n";
	while (itChannels != _serverChannels.end()) {
		std::cout << itChannels->first << std::endl;
		++itChannels;
	}
	std::cout << "________________________\n";
}

void	cmdUseage(void) {
	std::cout << "Unknown command." << std::endl << \
	"Available commands - /CREATE 'CHANNEL NAME' 'CHANNEL PASSWORD', /LIST" << std::endl;
}

void	Server::serverCmdParsing(const std::string &message) {
	std::vector<std::string>	tokens(split(message));

	if (!tokens[0].compare("/CREATE"))
		createChannel(tokens);
	else if (!tokens[0].compare("/OPERATOR"))
		assignOperator(tokens);
	else if (!tokens[0].compare("/LIST"))
		listAll(tokens);
	else
		cmdUseage();
}

void	Server::cmdJoin(const int &cfd, const std::vector<std::string> &tokens) {
	std::string	text;
	if (!(tokens.size() == 3 || tokens.size() == 2)) {
		text = "Warning : Wrong number of parameters /HELP.\n";
		send(cfd, text.c_str(), text.length(), 0);
		return;
	}
	if (_serverChannels.find(tokens[1]) == _serverChannels.end()) {
		text = "ERROR: A server with this name doesn't exist.\n";
		send(cfd, text.c_str(), text.length(), 0);
		return;
	}
	if (_serverClients[cfd]->getChannel() != "\0") {
		_serverChannels[_serverClients[cfd]->getChannel()]->removeClient(cfd);
	}
	_serverChannels[tokens[1]]->joinChannel(*_serverClients[cfd], tokens);
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
			_serverChannels[tokens[1].substr(1)]->setKey("");
		if (condition)
			send(cfd, "You have set the channel to be password protected.\n", 51, 0);
		else
			send(cfd, "You made the channel passwordless.\n", 35, 0);
	}
	else if (tokens[2][1] == 'o') { //seg
		_serverChannels[tokens[1].substr(1)]->setO(condition);
		if (tokens.size() == 4) {
			for (size_t i = 0; i < _serverChannels[tokens[1].substr(1)]->getUsers().size(); ++i) {
				if (_serverChannels[tokens[1].substr(1)]->getUsers()[i]->getUsername() == tokens[3]) { //seg
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
	if (bytesRead > 0) {
		buff[bytesRead - 1] = '\0';
		message = buff;
		serverCmdParsing(message);
	} else if (bytesRead == 0) {
		std::cout << "EXIT: EOF on input." << std::endl;
		return;
	}
}

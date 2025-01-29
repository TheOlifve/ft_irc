#include "Server.hpp"

void	Server::cmdHelp(int	cfd) {
	std::string	text("/CHANNELS - See available channels.\n/JOIN 'CHANNEL NAME' 'KEY (optional)' - Join to a channel.\n/DC - Disconnect from current channel.\n");

	text.append("/UN 'New username' - Change username.\n/NN 'New nickname' - Change nickname.\n/PM 'Username' - Send private message.\n/EXIT - Leave server.\nType without quotes.\n");
	if (_serverClients[cfd]->getOp()) {
		text.append("KICK - Eject a client from the channel.\nINVITE - Invite a client to a channel.\n/TOPIC - Change or view the channel topic.\n/MODE - Change the channel’s mode.\n");
	}
	send(cfd, text.c_str(), text.length(), 0);
	text.clear();
}

void	Server::cmdParsing(int cfd, std::vector<std::string> &tokens) {
	if (!tokens[0].compare("/HELP"))
		cmdHelp(cfd);
	else if (!tokens[0].compare("/JOIN"))
		cmdJoin(cfd, tokens);
	else
		send(cfd, "Unknown command.\n", 17, 0);
}

void	Server::clientInput(int i) {
	int							bytesRead;
	int							cfd = _pfd[i].fd;
	char						buff[1024];
	std::vector<std::string>	tokens;

	bytesRead = read(cfd, buff, sizeof(buff) - 1);
	if (bytesRead > 1024) {
		send(cfd, "ERROR: Out of buffer(1024).\n", 28, 0);
		return;
	} else if (bytesRead > 0) {
		buff[bytesRead - 1] = '\0';
		tokens = split(buff);
		if (tokens.size() <= 0)
			send(cfd, "ERROR: Too short.\n", 18, 0);
		if (!_serverClients[cfd]->getAuthorized())
			authorization(cfd, i, tokens[0]);
		else if (!_serverClients[cfd]->getName())
			addUsername(cfd, tokens[0]);
		else
			cmdParsing(cfd, tokens);
	} else if (bytesRead == 0) {
		send(cfd, "EXIT: EOF on input.\n", 20, 0);
		std::cout << "Client [" << cfd << "] : disconnected (EOF on input)" << std::endl;
		removeClient(i);
		return;
	}
	memset(buff, 0, 1024);
}

void	Server::createChannel(const std::vector<std::string> &tokens) {
	if (tokens.size() != 3) {
		std::cout << "Wrong number of parameters: Usage /CREATE 'NAME' 'KEY'." << std::endl;
		return;
	}
	_serverChannels.insert(std::pair<std::string, Channel *>(tokens[1], new Channel(tokens[1], tokens[2])));
	std::cout << "Channel " << tokens[1] << " successfully created." << std::endl;
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

void	Server::serverCmdParsing(const std::string &message) {
	std::vector<std::string>	tokens(split(message));

	if (!tokens[0].compare("/CREATE"))
		createChannel(tokens);
	else
		std::cout << "Unknown command." << std::endl;
}

void	Server::cmdJoin(const int &cfd, const std::vector<std::string> &tokens) {
	if (tokens.size() == 3 || tokens.size() == 2)
		;
	else {
		std::cout << "Wrong number of parameters: Usage /JOIN 'NAME' 'KEY'." << std::endl;
		return;
	}
	if (_serverChannels.find(tokens[1]) == _serverChannels.end()) {
		send(cfd, "ERROR: A server with this name doesn't exist.\n", 46, 0);
		return;
	}
	if (_serverClients[cfd]->getChannel() != "\0") {
		_serverChannels[_serverClients[cfd]->getChannel()]->removeClient(cfd);
	}
	_serverChannels[tokens[1]]->joinChannel(*_serverClients[cfd], tokens);
}

void	Server::serverInput(void) {
	int			bytesRead;
	char		buff[1024];
	std::string	message;

	bytesRead = read(STDIN_FILENO, buff, sizeof(buff) - 1);
	if (bytesRead > 0) {
		buff[bytesRead - 1] = '\0';
		message = buff;
		serverCmdParsing(message);
	} else if (bytesRead == 0) {
		std::cout << "EXIT: EOF on input." << std::endl;
		return;
	}
}
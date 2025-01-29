#include "Server.hpp"

void	Server::addUsername(int cfd, std::string username) {
	std::string	tmp("You are logged in as ");

	if (!(username.compare("\0")))
		username = "Guest";
	_serverClients[cfd]->setUsername(username);
	_serverClients[cfd]->setName(true);
	std::cout << "Client [" << cfd << "] : logged in as " << username << std::endl;
	tmp.append(username);
	tmp.append(".\nEnter the command or type /HELP to see available commands.\n");
	send(cfd, tmp.c_str(), tmp.length(), 0);
}

void	Server::authorization(int cfd, int i,std::string password) {
	if (password.compare(_password)) {
		send(cfd, "Wrong password.\nServer closed connection.\n", 42, 0);
		removeClient(i);
		std::cout << "Client [" << cfd << "] : disconected (wrong password)." << std::endl;
		return ;
	}
	_serverClients[cfd]->setAuthorized(true);
	send(cfd, "Now enter your username.\n", 25, 0);
}
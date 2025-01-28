#include "Server.hpp"

void	Server::connectionMsgServ(int cfd) {
	std::cout << "Client [" << cfd << "] : connected to the server." << std::endl;
}

void	Server::connectionMsgClient(int cfd) {
	send(cfd, "You successfully connected to the server.\nEnter the password.\nIn case of a inaccuracy you will be disconnected.\n"\
						, 112, 0);
}

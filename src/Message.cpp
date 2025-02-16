#include "Server.hpp"
#include "Message.hpp"

std::string numericReply(int code) {
	char buf[4];
	snprintf(buf, sizeof(buf), "%03d", code);
	return std::string(buf);
}

void	Server::sendMessage(const int &cfd, const int code, const std::string token) {
	std::string	text;
	std::string	prefix = ":ft_irc.local ";
	std::string	nick = _serverClients[cfd]->getNickname();

	switch (code)
	{
		case RPL_WELCOME:
			text = prefix + numericReply(code) + " " + nick + " :Welcome to the Internet Relay Network " + 
				  nick + "!" + _serverClients[cfd]->getUsername() + "@ft_irc.local\r\n";
			std::cout << "Client " << nick << "[" << cfd << "] : connected to the server." << std::endl;
			break;
		case RPL_YOURHOST:
			text = prefix + numericReply(code) + " " + nick + " :Your host is ft_irc.local, running version 1.0\r\n";
			break;
		case RPL_CREATED:
			text = prefix + numericReply(code) + " " + nick + " :This server was created " + 
				  "sometime\r\n";
			break;
		case RPL_MYINFO:
			text = prefix + numericReply(code) + " " + nick + " ft_irc.local 1.0 itkol\r\n";
			break;
		case RPL_JOINCHANNEL:
			text = prefix + numericReply(code) + " " + nick + " JOIN :" + token + "\r\n";
			break;
		case RPL_PART:
			text = prefix + numericReply(code) + " " + nick + " PART " + _serverClients[cfd]->getChannel() + " :" + token + "\r\n";
			break;
		case RPL_QUIT:
			text = "ERROR :Closing link: (" + nick + ") [Quit: " + token + "]\r\n";
			break;
		case RPL_HELPTXT:
			text = prefix + numericReply(code) + " " + nick + " :" + token + "\r\n";
			break;
		case RPL_TOPIC:
			text = prefix + numericReply(code) + " " + nick + " " + _serverClients[cfd]->getChannel() + " :" + token + "\r\n";
			break;
		case RPL_NOTOPIC:
			text = prefix + numericReply(code) + " " + nick + " " + _serverClients[cfd]->getChannel() + " :No topic is set\r\n";
			break;
		case RPL_LISTSTART:
			text = prefix + numericReply(code) + " " + nick + " Channel :Users Topic\r\n";
			break;
		case RPL_LIST:
			text = prefix + numericReply(code) + " " + nick + " " + token + "\r\n";
			break;
		case RPL_LISTEND:
			text = prefix + numericReply(code) + " " + nick + " :End of /LIST\r\n";
			break;
		case RPL_NAMREPLY:
			text = prefix + numericReply(code) + " " + nick + " = " + 
				  _serverClients[cfd]->getChannel() + " :" + token + "\r\n";
			break;
		case RPL_ENDOFNAMES:
			text = prefix + numericReply(code) + " " + nick + " " + _serverClients[cfd]->getChannel() + " :End of /NAMES list\r\n";
			break;
		case RPL_MODE:
			text = prefix + numericReply(code) + " " + _serverClients[cfd]->getChannel() + " " + token + "\r\n";
			break;
		case ERR_SERVICESFULL:
			text = prefix + numericReply(code) + " * :Cannot join, server is full\r\n";
			std::cout<< "Client [" << cfd << "] : connection failed (server is full).\n";
			break;
		case ERR_USERONCHANNEL:
			text = prefix + numericReply(code) + " " + nick + " " + token + " :is already on channel\r\n";
			break;
		case ERR_NOTONCHANNEL:
			text = prefix + numericReply(code) + " " + nick + " " + token + " :You're not on that channel\r\n";
			break;
		case ERR_NOSUCHCHANNEL:
			text = prefix + numericReply(code) + " " + nick + " " + token + " :No such channel\r\n";
			break;
		case ERR_MSGTOOLONG:
			text = prefix + numericReply(code) + " " + nick + " :Message too long\r\n";
			break;
		case ERR_NICKNAMEINUSE:
			text = prefix + numericReply(code) + " * :" + token + " is already in use\r\n";
			break;
		case ERR_PASSWDMISMATCH:
			text = "ERROR :" + numericReply(code) + " * :Password incorrect\r\n";
			std::cout << "Client [" << cfd << "] : disconnected (incorrect password)" << std::endl;
			break;
		case ERR_NEEDMOREPARAMS:
			text = "ERROR :" + numericReply(code) + " * " + token + " :Not enough parameters\r\n";
			break;
		case ERR_BADCHANNELKEY:
			text = prefix + numericReply(code) + " " + nick + " " + token + " :Cannot join channel (+k)\r\n";
			break;
		case RPL_PING:
			text = "PING ft_irc\r\n";
			break;
		case ERR_CHANOPRIVSNEEDED:
			text = prefix + numericReply(code) + " " + nick + " " + token + " :You're not channel operator\r\n";
			break;
		case RPL_KICK:
			text = prefix + numericReply(code) + " " + nick + "!" + _serverClients[cfd]->getUsername() + "@ft_irc KICK " + 
				  _serverClients[cfd]->getChannel() + " " + token + "\r\n";
			break;
		case RPL_INVITING:
			text = prefix + numericReply(code) + " " + nick + "!" + _serverClients[cfd]->getUsername() + "@ft_irc INVITE " + token + "\r\n";
			break;
		default:
			break;
	}
	std::cout << "To client [" << cfd << "] - " << text;
	send(cfd, text.c_str(), text.length(), 0);
}
#include "Server.hpp"
#include "Message.hpp"

void	Server::sendMessage(const int &cfd, const int code, const std::string token) {
	std::string	text;

	switch (code)
	{
		case RPL_HELPTXT:
			text = ":ft_irc 705 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_PART:
			text = ":";
			text.append(_serverClients[cfd]->getNickname());
			text.append("!");
			text.append(_serverClients[cfd]->getUsername());
			text.append("@ft_irc PART ");
			text.append(_serverClients[cfd]->getChannel());
			text.append(" :");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_WELCOME:
			text = ":ft_irc 001 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :Welcome to the Internet Relay Network\r\n");
			std::cout << "Client " << _serverClients[cfd]->getNickname() << "[" << cfd << "] : connected to the server." << std::endl;
			break;
		case RPL_JOINCHANNEL:
			text = ":";
			text.append(_serverClients[cfd]->getNickname());
			text.append("!");
			text.append(_serverClients[cfd]->getUsername());
			text.append("@ft_irc JOIN ");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_TOPIC:
			text = ":ft_irc 332 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(_serverClients[cfd]->getChannel());
			text.append(" :");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_NOTOPIC:
			text = ":ft_irc 331 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(_serverClients[cfd]->getChannel());
			text.append(" :No topic is set\r\n");
			break;
		case RPL_NAMREPLY:
			text = ":ft_irc 353 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" = ");
			text.append(_serverClients[cfd]->getChannel());
			text.append(" :");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_ENDOFNAMES:
			text = ":ft_irc 366 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(_serverClients[cfd]->getChannel());
			text.append(" :End of /NAMES list\r\n");
			break;
		case RPL_MODE:
			text = ":ft_irc MODE ";
			text.append(_serverClients[cfd]->getChannel());
			text.append(" ");
			text.append(token);
			text.append("\r\n");
			break;
		// case RPL_NAMREPLY:
		// 	text = ":ft_irc 353 Olifve = #CH1 :@Olifve\r\n";
		// 	break;
		// case RPL_ENDOFNAMES:
		// 	text = ":ft_irc 366 Olifve #CH1 :End of /NAMES list\r\n";
		// 	break;
		case ERR_SERVICESFULL:
			text = ":ft_irc 471 * :Cannot join, server is full\r\n";
			std::cout<< "Client [" << cfd << "] : connection failed (server is full).\n";
			break;
		case ERR_USERONCHANNEL:
			text = ":ft_irc 443 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :is already on channel\r\n");
			break;
		case ERR_NOTONCHANNEL:
			text = ":ft_irc 442 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :You're not on that channel\r\n");
			break;
		case ERR_NOSUCHCHANNEL:
			text = ":ft_irc 403 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :No such channel\r\n");
			break;
		case ERR_MSGTOOLONG:
			text = ":ft_irc 414 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :Message too long\r\n");
			break;
		case ERR_NICKNAMEINUSE:
			text = ":ft_irc 433 * :";
			text.append(token);
			text.append(" is already in use\r\n");
			break;
		case ERR_PASSWDMISMATCH:
			text = ":ft_irc 464 * :Password incorrect\r\n";
			std::cout << "Client [" << cfd << "] :Password incorrect." << std::endl;
			break;
		case ERR_NEEDMOREPARAMS:
			text = ":ft_irc 461 * ";
			text.append(token);
			text.append(" :Not enough parameters\r\n");
			break;
		case ERR_BADCHANNELKEY:
			text = ":ft_irc 475 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :Cannot join channel (+k)\r\n");
			break;
		default:
			break;
	}
	std::cout << "Send to client - " << text;
	send(cfd, text.c_str(), text.length(), 0);
}
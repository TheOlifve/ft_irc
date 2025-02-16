#include "Server.hpp"
#include "Message.hpp"

void	Server::sendMessage(const int &cfd, const int code, const std::string token) {
	std::string	text;

	switch (code)
	{
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
			text.append("@ft_irc JOIN :");
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
		case RPL_QUIT:
			text = "ERROR :Closing link: (";
			text.append(_serverClients[cfd]->getNickname());
			text.append(") [Quit: ");
			text.append(token);
			text.append("]\r\n");
			break;
		case RPL_HELPTXT:
			text = ":ft_irc 705 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :");
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
		case RPL_LISTSTART:
			text = ":ft_irc 321 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" Channel :Users Topic\r\n");
			break;
		case RPL_LIST:
			text = ":ft_irc 322 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_LISTEND:
			text = ":ft_irc 323 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :End of /LIST\r\n");
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
			std::cout << "Client [" << cfd << "] : disconnected (incorrect password)" << std::endl;
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
		case RPL_PING:
			text = "PING ft_irc\r\n";
			break;
		case ERR_UNKNOWNCOMMAND:
			text = ":ft_irc 421 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append	(token);
			text.append(" :Unknown command\r\n");
			break;
		case ERR_NORECIPIENT:
			text = ":ft_irc 411 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :No recipient given (PRIVMSG)\r\n");
			break;
		case ERR_NOSUCHNICK:
			text = ":ft_irc 401 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :No such nick/channel\r\n");
			break;
		case ERR_USERNOTINCHANNEL:
			text = ":ft_irc 441 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" ");
			text.append(_serverClients[cfd]->getChannel());
			text.append(" :They aren't on that channel\r\n");
			break;
		case ERR_NOTEXTTOSEND:
			text = ":ft_irc 412 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :No text to send\r\n");
			break;
		case ERR_TOOMANYTARGETS:
			text = ":ft_irc 417 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :Too many recipients\r\n");
			break;
		case RPL_PRIVMSG:
			text = ":";
			text.append(_serverClients[cfd]->getNickname());
			text.append("!");
			text.append(_serverClients[cfd]->getUsername());
			text.append("@ft_irc PRIVMSG ");
			text.append(token);
			text.append("\r\n");
			send(_ClientsID[token.substr(0, token.find(' '))], text.c_str(), text.length(), 0);
			return;
		default:
			break;
	}
	std::cout << "To client [" << cfd << "] - " << text;
	send(cfd, text.c_str(), text.length(), 0);
}
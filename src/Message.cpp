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
			text = ":ft_irc 704 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_ENDOFHELP:
			text = ":ft_irc 705 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :End of HELP info\r\n");
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
			text = "PING :";
			text.append(token);
			text.append("\r\n");
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
		case ERR_ERRONEUSNICKNAME:
			text = ":ft_irc 432 ";
			if (!_serverClients[cfd]->getNickname().compare("\0"))
				text.append("*");
			else
				text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :Erroneous nickname\r\n");
			break;
		case ERR_ERRONEUSUSERNAME:
			text = ":ft_irc 433 ";
			if (!_serverClients[cfd]->getNickname().compare("\0"))
				text.append("*");
			else
				text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :Erroneous username\r\n");
			break;
		case RPL_CHANNELMODEOP:
			text = ":ft_irc 730 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :You have been assigned as operator in the channel\r\n");
			break;
		case RPL_INVITEONLY:
			text = ":ft_irc 731 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :You have set the channel to invite only.\r\n");
			break;
		case RPL_INVITEFREE:
			text = ":ft_irc 732 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :You have set the channel to be invite-free.\r\n");
			break;
		case RPL_TOPICOPONLY:
			text = ":ft_irc 733 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :You have set the channel's topic to be set by operators only.\r\n");
			break;
		case RPL_TOPICOPANYONE:
			text = ":ft_irc 734 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :You have set the channel's topic to be set by anyone.\r\n");
			break;
		case RPL_CHANNELPASS:
			text = ":ft_irc 735 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :You have set the channel to be password protected.\r\n");
			break;
		case RPL_CHANNELPASSLESS:
			text = ":ft_irc 736 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :You made the channel passwordless.\r\n");
			break;
		case RPL_ASSIGNOP:
			text = ":ft_irc 737 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :You have set the user to be an operator.\r\n");
			break;
		case ERR_NOTOPERATOR:
			text = ":ft_irc 738 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :ERROR: User is not an operator in this channel.\r\n");
			break;
		case RPL_REMOVEOPERATOR:
			text = ":ft_irc 739 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :You have removed the user from the operator list.\r\n");
			break;
		case RPL_REMOVEDOP:
			text = ":ft_irc 740 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :You have been removed from the operator list.\r\n");
			break;
		case RPL_USERLIMITSET:
			text = ":ft_irc 741 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :You have set the channel to have a user limit.\r\n");
			break;
		case RPL_USERLIMITREMOVED:
			text = ":ft_irc 742 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :You have removed the user limit from the channel.\r\n");
			break;
		case ERR_WRONGMODE:
			text = ":ft_irc 743 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :ERROR: Wrong channel mode.\r\n");
			break;
		case ERR_NOPERMISSION:
			text = ":ft_irc 744 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :ERROR: You don't have permission to change the channel mode.\r\n");
			break;
		case ERR_MODEPARAMS:
			text = ":ft_irc 745 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" :ERROR: Wrong number of parameters: Usage /MODE '#channel' '+/-mode' 'parameter'.\r\n");
			break;
		case ERR_KEYPARAMS:
			text = ":ft_irc 746 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :ERROR: Wrong number of parameters: Usage /MODE '#channel' '+/-k' 'key'.\r\n");
			break;
		case ERR_OPPARAMS:
			text = ":ft_irc 747 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :ERROR: Wrong number of parameters: Usage /MODE '#channel' '+/-o' 'username'.\r\n");
			break;
		case ERR_USERLIMITPARAMS:
			text = ":ft_irc 748 ";
			text.append(_serverClients[cfd]->getNickname());
			text.append(" ");
			text.append(token);
			text.append(" :ERROR: Wrong number of parameters: Usage /MODE '#channel' '+/-l' 'limit'.\r\n");
			break;
		default:
			break;
	}
	std::cout << "To client [" << cfd << "] - " << text;
	send(cfd, text.c_str(), text.length(), 0);
}
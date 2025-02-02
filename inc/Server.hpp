#ifndef SERVER_HPP
# define SERVER_HPP

#include "Irc.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Client;
class Channel;

class Server {
	public:
		Server();
		Server(int, char *, int);
		~Server();
		void	serverInput(void);
		void	startServer(void);
		void	createSocket(void);
		void	newConnection(void);
		void	cmdHelp(const int &, const std::vector<std::string> &);
		void	cmdQuit(const int &, const std::vector<std::string> &);
		void	cmdChannels(const int &, const std::vector<std::string> &);
		void	cmdJoin(const int &, const std::vector<std::string> &);
		void	cmdMode(const int &, const std::vector<std::string> &);
		void	cmdExit(const int &, const std::vector<std::string> &);
		void	cmdChangeUsername(const int &, const std::vector<std::string> &);
		void	cmdChangeNickname(const int &, const std::vector<std::string> &);
		void	cmdStatus(const int &cfd, const std::vector<std::string> &tokens);
		void	parseMode(const int &cfd, const std::vector<std::string> &tokens, bool condition);
		void	removeClient(int);
		void	clientSignIn(int);
		void	clientInput(int);
		void	createChannel(const std::vector<std::string> &);
		void	assignOperator(const std::vector<std::string> &);
		void	connectionMsgServ(int);
		void	connectionMsgClient(int);
		void	cmdParsing(const int &, const std::vector<std::string> &);
		void	authorization(int, std::string);
		void	addUsername(int, std::string);
		void	listAll(const std::vector<std::string> &);
		void	serverCmdParsing(const std::string &);
	private:
		int									_port;
		std::string							_password;
		int									_maxOnline;
		int									_currentOnline;
		pollfd								*_pfd;
		int									_serverSocket;
		sockaddr_in							_serverAddr;
		std::map<int, Client *>				_serverClients;
		std::map<std::string, int>			_ClientsID;
		std::map<std::string, Channel *>	_serverChannels;
		// std::map<int, Client *>	_serverClients;
};

std::string					itos(int number);
std::vector<std::string>	split(const std::string& str);

# endif
#ifndef SERVER_HPP
# define SERVER_HPP

#include "Irc.hpp"

class Client;
class Channel;

class Server {
	public:
		Server();
		Server(int, char *, int);
		~Server();
		int		createChannel(const int &, const std::vector<std::string> &);
		void	serverInput(void);
		void	startServer(void);
		void	createSocket(void);
		void	newConnection(void);
		void	cmdHelp(const int &, const std::vector<std::string> &);
		void	cmdPart(const int &, const std::vector<std::string> &);
		void	cmdQuit(const int &, const std::vector<std::string> &);
		void	cmdChannels(const int &, const std::vector<std::string> &);
		void	cmdJoin(const int &, const std::vector<std::string> &);
		void	cmdMode(const int &, const std::vector<std::string> &);
		void	cmdExit(const int &, const std::vector<std::string> &);
		void	message(const int &, const std::vector<std::string> &);
		void	cmdChangeUsername(const int &, const std::vector<std::string> &);
		void	cmdChangeNickname(const int &, const std::vector<std::string> &);
		void	cmdStatus(const int &cfd, const std::vector<std::string> &tokens);
		void	parseMode(const int &cfd, const std::vector<std::string> &tokens, bool condition);
		void	removeClient(int);
		void	clientSignIn(int);
		void	clientInput(int);
		void	assignOperator(const std::vector<std::string> &);
		void	cmdParsing(const int &, const std::vector<std::string> &);
		void	authorization(const int &, const std::vector<std::string> &);;
		void	addNickname(const int &, const std::string &);
		void	listAll(const std::vector<std::string> &);
		void	serverCmdParsing(const std::string &);
		void	sendMessage(const int &, const int, const std::string);
	private:
		std::string							_name;
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
};

void	replaceWhiteSpaces(std::string &str);
std::string	itos(int number);
std::vector<std::string>	split(std::string str);

# endif
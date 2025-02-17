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
		void	cmdPing(const int &, const std::vector<std::string> &);
		void	cmdHelp(const int &, const std::vector<std::string> &);
		void	cmdPart(const int &, const std::vector<std::string> &);
		void	cmdQuit(const int &, const std::vector<std::string> &);
		void	cmdJoin(const int &, const std::vector<std::string> &);
		void	cmdList(const int &, const std::vector<std::string> &);
		void	cmdMode(const int &, const std::vector<std::string> &);
		void	cmdUser(const int &, const std::vector<std::string> &);
		void	cmdNick(const int &, const std::vector<std::string> &);
		void	cmdStatus(const int &, const std::vector<std::string> &);
		void	cmdPrivmsg(const int &, const std::vector<std::string> &);
		void	parseMode(const int &, const std::vector<std::string> &, bool);
		void	removeClient(int);
		void	clientInput(int);
		void	assignOperator(const std::vector<std::string> &);
		void	cmdParsing(const int &, const std::vector<std::string> &);
		void	authorization(const int &, const std::vector<std::string> &);;
		void	sendMessage(const int &, const int, const std::string);

		bool	getListening(void) const;
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
		bool								_listening;
};

void						replaceWhiteSpaces(std::string &str);
std::string					itos(int number);
const std::string			buildMessage(const int from, const std::vector<std::string> &tokens);
std::vector<std::string>	split(std::string str);
bool						nickValidation(const std::string &token);

# endif
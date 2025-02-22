#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Irc.hpp"
#include "Client.hpp"

class Client;

class Channel {
	public:
		Channel();
		Channel(std::string, std::string);
		void	removeClient(int);
		void	joinChannel(Client &, const std::vector<std::string> &);
		void	channelMessage(const int &, const std::string &);
		~Channel();

		void	setI(bool);
		void	setT(bool);
		void	setK(bool);
		void	setL(bool);
		void	setKey(std::string);
		void	setLimit(int);
		void	addOp(const int &, const Client *);
		void	removeOp(const int &);

		int		getLimit(void) const;
		int		getOnline(void) const;
		bool	getI(void) const;
		bool	getT(void) const;
		bool	getK(void) const;
		bool	getL(void) const;
		std::string							getTopic(void) const;
		std::map<const int,const Client *>	getUsers(void) const;
		std::map<const int,const Client *>	getOps(void) const;
	private:
		std::string							_name;
		std::string							_key;
		std::string							_topic;
		int									_limit;
		int									_online;
		std::map<const int,const Client *>	_users;
		std::map<const int,const Client *>	_ops;
		bool		_i;
		bool		_t;
		bool		_k;
		bool		_l;
};

# endif
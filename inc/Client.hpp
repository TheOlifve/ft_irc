#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Irc.hpp"

class Client {
	public:
		Client();
		Client(int);
		int			getUserFd(void) const;
		std::string	getChannel(void) const;
		std::string	getUsername(void) const;
		std::string	getNickname(void) const;
		bool		getOp(void) const;
		bool		getPass(void) const;
		bool		getName(void) const;
		bool		getAuthorized(void) const;

		void	setChannel(const std::string);
		void	setUsername(const std::string);
		void	setNickname(const std::string);
		void	setOp(bool);
		void	setPass(bool);
		void	setName(bool);
		void	setAuthorized(bool);
		~Client();
	private:
		int			_userFd;
		std::string	_username;
		std::string	_nickname;
		bool		_name;
		bool		_op;
		bool		_authorized;
		bool		_pass;
		std::string	_channel;
};

# endif
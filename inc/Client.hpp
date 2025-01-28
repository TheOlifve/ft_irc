#ifndef CLIENT_HPP
# define CLIENT_HPP

#include "Irc.hpp"

class Client {
	public:
		Client();
		Client(int);
		int			getUserFd(void) const;
		std::string	getUsername(void) const;
		std::string	getNickname(void) const;
		bool		getOp(void) const;
		bool		getName(void) const;
		bool		getAuthorized(void) const;

		void	setUsername(std::string);
		void	setNickname(std::string);
		void	setOp(bool);
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
};

# endif
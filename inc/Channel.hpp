#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Irc.hpp"

class Channel {
	public:
		Channel();
		Channel(std::string, std::string);
		void	joinChannel(const Client &, const std::vector<std::string> &);
		void	channelMessage(const int &, const std::string &);
		~Channel();
	private:
		std::string							_name;
		std::string							_key;
		std::string							_topic;
		std::map<const int,const Client *>	_users;
		bool		_i;
		bool		_t;
		bool		_k;
		bool		_o;
		bool		_l;
};

# endif
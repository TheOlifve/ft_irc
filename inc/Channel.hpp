#ifndef CHANNEL_HPP
# define CHANNEL_HPP

#include "Irc.hpp"

class Client;

struct ChannelModes {
	bool invite_only;
	bool topic_restricted;
	bool key_required;
	bool user_limit;
	
	ChannelModes() : invite_only(false), topic_restricted(false), 
					 key_required(false), user_limit(false) {}
};

class Channel {
	public:
		Channel();
		Channel(std::string, std::string);
		~Channel();
		void								removeClientCh(int);
		void								joinChannel(Client &, const std::vector<std::string> &);
		void								channelMessage(const Client &, const int, const std::string);
		std::string							usersList(void);
 
		void								setI(bool);
		void								setT(bool);
		void								setK(bool);
		void								setO(bool);
		void								setL(bool);
		void								setKey(std::string);
		void								setLimit(int);
		void								setOp(const int &, const Client *);
		void 								setTopic(const std::string &topic);
		void								setMode(char mode, bool add, const std::string &param);

		int									getLimit(void) const;
		int									getOnline(void) const;
		bool								getI(void) const;
		bool								getT(void) const;
		bool								getK(void) const;
		bool								getO(void) const;
		bool								getL(void) const;
		std::string							getKey(void) const;
		std::string							getTopic(void) const;
		std::string							getName(void) const;
		std::map<const int,const Client *>	getOps(void) const;
		std::map<const int,const Client *>	getUsers(void) const;
	private:
		std::string							_name;
		std::string							_key;
		std::string							_topic;
		int									_limit;
		int									_online;
		std::map<const int,const Client *>	_users;
		std::map<const int,const Client *>	_ops;
		bool								_i;
		bool								_t;
		bool								_k;
		bool								_o;
		bool								_l;
		ChannelModes _modes;
};

# endif
#include "Channel.hpp"

Channel::Channel(): _name(""), _key(""), _topic(""), _users() \
					, _i(false), _t(false), _k(false), _o(false), _l(false) {}

Channel::Channel(std::string name, std::string key): _name(name), _key(key), _topic(""), _users() {
	_i = false;
	_t = false;
	_k = false;
	_o = false;
	_l = false;
}

void	Channel::channelMessage(const int &cfd, const std::string &message) {
	for (size_t i = 0; i < _users.size(); ++i) {
		int	tmpfd = _users[i]->getUserFd();
		std::string	prefix = _users[i]->getUsername();
		prefix.append(": ");
		prefix.append(message);
		if (cfd != tmpfd) {
			send(tmpfd, prefix.c_str(), prefix.length(), 0);
		}
	}
}

void	Channel::joinChannel(const Client &user, const std::vector<std::string> &tokens) {
	int	cfd = user.getUserFd();

	if (_users.find(cfd) != _users.end()) {
		send(cfd, "Warning : You are already in this channel.\n", 43, 0);
		return;
	}
	if (_k == true && tokens.size() != 3) {
		send(cfd, "Warning : Wrong number of parameters /JOIN 'NAME' 'KEY'.\n", 57, 0);
		return;
	}
	if (_k == false && tokens.size() != 2) {
		send(cfd, "Warning : Wrong number of parameters /JOIN 'NAME'.\n", 51, 0);
		return;
	}
	if (_k && _key.compare(tokens[2])) {
		send(cfd, "Warning : Wrong channel key.\n", 29, 0);
		return;
	}
	_users.insert(std::pair<const int,const Client *>(cfd, &user));
	std::cout << "Client " << user.getUsername() << "[" << cfd << "] : Joined to the " << tokens[1] << " channel." << std::endl;
	send(cfd, "Join : You have joined the channel.\n", 36, 0);
	channelMessage(cfd, " connects to the channel.\n");
}

Channel::~Channel() {
	std::cout << "[ Channel destructor called ]" << std::endl;
}


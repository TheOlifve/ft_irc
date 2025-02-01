#include "Channel.hpp"

Channel::Channel(): _name(""), _key(""), _topic(""), _limit(0), _online(0)\
					, _users(), _ops(), _i(false), _t(false), _k(false), _o(false), _l(false) {}

Channel::Channel(std::string name, std::string key): _name(name), _key(key), _topic(""), _limit(0), _online(0), _users(), _ops(){
	_i = false;
	_t = false;
	_k = false;
	_o = false;
	_l = false;
}

void	Channel::channelMessage(const int &cfd, const std::string &message) {
	std::map<const int, const Client *>::const_iterator	it = _users.begin();

	std::string	prefix = _users[cfd]->getUsername();
	prefix.append(": ");
	prefix.append(message);
	while (it != _users.end()) {
		int	tmpfd = it->second->getUserFd();
		if (cfd != tmpfd) {
			send(tmpfd, prefix.c_str(), prefix.length(), 0);
		}
		++it;
	}
}

void	Channel::joinChannel(Client &client, const std::vector<std::string> &tokens) {
	int	cfd = client.getUserFd();

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
	_users.insert(std::pair<const int,const Client *>(cfd, &client));
	std::cout << "Client " << client.getUsername() << "[" << cfd << "] : Joined to the " << tokens[1] << " channel." << std::endl;
	send(cfd, "You have joined the channel.\n", 29, 0);
	client.setChannel(tokens[1]);
	++_online;
	channelMessage(cfd, "connects to the channel.\n");
}

void	Channel::removeClient(int cfd) {
	std::cout << "Client " << _users[cfd]->getUsername() << "[" << cfd << "] : Leaved channel (" << _users[cfd]->getChannel() << ")." << std::endl;
	channelMessage(cfd, "leaved channel.\n");
	_users.erase(cfd);
	--_online;
}

Channel::~Channel() {
	std::cout << "[ Channel destructor called ]" << std::endl;
}

void	Channel::setI(bool i) {
	_i = i;
}

void	Channel::setT(bool t) {
	_t = t;
}

void	Channel::setK(bool k) {
	_k = k;
}

void	Channel::setO(bool o) {
	_o = o;
}

void	Channel::setL(bool l) {
	_l = l;
}

void	Channel::setKey(std::string key)
{
	_key = key;
}

void	Channel::setOp(const int &cfd, const Client *client) {
	_ops.insert(std::pair<const int,const Client *>(cfd, client));
}

void	Channel::setLimit(int limit) {
	_limit = limit;
}


int		Channel::getLimit(void) const {
	return _limit;
}
int		Channel::getOnline(void) const {
	return _online;
}

bool	Channel::getI(void) const {
	return _i;
}

bool	Channel::getT(void) const {
	return _t;
}

bool	Channel::getK(void) const {
	return _k;
}

bool	Channel::getO(void) const {
	return _o;
}

bool	Channel::getL(void) const {
	return _l;
}

std::map<const int,const Client *>	Channel::getUsers(void) const {
	return _users;
}

std::map<const int,const Client *>	Channel::getOps(void) const {
	return _ops;
}

std::string	Channel::getTopic(void) const {
	if (_topic.empty())
		return "";
	return _topic;
}
#include "Channel.hpp"

Channel::Channel(): _name("\0"), _key("\0"), _topic("\0"), _limit(0), _online(0)\
					, _users(), _ops(), _i(false), _t(false), _k(false), _o(false), _l(false) {}

Channel::Channel(std::string name, std::string key): _name(name), _key(key), _topic("\0"), _limit(0), _online(0), _users(), _ops(){
	_i = false;
	_t = false;
	_k = false;
	_o = false;
	_l = false;
}

void	Channel::channelMessage(const Client &client, const int code, const std::string token) {
	int													cfd = client.getUserFd();
	std::string											text;
	std::map<const int, const Client *>::const_iterator	it = _users.begin();

	switch (code)
	{
		case RPL_JOINCHANNEL:
			text = ":";
			text.append(client.getNickname());
			text.append("!");
			text.append(client.getUsername());
			text.append("@ft_irc JOIN :");
			text.append(_name);
			text.append("\r\n");
			break;
		case RPL_PART:
			text = ":";
			text.append(client.getNickname());
			text.append("!");
			text.append(client.getUsername());
			text.append("@ft_irc PART ");
			text.append(client.getChannel());
			text.append(" :");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_QUIT:
			text = ":";
			text.append(client.getNickname());
			text.append("!");
			text.append(client.getUsername());
			text.append("@ft_irc QUIT :");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_NICK:
			text = ":";
			text.append(token);
			text.append("!");
			text.append(client.getUsername());
			text.append("@ft_irc NICK :");
			text.append(client.getNickname());
			text.append("\r\n");
			break;
		case RPL_PRIVMSG:
			text = ":";
			text.append(client.getNickname());
			text.append("!");
			text.append(client.getUsername());
			text.append("@ft_irc PRIVMSG ");
			text.append(token);
			text.append("\r\n");
			break;
		case RPL_USERTOPICSET:
			text = ":";
			text.append(client.getNickname());
			text.append("!");
			text.append(client.getUsername());
			text.append("@ft_irc ");
			text.append("TOPIC ");
			text.append(token);
			text.append(" :");
			text.append(getTopic());
			text.append("\r\n");
			break;
		case ERR_CHANNELISFULL:
			text = ":ft_irc 752 ";
			text.append(client.getNickname());
			text.append(" :Cannot join, channel is full\r\n");
			break;
		default:
			break;
	}
	while (it != _users.end()) {
		int	tmpfd = it->second->getUserFd();
		if (cfd != tmpfd)
			send(tmpfd, text.c_str(), text.length(), 0);
		++it;
	}
	it = _ops.begin();
	while (it != _ops.end()) {
		int	tmpfd = it->second->getUserFd();
		if (cfd != tmpfd)
			send(tmpfd, text.c_str(), text.length(), 0);
		++it;
	}
}

bool	Channel::joinChannel(Client &client, const std::vector<std::string> &tokens) {
	int	cfd = client.getUserFd();

	if (_online == 0) {
		client.setOp(true);
		_ops.insert(std::pair<const int,const Client *>(cfd, &client));
	}
	else if (_online == _limit) {
		std::cout << "Client " << client.getNickname() << "[" << cfd << "] : Cannot join, channel is full." << std::endl;
		channelMessage(client, ERR_CHANNELISFULL, "");
		return false;
	}
	else
		_users.insert(std::pair<const int,const Client *>(cfd, &client));
	std::cout << "Client " << client.getNickname() << "[" << cfd << "] : Joined to the " << tokens[1] << " channel." << std::endl;
	++_online;
	channelMessage(client, RPL_JOINCHANNEL, "");
	return true;
}

std::string	Channel::usersList(void) {
	std::string	text;
	std::map<const int, const Client *>::const_iterator	it = _ops.begin();

	for (; it != _ops.end(); ++it) {
		text.append("@");
		text.append(it->second->getNickname());
		text.append(" ");
	}
	it = _users.begin();
	for (; it != _users.end(); ++it) {
		text.append(it->second->getNickname());
		text.append(" ");
	}
	if (text.length() > 0) {
		text[text.length() - 1] = '\0';
	}
	return (text);
}

void	Channel::removeClientCh(int cfd) {
	if (_users.find(cfd) != _users.end()) {
		std::cout << "Client " << _users[cfd]->getNickname() << "[" << cfd << "] : Leaved channel (" << _users[cfd]->getChannel() << ")." << std::endl;
		_users.erase(cfd);
	} else if (_ops.find(cfd) != _ops.end()) {
		std::cout << "Client " << _ops[cfd]->getNickname() << "[" << cfd << "] : Leaved channel (" << _ops[cfd]->getChannel() << ")." << std::endl;
		_ops.erase(cfd);
	}
	--_online;
}

Channel::~Channel() {}

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
	_users.erase(cfd);
	_ops.insert(std::pair<const int,const Client *>(cfd, client));
}

void	Channel::removeOp(const int &cfd) {
	_users.insert(std::pair<const int,const Client *>(cfd, _ops[cfd]));
	_ops.erase(cfd);
}

void	Channel::setLimit(int limit) {
	_limit = limit;
}

void	Channel::setTopic(std::string topic) {
	_topic = topic;
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

std::string	Channel::getKey(void) const {
	return _key;
}

std::string	Channel::getName(void) const {
	return _name;
}

std::string	Channel::getTopic(void) const {
	if (_topic.empty())
		return "";
	return _topic;
}
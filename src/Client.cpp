#include "Client.hpp"

Client::Client(): _userFd(-1), _username(""), _nickname(""), _op(false), _authorized(false), _channel("\0") {}

Client::Client(int userFd): _userFd(userFd), _username(""), _nickname(""), _op(false), _authorized(false), _channel("\0") {}

Client::~Client() {}


int			Client::getUserFd(void) const { return _userFd; }
std::string	Client::getUsername(void) const { return _username; }
std::string	Client::getNickname(void) const { return _nickname; }
std::string	Client::getChannel(void) const { return _channel; }
bool		Client::getOp(void) const { return _op; }
bool		Client::getName(void) const { return _name; }
bool		Client::getAuthorized(void) const { return _authorized; }

void	Client::setChannel(const std::string channel) { _channel = channel; }
void	Client::setUsername(const std::string username) { _username = username; }
void	Client::setNickname(const std::string nickname) { _nickname = nickname; }
void	Client::setOp(bool op) { _op = op; }
void	Client::setAuthorized(bool authorized) { _authorized = authorized; }
void	Client::setName(bool name) { _name = name; }
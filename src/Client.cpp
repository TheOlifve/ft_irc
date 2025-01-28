#include "Client.hpp"

Client::Client(): _userFd(-1), _username(""), _nickname(""), _op(false), _authorized(false) {}

Client::Client(int userFd): _userFd(userFd), _username(""), _nickname(""), _op(false), _authorized(false) {}

Client::~Client() {
	std::cout << "[ Client destructor called ]" << std::endl;
}


int			Client::getUserFd(void) const { return _userFd; }
std::string	Client::getUsername(void) const { return _username; }
std::string	Client::getNickname(void) const { return _nickname; }
bool		Client::getOp(void) const { return _op; }
bool		Client::getName(void) const { return _name; }
bool		Client::getAuthorized(void) const { return _authorized; }

void	Client::setUsername(std::string username) { _username = username; }
void	Client::setNickname(std::string nickname) { _nickname = nickname; }
void	Client::setOp(bool op) { _op = op; }
void	Client::setAuthorized(bool authorized) { _authorized = authorized; }
void	Client::setName(bool name) { _name = name; }
#include "Irc.hpp"

int	main(int argc, char **argv) {
	if (argc != 3) {
		std::cout << "Wrong number of parameters.\nUsage: ./ircserv <port> <password>\n";
		return (1);
	}
	Server serv(atoi(argv[1]), argv[2], 2);

	if (serv.getListening() == false)
		return (1);
	try {
		serv.startServer();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}

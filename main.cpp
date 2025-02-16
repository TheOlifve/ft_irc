#include "Irc.hpp"

int	main(int argc, char **argv) {
	std::cout << "Starting IRC server..." << std::endl;
	if (argc != 3) {
		std::cout << "Wrong number of parameters.\nUsage: ./ircserv <port> <password>\n";
		return (1);
	}
	std::cout << "Initializing on port " << argv[1] << "..." << std::endl;
	Server serv(atoi(argv[1]), argv[2], 2);

	if (serv.getListening() == false)
		return (1);
	std::cout << "Server is ready and listening for connections." << std::endl;
	try {
		serv.startServer();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}

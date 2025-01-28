#include "Server.hpp"
#include "Irc.hpp"

int	main(int argc, char **argv) {
	(void)argv;
	if (argc != 3) {
		std::cout << "Wrong number of parameters.\nUsage: ./ircserv <port> <password>\n";
		return (1);
	}
	Server serv(atoi(argv[1]), argv[2], 2);

	serv.startServer();
	return 0;
}
	// int ret;
	// while (1) {
	// 	ret = poll(fds, 1, 5000);
	// 	if (ret) {
	// 		char buff[1024];

	// 		int n = read(clientSocket, buff, sizeof(buff) - 1);
	// 		buff[n] = '\0';
	// 		std::cout << buff;
	// 	}
	// 	else
	// 		std::cout << "Timeout";
	// }
	
// int	main(int argc, char **argv) {
// 	(void)argv;
// 	if (argc != 3) {
// 		std::cout << "Wrong number of parameters.\nUsage: ./ircserv <port> <password>\n";
// 		return (1);
// 	}

// 	return (0);
// }
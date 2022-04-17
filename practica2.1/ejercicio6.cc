#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#define NUM_THREADS 5

class MessageThread {
public:
	MessageThread(int s) :sd(s) {};
	virtual ~MessageThread() {};


	void do_message() {
		while (true) {
			char host[NI_MAXHOST];
			char server[NI_MAXSERV];
			char buff[256];
			struct sockaddr soc;
			socklen_t addrlen = sizeof(soc);

			ssize_t size = recvfrom(sd, buff, 255, 0, &soc, &addrlen); //recibiendo los mensajes en bucle 
			getnameinfo(&soc, addrlen, host, NI_MAXHOST, server, NI_MAXSERV, NI_NUMERICHOST);


			//ejecuta el comando dado
			std::cout << size << " bytes de " << host << " " << server << "  Thread: " << pthread_self() << "\n";


			char buffer[256];
			memset((void*)buffer, '\0', 256);
			time_t rawtime;
			time_t date;
			struct tm* info;

			switch (buff[0]) {
			case 'd':
				time(&date);
				info = localtime(&date);
				bytes = strftime(buffer, 80, "%F", info);
				buffer[bytes] = '\0';
				sendto(sd, buffer, 256, 0, &soc, addrlen);

				break;
			case 't':
				time(&rawtime);
				info = localtime(&rawtime);
				bytes = strftime(buffer, 80, "%r", info);
				buffer[bytes] = '\0';
				sendto(sd, buffer, 256, 0, &soc, addrlen);

				break;
			case 'q':
				std::cout << "Saliendo..." << std::endl;
				break;

			default:
				std::cout << "Comando no soportado " << buffer << std::endl;
				break;
			}
			sleep(3);

		}
	}
private:
	int sd;
	size_t bytes;
};
extern "C" void* _do_message(void* _mt) {
	MessageThread* mt = static_cast<MessageThread*>(_mt);
	mt->do_message();
	delete mt;

	return 0;

}
int main(int argc, char** argv)
{

	//inicializacion de sockets
	struct addrinfo hints;
	struct addrinfo* res;

	memset((void*)&hints, '\0', sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rc != 0) {
		std::cout << "error getaddrinfo(): " << gai_strerror(rc) << std::endl;
		return -1;
	}



	int sd = socket(res->ai_family, res->ai_socktype, 0);
	bind(sd, res->ai_addr, res->ai_addrlen);
	listen(sd, 15);
	freeaddrinfo(res);


	//inicializacion de los hilos
	for (int i = 0; i <= NUM_THREADS; i++) {
		pthread_t tid;
		pthread_attr_t attr;

		MessageThread* mt = new MessageThread(sd);

		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		pthread_create(&tid, &attr, _do_message, static_cast<void*>(mt));
	}


	char c = 'p';
	while (c != 'q') {
		std::cin >> c;
	}

	freeaddrinfo(res);
	close(sd);

	return 0;
}
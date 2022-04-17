#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>


#include <pthread.h>

//salida y envio de mensajes de threads
class MessageThread {
public:
	MessageThread(int s) :sd(s) {};
	virtual ~MessageThread() {};

	void do_message() {
		while (!fin) {

			char buff[256];
			ssize_t size = recv(sd, &buff, 255, 0);

			if (size == 0) {
				fin = true;
				std::cout << "Saliendo..." << std::endl;
			}

			else {
				send(sd, &buff, size, 0);
				std::cout << "Thread: " << pthread_self() << std::endl;


			}

		}
	}
private:
	int sd;
	bool fin;
};
extern "C" void* _do_message(void* _mt) {
	MessageThread* mt = static_cast<MessageThread*>(_mt);
	mt->do_message();
	delete mt;

	return 0;

}
int main(int argc, char** argv)
{
	//Inicializar sockets
	struct addrinfo hints;
	struct addrinfo* res;

	memset((void*)&hints, '\0', sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	char host[NI_MAXHOST];
	char server[NI_MAXSERV];

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

	if (rc != 0) {
		std::cout << "error getaddrinfo(): " << gai_strerror(rc) << std::endl;
		return -1;
	}



	int sd = socket(res->ai_family, res->ai_socktype, 0);
	bind(sd, res->ai_addr, res->ai_addrlen);
	listen(sd, 15);
	freeaddrinfo(res);

	struct sockaddr cliente;

	socklen_t client_len = sizeof(cliente);
	//gestion de threads
	while (true) {
		int nThread = accept(sd, (struct sockaddr*)&cliente, &client_len);

		pthread_t tid;
		pthread_attr_t attr;

		MessageThread* mt = new MessageThread(nThread);

		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		pthread_create(&tid, &attr, _do_message, static_cast<void*>(mt));


	}


	char c;
	std::cin >> c;





	return 0;
}
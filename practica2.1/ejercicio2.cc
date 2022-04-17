#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>


//procesa el mensage detectando el comando
void processMsg(int sd, char command, struct sockaddr* addr, socklen_t addrlen, bool& exit, char* msg)
{
	time_t t;
	char timeInfo[80];
	size_t bytes;

	switch (command)
	{
	case 't':
		time(&t);
		bytes = strftime(timeInfo, sizeof(timeInfo), "%T", localtime(&t));

		sendto(sd, timeInfo, bytes, 0, addr, addrlen);
		break;

	case 'd':
		time(&t);
		bytes = strftime(timeInfo, sizeof(timeInfo), "%F", localtime(&t));

		sendto(sd, timeInfo, bytes, 0, addr, addrlen);
		break;

	case 'q':
		exit = true;
		strcpy(msg, "Saliendo...");
		sendto(sd, "", 0, 0, addr, addrlen);
		break;

	default:
		strcpy(msg, "Comando no soportado");
		sendto(sd, "", 0, 1, addr, addrlen);
		break;
	}
}


int main(int argc, char* argv[])
{
	struct addrinfo hints;

	// Inicializamos los atributos del struct a 'null', y luego damos valor a aquellos que queremos
	memset((void*)&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	struct addrinfo* res;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
	if (rc != 0) { std::cout << gai_strerror(rc); return -1; }


	int sd = socket(res->ai_family, res->ai_socktype, 0); // Crea el endpoint para la comunicación y devuelve un descriptor
	if (sd == -1) { std::cout << gai_strerror(sd);	return -1; }

	bind(sd, res->ai_addr, res->ai_addrlen); // Asigna la dirección especificada al socket. Se puede decir que se le asigna un nombre al socket que es la direccion
	// Aquí el socket ya está preparado para la comunicación


	// Bucle para cada mensaje -> cada iteración es un mensaje
	bool exit = false;
	while (!exit)
	{
		char host[NI_MAXHOST]; //host (la IP)
		char serv[NI_MAXSERV]; //servicio (el puerto)

		struct  sockaddr src_addr; //dirección del socket
		socklen_t addrlen = sizeof(struct sockaddr); // Tamaño de la dirección
		char buffer[80]; // Donde se almacenará el mensaje (en este caso, el comando)
		char msg[80] = ""; //la respuesta que vamos a producir


		// Recibe el mensaje correspondiente a través de la conexión en el socket
		ssize_t nBytes = recvfrom(sd, (void*)buffer, sizeof(buffer), 0, &src_addr, &addrlen);
		buffer[nBytes] = '\0'; // Para indicar final del mensaje y final de cadena

		// Convierte la dirección del socket a su correspondiente host (IP) y servicio (puerto)
		getnameinfo(&src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

		processMsg(sd, buffer[0], &src_addr, addrlen, exit, msg);

		std::cout << nBytes << " bytes de " << host << ":" << serv << '\n';
		std::cout << msg << '\n';
	}

	freeaddrinfo(res);
	return 0;
}
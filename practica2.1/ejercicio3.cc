#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>



int main(int argc, char* argv[])
{
	struct addrinfo hints;

	// Inicializamos los atributos del struct a 'null', y luego damos valor a aquellos que queremos
	memset((void*)&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	struct addrinfo* res;

	int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
	if (rc != 0) { std::cout << gai_strerror(rc); return -1; } //Ip o puerto no conocidos


	int sd = socket(res->ai_family, res->ai_socktype, 0); // Crea el endpoint para la comunicaci�n y devuelve un descriptor
	if (sd == -1) { std::cout << gai_strerror(sd);	return -1; } // socket error

	bind(sd, res->ai_addr, res->ai_addrlen); // Asigna la direcci�n especificada al socket. Se puede decir que se le asigna un nombre al socket
	// Aqu� el socket ya est� preparado para la comunicaci�n


	struct  sockaddr src_addr; // Donde guardaremos la direcci�n del socket
	socklen_t addrlen = sizeof(struct sockaddr); // Tama�o de la direcci�n
	char buffer[80]; // Donde se almacenar� el mensaje (en este caso, el comando)

	src_addr = *res->ai_addr;
	strcpy(buffer, argv[3]);


	//parte que difiere del ejercicio 2, aqui enviaremos el comando al servidor para que el lo analice
	// Env�o al servidor
	sendto(sd, buffer, sizeof(buffer), 0, &src_addr, addrlen);

	// Recibe el mensaje correspondiente a trav�s de la conexi�n en el socket
	ssize_t nBytes = recvfrom(sd, (void*)buffer, sizeof(buffer), 0, &src_addr, &addrlen);
	buffer[nBytes] = '\0'; // Para indicar final del mensaje y final de cadena

	std::cout << buffer << '\n';
}
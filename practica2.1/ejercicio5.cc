#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <time.h>


int main(int argc, char* argv[])
{
    struct addrinfo hints;

    memset((void*)&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (rc != 0) { std::cout << gai_strerror(rc); return -1; }


    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if (sd == -1) { std::cout << gai_strerror(sd);  return -1; }

    bind(sd, res->ai_addr, res->ai_addrlen);

    connect(sd, res->ai_addr, res->ai_addrlen); //Se conecta al servidor

    freeaddrinfo(res);

    //Comunicación continua para responder al echo
    while (true)
    {
        socklen_t addrlen = sizeof(struct sockaddr);
        char buffer[80];

        //Como clientes recogemos el buffer por entrada y lo enviamos
        std::cin >> buffer;
        send(sd, buffer, sizeof(buffer), 0);

        if (!strcmp(buffer, "Q") || !strcmp(buffer, "q"))
            break;

        //Liberamos el buffer
        ssize_t nBytes = recv(sd, (void*)buffer, sizeof(buffer), 0);
        buffer[nBytes] = '\0';

        std::cout << buffer << '\n';
    }
}
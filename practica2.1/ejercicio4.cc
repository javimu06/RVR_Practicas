#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <iostream>

int main(int argc, char* argv[])
{
    //Inicializar sockets
    struct addrinfo hints;

    memset((void*)&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* res;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);
    if (rc != 0) { std::cout << gai_strerror(rc); return -1; }


    int sd = socket(res->ai_family, res->ai_socktype, 0);
    if (sd == -1) { std::cout << gai_strerror(sd);	return -1; }

    bind(sd, res->ai_addr, res->ai_addrlen);
    freeaddrinfo(res);
    //
    listen(sd, 16);
    bool exit = false;

    while (!exit)
    {
        //Establece conexion
        char host[NI_MAXHOST];
        char serv[NI_MAXSERV];

        struct  sockaddr src_addr;
        socklen_t addrlen = sizeof(struct sockaddr);
        char msg[80] = "";

        int sd_client = accept(sd, &src_addr, &addrlen);

        getnameinfo(&src_addr, addrlen, host, NI_MAXHOST, serv, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "Conexion desde " << host << " " << serv << '\n';
        //Salida  y envio del buffer
        while (!exit)
        {
            char buffer[80];


            ssize_t nBytes = recv(sd_client, (void*)buffer, sizeof(buffer), 0); //Recibe el buffer
            buffer[nBytes] = '\0';

            send(sd_client, buffer, nBytes, 0); //Envia el buffer

            if (nBytes == 0 || !strcmp(buffer, "Q") || !strcmp(buffer, "q"))    //Fin de comunicacion
            {
                exit = true;

            }
        }
    }
    std::cout << "Conexion Terminada" << '\n';
    return 0;
}
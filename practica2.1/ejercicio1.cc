#include <sys/socket.h> 
#include <netdb.h>
#include <sys/types.h>

#include <string.h>
#include <iostream>
#include <iomanip>

int main(int argc, char** argv) {
    struct addrinfo hints;
    struct addrinfo* res;

    memset((void*)&hints, 0, sizeof(addrinfo));

    if (argc < 2) {
        std::cerr << "Entrada no valida, prueba con: ./ej1.o www.google.com\n";
        return -1;
    }

    // permite filtrar las direcciones
    hints.ai_family = AF_UNSPEC;

    int ret = getaddrinfo(argv[1], argv[2], &hints, &res);

    if (ret != 0) {
        std::cerr << "Error: Nombre o servicio desconocido -> " << argv[1] << " " << argv[2] << "\n";
        return -1;
    }

    for (auto i = res; i != nullptr; i = i->ai_next) {
        char host[NI_MAXHOST];

        if (getnameinfo(i->ai_addr, i->ai_addrlen, host, NI_MAXHOST, NULL, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) != 0) {
            std::cerr << "Error al recoger el nombre de una de las direcciones\n";
            freeaddrinfo(res);
            return -1;
        }

        std::cout << std::left << "Host: " << std::setw(25) << host
            << " Family: " << std::setw(3) << i->ai_family
            << " SocketType: " << i->ai_socktype << std::endl;
    }

    freeaddrinfo(res);

    return 0;
}
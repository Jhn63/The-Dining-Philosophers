#include <iostream>
#include <string.h>
#include <string>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main(int argc, char** argv) {
    //Criando um Socket TCP IPv4 pra listening
    int listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket <= 0) {
        return -1;
    }

    // Definindo endereço de acesso do socket
    sockaddr_in address;
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;         // IPv4
    address.sin_addr.s_addr = INADDR_ANY; // permite conectar por qualquer interface de rede
    address.sin_port = htons(PORT);       // definição da porta e ajuste de endian

    int opt = 1;
    setsockopt(listening_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // vinculando socket listening com endreço
    if (bind(listening_socket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        close(listening_socket);
        return -2;
    }


    // iniciando socket listenig, permitindo até 3 conexões
    if (listen(listening_socket, 3) < 0) {
        close(listening_socket);
        return -3;
    }

    // aceitando conexão
    while (true) {
        int new_socket = accept(listening_socket, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            close(listening_socket);
            return -4;
        }

        std::string response = "Hello Change!\n";
        send(new_socket, response.c_str(), response.size(), 0);

        close(new_socket);
    }

    close(listening_socket);
    return 0;
}

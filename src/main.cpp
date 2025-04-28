#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <locale>
#include "semaphore.cpp"
#include "posix.cpp"
#include "posix_aging.cpp"

void* task(void* arg) {
    int* clientSocket = static_cast<int*>(arg);

    bool running = true;
    while(running) {

        std::string msg =
        "\n"
        "######   ##   ##  #######           #####     ####    ##   ##   ####    ##   ##    ####            ######   ##   ##   ####    ####      #####    #####    #####   ######   ##   ##  #######  ######    #####\n"
        "# ## #   ##   ##   ##   #            ## ##     ##     ###  ##    ##     ###  ##   ##  ##            ##  ##  ##   ##    ##      ##      ##   ##  ##   ##  ##   ##   ##  ##  ##   ##   ##   #   ##  ##  ##   ##\n"
        "  ##     ##   ##   ## #              ##  ##    ##     #### ##    ##     #### ##  ##                 ##  ##  ##   ##    ##      ##      ##   ##  #        ##   ##   ##  ##  ##   ##   ## #     ##  ##  #\n"
        "  ##     #######   ####              ##  ##    ##     ## ####    ##     ## ####  ##                 #####   #######    ##      ##      ##   ##   #####   ##   ##   #####   #######   ####     #####    #####\n"
        "  ##     ##   ##   ## #              ##  ##    ##     ##  ###    ##     ##  ###  ##  ###            ##      ##   ##    ##      ##   #  ##   ##       ##  ##   ##   ##      ##   ##   ## #     ## ##        ##\n"
        "  ##     ##   ##   ##   #            ## ##     ##     ##   ##    ##     ##   ##   ##  ##            ##      ##   ##    ##      ##  ##  ##   ##  ##   ##  ##   ##   ##      ##   ##   ##   #   ##  ##  ##   ##\n"
        " ####    ##   ##  #######           #####     ####    ##   ##   ####    ##   ##    #####           ####     ##   ##   ####    #######   #####    #####    #####   ####     ##   ##  #######  #### ##   #####\n\n"
        "1. Método por Semáforo\n"
        "2. Método com monitores POSIX\n"
        "3. Método com monitores POSIX e Aging (Anti-Starvation)\n"
        "4. Sair\n"
        "Escolha uma opção: ";
        send(*clientSocket, msg.c_str(), msg.size(), 0);

        char buffer[1024] = {0};
        int bytesReceived = recv(*clientSocket, buffer, sizeof(buffer), 0);

        std::string message(buffer, bytesReceived);
        int option = std::stoi(message);

        switch (option) {
            case 1:
                // Método por Semáforo
                {
                    SemaphoreDiningTable table(5, *clientSocket);
                    table.run(); // A execução continuará indefinidamente até ser interrompida
                }
                break;
            case 2:
                // Método com monitores POSIX
                {
                    PosixDiningTable table(5, *clientSocket);
                    table.run(); // A execução continuará indefinidamente até ser interrompida
                }
                break;
            
            case 3:
                // Método com monitores POSIX com mecanismo de Aging
                {
                    PosixAgingDiningTable table(5, *clientSocket);
                    table.run(); // A execução continuará indefinidamente até ser interrompida
                }
                break;
            
            case 4:
                msg = "Saindo do programa...\n";
                send(*clientSocket, msg.c_str(), msg.size(), 0);

                running = false;
                break;

            default:
                msg = "Opção inválida. Por favor, selecione uma opção entre 1 e 4.\n";
                send(*clientSocket, msg.c_str(), msg.size(), 0);

                option = 0;
                break;
        }

    }
    close(*clientSocket);
    delete clientSocket;
    pthread_exit(nullptr);
}

/* Classe responsavel em manter o servidor,
* receber conecção e gerar serviço
*/
class Server {
private:
    const int PORT = 8080;
    const int LIMIT = 10;
    int serverSocket;
    int addrlen;
    sockaddr_in address;

public:
    Server() {
        serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (serverSocket <= 0) {
            std::cerr << "Server socket erro " << std::endl;
            exit(-1);
        }

        addrlen = sizeof(address);

        address.sin_family = AF_INET;         // IPv4
        address.sin_addr.s_addr = INADDR_ANY; // permite conectar por qualquer interface de rede
        address.sin_port = htons(PORT);       // definição da porta e ajuste de endian

        int opt = 1;
        setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // vinculando socket listening com endereço
        if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Bind erro" << std::endl;
            close(serverSocket);
            exit(-1);
        }
    }

    void start() {
        // iniciando socket listenig, permitindo até 10 conexões
        if (listen(serverSocket, LIMIT) < 0) {
            std::cerr << "Listening erro" << std::endl;
            close(serverSocket);
            exit(-1);
        }

        pthread_t threads[LIMIT];

        for (int i = 0; i < LIMIT; ++i) {
            int* clientSocket = new int; // aloca espaço para cada socket
            *clientSocket = accept(serverSocket, (struct sockaddr*)&address, (socklen_t*)&addrlen);

            if (*clientSocket < 0) {
                std::cerr << "Accept erro " << std::endl;
                close(serverSocket);
                exit(-1);
            }

            if (pthread_create(&threads[i], nullptr, task, (void*)clientSocket) != 0) {
                std::cerr << "Thread create erro " << std::endl;
                delete clientSocket;
                exit(-1);
            }
        }
    }

};

int main() {
    // Configurar locale para exibir caracteres acentuados corretamente
    std::setlocale(LC_ALL, "pt_BR.UTF-8");
    
    Server server;
    server.start(); 
    return 0;
}

#include <iostream>
#include <locale>
#include "semaphore.cpp"
#include "posix.cpp"
#include "posix_aging.cpp"

int main() {
    // Configurar locale para exibir caracteres acentuados corretamente
    std::setlocale(LC_ALL, "pt_BR.UTF-8");
    
    int option = 0;
    bool running = true;

    while (running) {
        // Título e Menu
        std::cout << "\n ######   ##   ##  #######           #####     ####    ##   ##   ####    ##   ##    ####            ######   ##   ##   ####    ####      #####    #####    #####   ######   ##   ##  #######  ######    #####" << std::endl;
        std::cout << " # ## #   ##   ##   ##   #            ## ##     ##     ###  ##    ##     ###  ##   ##  ##            ##  ##  ##   ##    ##      ##      ##   ##  ##   ##  ##   ##   ##  ##  ##   ##   ##   #   ##  ##  ##   ##" << std::endl;
        std::cout << "   ##     ##   ##   ## #              ##  ##    ##     #### ##    ##     #### ##  ##                 ##  ##  ##   ##    ##      ##      ##   ##  #        ##   ##   ##  ##  ##   ##   ## #     ##  ##  #" << std::endl;
        std::cout << "   ##     #######   ####              ##  ##    ##     ## ####    ##     ## ####  ##                 #####   #######    ##      ##      ##   ##   #####   ##   ##   #####   #######   ####     #####    #####" << std::endl;
        std::cout << "   ##     ##   ##   ## #              ##  ##    ##     ##  ###    ##     ##  ###  ##  ###            ##      ##   ##    ##      ##   #  ##   ##       ##  ##   ##   ##      ##   ##   ## #     ## ##        ##" << std::endl;
        std::cout << "   ##     ##   ##   ##   #            ## ##     ##     ##   ##    ##     ##   ##   ##  ##            ##      ##   ##    ##      ##  ##  ##   ##  ##   ##  ##   ##   ##      ##   ##   ##   #   ##  ##  ##   ##" << std::endl;
        std::cout << "  ####    ##   ##  #######           #####     ####    ##   ##   ####    ##   ##    #####           ####     ##   ##   ####    #######   #####    #####    #####   ####     ##   ##  #######  #### ##   #####" << std::endl;
        std::cout << std::endl;

        std::cout << "1. Método por Semáforo" << std::endl;
        std::cout << "2. Método com monitores POSIX" << std::endl;
        std::cout << "3. Método com monitores POSIX e Aging (Anti-Starvation)" << std::endl;
        std::cout << "4. Sair" << std::endl;
        std::cout << "Escolha uma opção: ";
        std::cin >> option;
        std::cout << std::endl;

        // Handle options
        switch (option) {
            case 1:
                // Método por Semáforo
                {
                    SemaphoreDiningTable table;
                    table.run(); // A execução continuará indefinidamente até ser interrompida
                }
                break;
            case 2:
                // Método com monitores POSIX
                {
                    PosixDiningTable table;
                    table.run(); // A execução continuará indefinidamente até ser interrompida
                }
                break;
            
            case 3:
                // Método com monitores POSIX com mecanismo de Aging
                {
                    PosixAgingDiningTable table;
                    table.run(); // A execução continuará indefinidamente até ser interrompida
                }
                break;
            
            case 4:
                std::cout << "Saindo do programa..." << std::endl;
                running = false;
                break;

            default:
                std::cout << "Opção inválida. Por favor, selecione uma opção entre 1 e 4." << std::endl;
                option = 0;
                break;
        }
    }
    return 0;
}

/**
 * @file dining_table.h
 * @brief Interface para a mesa do problema dos Filósofos Jantantes
 */

#ifndef DINING_TABLE_H
#define DINING_TABLE_H

#include <vector>
#include <mutex>
#include <memory>
#include <iostream>
#include "philosophers.h"

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <format>

/**
 * @brief Interface para a mesa no problema dos Filósofos Jantantes
 */
class DiningTable {
public:
    /**
     * @brief Construtor para a mesa
     * @param numPhilosophers O número de filósofos na mesa (padrão: 5)
     */
    DiningTable(int numPhilosophers, int socketnum);

    /**
     * @brief Destrutor virtual
     */
    virtual ~DiningTable() = default;

    /**
     * @brief Executa a simulação dos filósofos
     * Este método deve ser implementado pelas classes derivadas
     */
    virtual void run() = 0;

    /**
     * @brief Retorna o número de filósofos na mesa
     * @return O número de filósofos
     */
    size_t getNumPhilosophers() const {
        return philosophers.size();
    }

protected:
    int socketID;

    std::vector<std::unique_ptr<Philosopher>> philosophers; ///< Vetor de filósofos
    std::vector<std::unique_ptr<std::mutex>> chopsticks;    ///< Vetor de mutex para os palitos
};

// Implementação do construtor
inline DiningTable::DiningTable(int numPhilosophers, int socketnum) : socketID(socketnum) {
    std::string msg = std::format("DiningTable: Inicializando com {} filósofos\n ", numPhilosophers); 
    send(socketID, msg.c_str(), msg.size(), 0);
    
    // Inicializa o vetor de filósofos
    philosophers.clear();
    for (int i = 0; i < numPhilosophers; i++) {
        philosophers.push_back(std::make_unique<Philosopher>(i, socketID));
    }
    
    // Inicializa o vetor de palitos (mutex)
    chopsticks.clear();
    for (int i = 0; i < numPhilosophers; i++) {
        chopsticks.push_back(std::make_unique<std::mutex>());
    }
    
    msg = std::format("DiningTable: Criados {} filósofos e {} palitos", philosophers.size(), chopsticks.size());
    send(socketID, msg.c_str(), msg.size(), 0);
}

#endif // DINING_TABLE_H 
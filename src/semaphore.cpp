#include "../include/dining_table.h"
#include <iostream>
#include <thread>
#include <vector>
#include <semaphore>
#include <functional>
#include <atomic>

/**
 * @brief Implementação da mesa de jantar usando semáforos, permitindo deadlocks
 */
class SemaphoreDiningTable : public DiningTable {
public:
    /**
     * @brief Construtor para a mesa com semáforos
     * @param numPhilosophers O número de filósofos na mesa (padrão: 5)
     */
    SemaphoreDiningTable(int numPhilosophers, int socketnum)
        : DiningTable(numPhilosophers, socketnum) {
        // Inicializa os semáforos para os palitos (todos disponíveis inicialmente)
        chopstickSemaphores.clear(); // Garante que o vetor esteja vazio
        for (int i = 0; i < numPhilosophers; i++) {
            chopstickSemaphores.push_back(std::make_unique<std::binary_semaphore>(1));
        }
    }

    /**
     * @brief Executa a simulação dos filósofos usando semáforos
     */
    void run() override {
        std::string msg = std::format("Iniciando simulação com {} filósofos.\n", philosophers.size());
        msg = msg + "Esta implementação permite deadlocks.\n\n";

        send(socketID, msg.c_str(), msg.size(), 0);
        
        // Cria uma thread para cada filósofo
        std::vector<std::thread> threads;
        for (size_t i = 0; i < philosophers.size(); i++) {
            threads.emplace_back(&SemaphoreDiningTable::philosopherLifecycle, this, i);
        }
        
        // Aguarda todas as threads terminarem
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        msg = "Simulação finalizada.\n";
        send(socketID, msg.c_str(), msg.size(), 0);
    }

    /**
     * @brief Para a execução da simulação
     */
    void stop() {
        running = false;
    }

private:
    std::atomic<bool> running{true};                      ///< Flag atômica para controlar a execução das threads
    std::vector<std::unique_ptr<std::binary_semaphore>> chopstickSemaphores; ///< Semáforos para os palitos

    /**
     * @brief Ciclo de vida de um filósofo
     * @param philosopherId ID do filósofo
     */
    void philosopherLifecycle(int philosopherId) {
        std::string msg;
        while (running) {
            // Pensar
            philosophers[philosopherId]->think();
            
            // Tentar pegar os palitos (primeiro o esquerdo, depois o direito)
            int leftChopstick = philosopherId;
            int rightChopstick = (philosopherId + 1) % philosophers.size();
            
            // Tenta pegar o palito esquerdo
            msg = std::format ("Filósofo {} tentando pegar o palito esquerdo\n", philosopherId);
            send(socketID, msg.c_str(), msg.size(), 0);

            chopstickSemaphores[leftChopstick]->acquire();
            philosophers[philosopherId]->pickUpLeftChopstick();
            
            // Tenta pegar o palito direito
            msg = std::format ("Filósofo {} tentando pegar o palito direito\n", philosopherId);
            send(socketID, msg.c_str(), msg.size(), 0);

            chopstickSemaphores[rightChopstick]->acquire();
            philosophers[philosopherId]->pickUpRightChopstick();
            
            // Comer
            philosophers[philosopherId]->eat();
            
            // Soltar os palitos
            philosophers[philosopherId]->putDownRightChopstick();
            chopstickSemaphores[rightChopstick]->release();
            
            philosophers[philosopherId]->putDownLeftChopstick();
            chopstickSemaphores[leftChopstick]->release();
        }
    }
};
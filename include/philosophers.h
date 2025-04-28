/**
 * @file philosophers.h
 * @brief Interface para o problema dos Filósofos Jantantes
 */

#ifndef PHILOSOPHERS_H
#define PHILOSOPHERS_H

#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <random>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <format>

/**
 * @brief Estados possíveis de um filósofo
 */
enum class State {
    THINKING,   ///< O filósofo está pensando
    HUNGRY,     ///< O filósofo está com fome (tentando pegar os palitos)
    EATING      ///< O filósofo está comendo
};

/**
 * @brief Interface para um filósofo no problema dos Filósofos Jantantes
 */
class Philosopher {
public:
    /**
     * @brief Construtor para um filósofo
     * @param id O identificador único para o filósofo
     */
    Philosopher(int id, int socketnum);

    /**
     * @brief Destrutor
     */
    ~Philosopher() = default;

    /**
     * @brief Obtém o estado atual do filósofo
     * @return O estado atual do filósofo
     */
    State getState() const;

    /**
     * @brief Define o estado atual do filósofo
     * @param newState O novo estado do filósofo
     */
    void setState(State newState);

    /**
     * @brief Obtém o ID do filósofo
     * @return O ID do filósofo
     */
    int getId() const;

    /**
     * @brief Pega o palito esquerdo
     */
    void pickUpLeftChopstick();

    /**
     * @brief Pega o palito direito
     */
    void pickUpRightChopstick();

    /**
     * @brief Solta o palito esquerdo
     */
    void putDownLeftChopstick();

    /**
     * @brief Solta o palito direito
     */
    void putDownRightChopstick();

    /**
     * @brief Faz o filósofo pensar por um tempo aleatório entre 1 e 3 segundos
     */
    void think();

    /**
     * @brief Faz o filósofo comer por 3 segundos
     */
    void eat();

private:
    int socketID;

    int id;                     ///< ID do filósofo
    State state;     ///< Estado atual do filósofo
    bool leftChopstick;         ///< Indica se o filósofo está segurando o palito esquerdo
    bool rightChopstick;        ///< Indica se o filósofo está segurando o palito direito
    mutable std::mutex stateMutex; ///< Mutex para proteger o acesso ao estado
};

// Implementação dos métodos
inline Philosopher::Philosopher(int id, int socketnum)
    : id(id), socketID(socketnum), state(State::THINKING), leftChopstick(false), rightChopstick(false) {
}

inline State Philosopher::getState() const {
    std::lock_guard<std::mutex> lock(stateMutex);
    return state;
}

inline void Philosopher::setState(State newState) {
    std::lock_guard<std::mutex> lock(stateMutex);
    state = newState;
}

inline int Philosopher::getId() const {
    return id;
}

inline void Philosopher::pickUpLeftChopstick() {
    leftChopstick = true;
    std::string msg = std::format("Filósofo {} pegou o palito esquerdo\n", id);
    send(socketID, msg.c_str(), msg.size(), 0);
}

inline void Philosopher::pickUpRightChopstick() {
    rightChopstick = true;
    std::string msg = std::format("Filósofo {} pegou o palito direito\n", id);
    send(socketID, msg.c_str(), msg.size(), 0);
}

inline void Philosopher::putDownLeftChopstick() {
    leftChopstick = false;
    std::string msg = std::format("Filósofo {} soltou o palito esquerdo\n", id);
    send(socketID, msg.c_str(), msg.size(), 0);
}

inline void Philosopher::putDownRightChopstick() {
    rightChopstick = false;
    std::string msg = std::format("Filósofo {} soltou o palito direito\n", id);
    send(socketID, msg.c_str(), msg.size(), 0);
}

inline void Philosopher::think() {
    // Gera um tempo aleatório entre 1 e 3 segundos
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(1000, 3000); // 1-3 segundos em milissegundos
    int thinkTime = distrib(gen);
    
    std::string msg = std::format("Filósofo {} está pensando\n", id);
    send(socketID, msg.c_str(), msg.size(), 0);
    
    // Dorme pelo tempo gerado
    std::this_thread::sleep_for(std::chrono::milliseconds(thinkTime));
    setState(State::HUNGRY);
}

inline void Philosopher::eat() {
    setState(State::EATING);

    std::string msg = std::format("Filósofo {} está comendo\n", id);
    send(socketID, msg.c_str(), msg.size(), 0);
    
    // Come por exatamente 3 segundos
    std::this_thread::sleep_for(std::chrono::seconds(3));
    setState(State::THINKING);
}

#endif // PHILOSOPHERS_H 
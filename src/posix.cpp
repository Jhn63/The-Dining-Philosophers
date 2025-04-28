#include "../include/dining_table.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <atomic>

/**
 * @brief Implementação da mesa de jantar usando monitores POSIX
 */
class PosixDiningTable : public DiningTable {
public:
    /**
     * @brief Construtor para a mesa com monitores POSIX
     * @param numPhilosophers O número de filósofos na mesa (padrão: 5)
     */
    PosixDiningTable(int numPhilosophers = 5)
        : DiningTable(numPhilosophers) {
        // Inicializa o mutex e as variáveis de condição
        pthread_mutex_init(&mutex, NULL);
        
        // Inicializa as variáveis de condição para cada filósofo
        cond.resize(numPhilosophers);
        for (int i = 0; i < numPhilosophers; i++) {
            pthread_cond_init(&cond[i], NULL);
        }
    }
    
    /**
     * @brief Destrutor para liberar recursos
     */
    ~PosixDiningTable() {
        pthread_mutex_destroy(&mutex);
        for (size_t i = 0; i < cond.size(); i++) {
            pthread_cond_destroy(&cond[i]);
        }
    }

    /**
     * @brief Executa a simulação dos filósofos
     */
    void run() override {
        std::cout << "Iniciando simulação com " << philosophers.size() << " filósofos.\n";
        std::cout << "Implementação usando monitores POSIX.\n" << std::endl;
        std::cout << "Esta implementação permite starvation." << std::endl;

        // Armazena threads em um vetor
        std::vector<pthread_t> threads(philosophers.size());
        
        // Cria uma thread para cada filósofo
        for (size_t i = 0; i < philosophers.size(); i++) {
            pthread_create(&threads[i], NULL, 
                           &PosixDiningTable::philosopherLifecycleStatic, 
                           new ThreadData{this, static_cast<int>(i)});
        }
        
        // Aguarda todas as threads terminarem
        for (auto& thread : threads) {
            pthread_join(thread, NULL);
        }
        
        std::cout << "Simulação finalizada.\n";
    }

    /**
     * @brief Para a execução da simulação
     */
    void stop() {
        running = false;
    }

private:
    std::atomic<bool> running{true};  ///< Flag atômica para controlar a execução das threads
    pthread_mutex_t mutex;            ///< Mutex POSIX para controlar acesso aos recursos
    std::vector<pthread_cond_t> cond; ///< Variáveis de condição para cada filósofo
    
    /**
     * @brief Estrutura para passar dados para as threads
     */
    struct ThreadData {
        PosixDiningTable* table;
        int id;
    };
    
    /**
     * @brief Função estática para iniciar o ciclo de vida do filósofo
     * @param arg Dados da thread (ponteiro para ThreadData)
     * @return void* Retorno não usado
     */
    static void* philosopherLifecycleStatic(void* arg) {
        ThreadData* data = static_cast<ThreadData*>(arg);
        data->table->philosopherLifecycle(data->id);
        delete data;
        return NULL;
    }
    
    /**
     * @brief Verifica se o filósofo pode comer
     * @param philosopher_number ID do filósofo
     * @return true se o filósofo pode comer, false caso contrário
     */
    bool canEat(int philosopher_number) {
        int left = (philosopher_number + philosophers.size() - 1) % philosophers.size();
        int right = (philosopher_number + 1) % philosophers.size();
        
        return (philosophers[left]->getState() != State::EATING && 
                philosophers[right]->getState() != State::EATING);
    }
    
    /**
     * @brief Tenta fazer o filósofo comer se possível
     * @param philosopher_number ID do filósofo
     */
    void test(int philosopher_number) {
        if (philosophers[philosopher_number]->getState() == State::HUNGRY && canEat(philosopher_number)) {
            // Filósofo pode comer
            philosophers[philosopher_number]->setState(State::EATING);
            
            // Sinaliza que o filósofo pode comer
            pthread_cond_signal(&cond[philosopher_number]);
        }
    }
    
    /**
     * @brief Implementação da interface do jantar dos filósofos utilizando POSIX
     * Função pickup_forks - cada filósofo chama quando quer comer
     * @param philosopher_number O número do filósofo (0 a n-1)
     */
    void pickup_forks(int philosopher_number) {
        pthread_mutex_lock(&mutex);
        
        // Tenta pegar os palitos
        test(philosopher_number);
        
        // Se não conseguiu comer, espera até que possa
        while (philosophers[philosopher_number]->getState() == State::HUNGRY) {
            std::cout << "Filósofo " << philosopher_number << " está esperando para comer\n";
            pthread_cond_wait(&cond[philosopher_number], &mutex);
        }
        
        // Pega os palitos
        philosophers[philosopher_number]->pickUpLeftChopstick();
        philosophers[philosopher_number]->pickUpRightChopstick();
        
        pthread_mutex_unlock(&mutex);
    }
    
    /**
     * @brief Implementação da interface do jantar dos filósofos utilizando POSIX
     * Função return_forks - cada filósofo chama quando termina de comer
     * @param philosopher_number O número do filósofo (0 a n-1)
     */
    void return_forks(int philosopher_number) {
        pthread_mutex_lock(&mutex);
        
        // Filósofo volta a pensar
        philosophers[philosopher_number]->setState(State::THINKING);
        
        // Solta os palitos
        philosophers[philosopher_number]->putDownLeftChopstick();
        philosophers[philosopher_number]->putDownRightChopstick();
        
        // Verifica se os filósofos vizinhos podem comer
        int left = (philosopher_number + philosophers.size() - 1) % philosophers.size();
        int right = (philosopher_number + 1) % philosophers.size();
        
        test(left);
        test(right);
        
        pthread_mutex_unlock(&mutex);
    }
    
    /**
     * @brief Ciclo de vida de um filósofo
     * @param philosopherId ID do filósofo
     */
    void philosopherLifecycle(int philosopherId) {
        while (running) {
            // Pensar
            philosophers[philosopherId]->think();
            
            // Pegar os palitos
            pickup_forks(philosopherId);
            
            // Comer
            philosophers[philosopherId]->eat();
            
            // Soltar os palitos
            return_forks(philosopherId);
        }
    }
};
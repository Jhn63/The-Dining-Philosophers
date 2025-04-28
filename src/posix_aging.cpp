#include "../include/dining_table.h"
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <vector>
#include <atomic>
#include <queue>
#include <algorithm>
#include <chrono>

/**
 * @brief Implementação da mesa de jantar usando monitores POSIX com mecanismo de aging para evitar starvation
 */
class PosixAgingDiningTable : public DiningTable {
public:
    /**
     * @brief Construtor para a mesa com monitores POSIX e aging
     * @param numPhilosophers O número de filósofos na mesa (padrão: 5)
     */
    PosixAgingDiningTable(int numPhilosophers, int socketnum)
        : DiningTable(numPhilosophers, socketnum) {
        // Inicializa o mutex e as variáveis de condição
        pthread_mutex_init(&mutex, NULL);
        
        // Inicializa as variáveis de condição para cada filósofo
        cond.resize(numPhilosophers);
        for (int i = 0; i < numPhilosophers; i++) {
            pthread_cond_init(&cond[i], NULL);
        }
        
        // Inicializa contadores de espera para cada filósofo
        waitingTime.resize(numPhilosophers, 0);
        
        // Inicializa timestamps da última vez que cada filósofo comeu
        lastEatTime.resize(numPhilosophers);
        for (int i = 0; i < numPhilosophers; i++) {
            lastEatTime[i] = std::chrono::steady_clock::now();
        }
        
        // Define o limiar de starvation (em milissegundos)
        starvationThreshold = 10000; // 10 segundos
    }
    
    /**
     * @brief Destrutor para liberar recursos
     */
    ~PosixAgingDiningTable() {
        pthread_mutex_destroy(&mutex);
        for (size_t i = 0; i < cond.size(); i++) {
            pthread_cond_destroy(&cond[i]);
        }
    }

    /**
     * @brief Executa a simulação dos filósofos
     */
    void run() override {
        std::string msg = std::format("Iniciando simulação com {} filósofos.\n", philosophers.size());
        msg = msg + "Implementação usando monitores POSIX com mecanismo de aging.\n";
        msg = msg + "Esta implementação previne starvation.\n";
        send(socketID, msg.c_str(), msg.size(), 0);

        // Armazena threads em um vetor
        std::vector<pthread_t> threads(philosophers.size());
        
        // Cria uma thread para cada filósofo
        for (size_t i = 0; i < philosophers.size(); i++) {
            pthread_create(&threads[i], NULL, 
                           &PosixAgingDiningTable::philosopherLifecycleStatic, 
                           new ThreadData{this, static_cast<int>(i)});
        }
        
        // Aguarda todas as threads terminarem
        for (auto& thread : threads) {
            pthread_join(thread, NULL);
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
    std::atomic<bool> running{true};      ///< Flag atômica para controlar a execução das threads
    pthread_mutex_t mutex;                ///< Mutex POSIX para controlar acesso aos recursos
    std::vector<pthread_cond_t> cond;     ///< Variáveis de condição para cada filósofo
    std::vector<int> waitingTime;         ///< Contador de espera para cada filósofo
    std::vector<std::chrono::steady_clock::time_point> lastEatTime;  ///< Timestamp da última vez que cada filósofo comeu
    int starvationThreshold;              ///< Limiar de tempo para considerar starvation (ms)
    
    /**
     * @brief Estrutura para passar dados para as threads
     */
    struct ThreadData {
        PosixAgingDiningTable* table;
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
     * @brief Encontra o filósofo faminto com maior tempo de espera entre os que podem comer
     * @return ID do filósofo selecionado ou -1 se nenhum for elegível
     */
    int selectHungryPhilosopher() {
        int selectedPhilosopher = -1;
        int maxWaitingTime = -1;
        auto now = std::chrono::steady_clock::now();
        
        for (size_t i = 0; i < philosophers.size(); i++) {
            if (philosophers[i]->getState() == State::HUNGRY && canEat(i)) {
                // Calcula o tempo de espera desde a última refeição
                auto waitTime = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - lastEatTime[i]).count();
                
                // Calcula prioridade baseada no tempo de espera e no contador de aging
                int priority = waitingTime[i] + (waitTime > starvationThreshold ? 1000 : 0);
                
                // Seleciona o filósofo com maior prioridade
                if (priority > maxWaitingTime) {
                    maxWaitingTime = priority;
                    selectedPhilosopher = i;
                }
            }
        }
        
        return selectedPhilosopher;
    }
    
    /**
     * @brief Tenta encontrar o próximo filósofo que deve comer, com base no mecanismo de aging
     */
    void testWithAging() {
        int selectedPhilosopher = selectHungryPhilosopher();
        
        if (selectedPhilosopher >= 0) {
            // Reseta o contador de espera para este filósofo
            waitingTime[selectedPhilosopher] = 0;
            
            // Atualiza o estado do filósofo para EATING
            philosophers[selectedPhilosopher]->setState(State::EATING);
            
            // Sinaliza que o filósofo pode comer
            pthread_cond_signal(&cond[selectedPhilosopher]);
        }
    }
    
    /**
     * @brief Implementação da interface do jantar dos filósofos utilizando POSIX com aging
     * Função pickup_forks - cada filósofo chama quando quer comer
     * @param philosopher_number O número do filósofo (0 a n-1)
     */
    void pickup_forks(int philosopher_number) {
        pthread_mutex_lock(&mutex);
        
        // Define o estado como faminto
        philosophers[philosopher_number]->setState(State::HUNGRY);
        
        // Tenta encontrar um filósofo para comer com base no aging
        testWithAging();
        
        // Se não conseguiu comer, espera até que possa
        std::string msg;

        while (philosophers[philosopher_number]->getState() == State::HUNGRY) {
            // Incrementa o contador de espera a cada tentativa frustrada
            waitingTime[philosopher_number]++;

            msg = std::format("Filósofo {} aguardando (tempo de espera: {})\n", 
                        philosopher_number,
                        waitingTime[philosopher_number]);
            send(socketID, msg.c_str(), msg.size(), 0);

            // Espera ser sinalizado
            pthread_cond_wait(&cond[philosopher_number], &mutex);
        }
        
        // Pega os palitos
        philosophers[philosopher_number]->pickUpLeftChopstick();
        philosophers[philosopher_number]->pickUpRightChopstick();
        
        // Atualiza o timestamp da última refeição
        lastEatTime[philosopher_number] = std::chrono::steady_clock::now();
        
        pthread_mutex_unlock(&mutex);
    }
    
    /**
     * @brief Implementação da interface do jantar dos filósofos utilizando POSIX com aging
     * Função return_forks - cada filósofo chama quando termina de comer
     * @param philosopher_number O número do filósofo (0 a n-1)
     */
    void return_forks(int philosopher_number) {
        pthread_mutex_lock(&mutex);
        // Solta os palitos
        philosophers[philosopher_number]->putDownLeftChopstick();
        philosophers[philosopher_number]->putDownRightChopstick();
        
        // Tenta encontrar o próximo filósofo para comer com base no aging
        testWithAging();
        
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
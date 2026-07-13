#include <iostream>       // Biblioteca para entrada/saída (std::cout)
#include <thread>         // Biblioteca para criação e gerenciamento de threads (std::thread)
#include <semaphore>      // Biblioteca para semáforos (std::counting_semaphore)
#include <mutex>          // Biblioteca para exclusão mútua (std::mutex, std::lock_guard)
#include <deque>          // Biblioteca para o container deque (usado como buffer)
#include <chrono>         // Biblioteca para trabalhar com durações de tempo (milissegundos)
#include <random>         // Biblioteca para geração de números aleatórios

constexpr int TAM_BUFFER = 5;   // Define o tamanho máximo do buffer compartilhado (constante em tempo de compilação)
constexpr int N_ITENS    = 10;  // Define quantos itens cada thread (produtor/consumidor) vai processar

std::deque<int> buffer;   // Buffer compartilhado entre produtor e consumidor, implementado como fila dupla

// Semáforo que conta quantos "espaços vazios" existem no buffer.
// Começa cheio (TAM_BUFFER), pois inicialmente o buffer está totalmente vazio (todos os espaços disponíveis)
std::counting_semaphore<TAM_BUFFER> vazios(TAM_BUFFER);

// Semáforo que conta quantos "itens prontos" existem no buffer.
// Começa em 0, pois no início não há nenhum item produzido ainda
std::counting_semaphore<TAM_BUFFER> cheios(0);

std::mutex mtx;   // Mutex usado para proteger o acesso concorrente ao buffer (evitar condição de corrida)

// Função executada pela thread produtora
void produtor(int n_itens) {
    std::mt19937 rng(std::random_device{}());              // Gerador de números aleatórios, semente vinda de fonte de entropia do sistema
    std::uniform_int_distribution<int> valor(1, 100);       // Distribuição uniforme para gerar o "item" produzido (valores de 1 a 100)
    std::uniform_int_distribution<int> tempo_ms(100, 300);  // Distribuição uniforme para o tempo de espera simulado entre produções (100 a 300 ms)

    for (int i = 0; i < n_itens; ++i) {   // Loop que se repete n_itens vezes (uma vez para cada item a ser produzido)
        int item = valor(rng);            // Gera um valor aleatório que representará o item produzido

        vazios.acquire(); // Decrementa o semáforo "vazios"; se não houver espaço livre (contador em 0), a thread bloqueia até haver espaço
        {
            std::lock_guard<std::mutex> lock(mtx); // Trava o mutex automaticamente ao entrar no escopo (RAII), garantindo acesso exclusivo ao buffer
            buffer.push_back(item);                // Insere o item produzido no final do buffer
            std::cout << "[Produtor] produziu " << item
                      << " | buffer_size=" << buffer.size() << "\n"; // Exibe no console o item produzido e o tamanho atual do buffer
        } // Fim do escopo: o lock_guard destrói o objeto e libera o mutex automaticamente aqui

        cheios.release();  // Incrementa o semáforo "cheios", sinalizando que há um novo item disponível para o consumidor

        std::this_thread::sleep_for(std::chrono::milliseconds(tempo_ms(rng))); // Pausa a thread por um tempo aleatório, simulando o "tempo de produção"
    }
}

// Função executada pela thread consumidora
void consumidor(int n_itens) {
    std::mt19937 rng(std::random_device{}() + 1);           // Gerador de números aleatórios com semente diferente do produtor (soma 1 para evitar mesma seed)
    std::uniform_int_distribution<int> tempo_ms(200, 500);  // Distribuição uniforme para o tempo de espera simulado entre consumos (200 a 500 ms)

    for (int i = 0; i < n_itens; ++i) {   // Loop que se repete n_itens vezes (uma vez para cada item a ser consumido)
        cheios.acquire();  // Decrementa o semáforo "cheios"; se não houver item disponível (contador em 0), a thread bloqueia até haver um item
        int item;          // Variável para armazenar o item retirado do buffer
        {
            std::lock_guard<std::mutex> lock(mtx); // Trava o mutex automaticamente ao entrar no escopo, garantindo acesso exclusivo ao buffer
            item = buffer.front();                 // Lê o item que está no início do buffer
            buffer.pop_front();                    // Remove esse item do início do buffer
            std::cout << "[Consumidor] consumiu " << item
                      << " | buffer_size=" << buffer.size() << "\n"; // Exibe no console o item consumido e o tamanho atual do buffer
        } // Fim do escopo: o mutex é liberado automaticamente aqui

        vazios.release();  // Incrementa o semáforo "vazios", sinalizando que um espaço do buffer foi liberado

        std::this_thread::sleep_for(std::chrono::milliseconds(tempo_ms(rng))); // Pausa a thread por um tempo aleatório, simulando o "tempo de consumo"
    }
}

int main() {
    std::cout << "=== Produtor/Consumidor iniciado ===\n\n"; // Mensagem indicando o início da execução do programa

    std::thread t_prod(produtor, N_ITENS); // Cria e inicia a thread produtora, passando N_ITENS como argumento da função
    std::thread t_cons(consumidor, N_ITENS); // Cria e inicia a thread consumidora, passando N_ITENS como argumento da função

    t_prod.join(); // Bloqueia a thread principal até que a thread produtora termine sua execução
    t_cons.join(); // Bloqueia a thread principal até que a thread consumidora termine sua execução

    std::cout << "\n=== Produtor/Consumidor encerrado ===\n"; // Mensagem indicando o fim da execução do programa
    return 0; // Encerra o programa retornando 0 (sucesso)
}
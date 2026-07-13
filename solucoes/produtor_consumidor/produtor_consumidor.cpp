#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <deque>
#include <chrono>
#include <random>

constexpr int TAM_BUFFER = 5;
constexpr int N_ITENS    = 10;

std::deque<int> buffer;

std::counting_semaphore<TAM_BUFFER> vazios(TAM_BUFFER);
std::counting_semaphore<TAM_BUFFER> cheios(0);
std::mutex mtx;

void produtor(int n_itens) {
    std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> valor(1, 100);
    std::uniform_int_distribution<int> tempo_ms(100, 300);

    for (int i = 0; i < n_itens; ++i) {
        int item = valor(rng);

        vazios.acquire(); // consome 1, quando for 0 espera                
        {
            std::lock_guard<std::mutex> lock(mtx); // Garante segurança para ninguem mais alterar o buffer
            buffer.push_back(item);
            std::cout << "[Produtor] produziu " << item
                      << " | buffer_size=" << buffer.size() << "\n";
        }
        cheios.release();  // liberar 1                

        std::this_thread::sleep_for(std::chrono::milliseconds(tempo_ms(rng)));
    }
}

void consumidor(int n_itens) {
    std::mt19937 rng(std::random_device{}() + 1);
    std::uniform_int_distribution<int> tempo_ms(200, 500);

    for (int i = 0; i < n_itens; ++i) {
        cheios.acquire();                 
        int item;
        {
            std::lock_guard<std::mutex> lock(mtx);
            item = buffer.front();
            buffer.pop_front();
            std::cout << "[Consumidor] consumiu " << item
                      << " | buffer_size=" << buffer.size() << "\n";
        }
        vazios.release();                 

        std::this_thread::sleep_for(std::chrono::milliseconds(tempo_ms(rng)));
    }
}

int main() {
    std::cout << "=== Produtor/Consumidor iniciado ===\n\n";

    std::thread t_prod(produtor, N_ITENS);
    std::thread t_cons(consumidor, N_ITENS);

    t_prod.join();
    t_cons.join();

    std::cout << "\n=== Produtor/Consumidor encerrado ===\n";
    return 0;
}
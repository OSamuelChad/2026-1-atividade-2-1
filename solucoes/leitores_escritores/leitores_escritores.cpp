#include <iostream>
#include <thread>
#include <mutex>
#include <shared_mutex>
#include <vector>
#include <chrono>
#include <random>

int dado_compartilhado = 0;
std::shared_mutex rw_mutex;

void leitor(int id, int n_leituras) {
    std::mt19937 rng(std::random_device{}() + id);
    std::uniform_int_distribution<int> tempo_ms(100, 300);

    for (int i = 0; i < n_leituras; ++i) {
        {
            std::shared_lock<std::shared_mutex> lock(rw_mutex); 
            std::cout << "[Leitor " << id << "] leu valor = "
                      << dado_compartilhado << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(tempo_ms(rng)));
    }
}

void escritor(int id, int n_escritas) {
    std::mt19937 rng(std::random_device{}() + id * 100);
    std::uniform_int_distribution<int> tempo_ms(300, 600);

    for (int i = 0; i < n_escritas; ++i) {
        {
            std::unique_lock<std::shared_mutex> lock(rw_mutex); 
            dado_compartilhado++;
            std::cout << "[Escritor " << id << "] escreveu valor = "
                      << dado_compartilhado << "\n";
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(tempo_ms(rng)));
    }
}

int main() {
    std::cout << "=== Leitores/Escritores iniciado ===\n\n";

    std::vector<std::thread> threads;

    for (int i = 1; i <= 4; ++i)
        threads.emplace_back(leitor, i, 3);
    for (int i = 1; i <= 2; ++i)
        threads.emplace_back(escritor, i, 3);

    for (auto& t : threads) t.join();

    std::cout << "\n=== Leitores/Escritores encerrado. Valor final = "
              << dado_compartilhado << " ===\n";
    return 0;
}
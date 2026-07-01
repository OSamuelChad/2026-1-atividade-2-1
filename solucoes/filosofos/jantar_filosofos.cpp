#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <array>
#include <chrono>
#include <random>

constexpr int N = 5;

std::array<std::mutex, N> garfos;
std::counting_semaphore<N - 1> mesa(N - 1);

void pensar(int id, std::mt19937& rng) {
    std::uniform_real_distribution<double> dur(1.0, 3.0);
    double d = dur(rng);
    std::cout << "Filósofo " << id << " está PENSANDO por " << d << "s\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(d * 1000)));
}

void comer(int id, std::mt19937& rng) {
    std::uniform_real_distribution<double> dur(1.0, 2.0);
    double d = dur(rng);
    std::cout << "Filósofo " << id << " está COMENDO  por " << d << "s\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(d * 1000)));
}

void filosofo(int id, int rodadas) {
    int esquerdo = id;
    int direito  = (id + 1) % N;
    std::mt19937 rng(std::random_device{}() + id);

    for (int r = 1; r <= rodadas; ++r) {
        pensar(id, rng);

        std::cout << "Filósofo " << id << " (rodada " << r
                  << "): quer comer, aguardando garfos...\n";
        mesa.acquire();                       

        garfos[esquerdo].lock();              
        std::cout << "Filósofo " << id << ": pegou garfo esquerdo ("
                  << esquerdo << ")\n";

        garfos[direito].lock();               
        std::cout << "Filósofo " << id << ": pegou garfo direito  ("
                  << direito << ")\n";

        comer(id, rng);

        garfos[direito].unlock();             
        garfos[esquerdo].unlock();            
        mesa.release();                       

        std::cout << "Filósofo " << id << ": devolveu os garfos\n";
    }
}

int main() {
    constexpr int RODADAS = 3;
    std::cout << "=== Jantar dos Filósofos iniciado ===\n\n";

    std::array<std::thread, N> threads;
    for (int i = 0; i < N; ++i)
        threads[i] = std::thread(filosofo, i, RODADAS);

    for (auto& t : threads) t.join();

    std::cout << "\n=== Jantar encerrado ===\n";
    return 0;
}
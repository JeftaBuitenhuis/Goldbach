//#include <immintrin.h> // SIMD AVX intel library
//#include <ia32intrin.h> // SIMD SVML intel library
#include <iostream> // io
#include <thread> // Multi-threading
#include <vector> // Vectors
#include <math.h> // Mathematics functions

/**
 * A class for calculating goldbach.
 * 
 * @author J.J.Buitenhuis
 * @version 0.1
 * @since 0.0
*/
class Goldbach{
    public:
        /**
         * Constructor for the Goldbach class
         * 
         * @param max the maximum value calculated
         * @param threads the amount of CPU threads used for calculations
        */
        Goldbach(int max, int threads){
            THREADS = threads;
            MAX = max;
            CACHE = new bool[max];
            CURRENT_NUM = MAX;
        }
        // FUNCTIONS
        void gen_primes();
        void write_cache();
        void initialize_threads();
    private:
        // VARIABLES
        int MAX;
        int THREADS = 1;
        int NUM = 7;
        //int SECTIONS;
        int CURRENT_NUM = 2;

        bool *CACHE;

        // FUNCTIONS
        void cache_primes();
        bool is_prime(int);
        int next_num();
        void solve_goldbach(int);
        void balance_load();

        // DEBUG
};

//----------------------------------------------------// Goldbach

void Goldbach::solve_goldbach(int num){
    bool stop = false;
    int i = 2;
    while (!stop){
        stop = (CACHE[i] && CACHE[num-i]);
        i++;
    }
    //std::cout << num << ": " << (i-1) << " + " << num-(i-1) << "\n";
} 

void Goldbach::initialize_threads(){
    std::vector<std::thread> thread_vector;

    for (int thread = 0; thread < this->THREADS; thread++){
        thread_vector.emplace_back([&](){balance_load();});
    }

    for(auto& t: thread_vector){
        t.join();
    }
}

// void BalanceLoad()
void Goldbach::balance_load(){
    int n = next_num();

    while (n >= 4){
        solve_goldbach(n);
        n = next_num();
    }
}

int Goldbach::next_num(){
    CURRENT_NUM -= 2;
    return CURRENT_NUM;
}


//----------------------------------------------------// Primes

void Goldbach::gen_primes(){
    CACHE[2] = true;
    CACHE[3] = true;
    CACHE[5] = true;
    CACHE[7] = true;
    std::vector<std::thread> thread_vector;

    for (int thread = 0; thread < THREADS; thread++){
        thread_vector.emplace_back([&](){cache_primes();});
    }
    for(auto& t: thread_vector){
        t.join();
    }
}

void Goldbach::cache_primes(){
    int n = (NUM += 2);
    while (NUM < MAX){
        CACHE[n] = is_prime(n);
        n = NUM += 2;
    }
}

bool Goldbach::is_prime(int n){
    int sqrt_n = std::sqrt(n);
    int i = 5;
    bool mod_z = (n % 3);
    while (mod_z && i <= sqrt_n){
        mod_z = (n % i && n % (i + 2));
        i += 6;
    }
    return mod_z;
}

void Goldbach::write_cache(){
    int amount = 0;
    for (int i = 0; i < MAX; i++){
        if (CACHE[i]) {
            amount++;
        }
        std::cout << i << " = " <<CACHE[i] << "\n";
    }
    std::cout << amount << "\n";
}

//---------------------------------------------------------------// Main

int main(){
    int num = 100000000;
    int amount_of_threads = 8;
    Goldbach goldbach(num, amount_of_threads);
    goldbach.gen_primes();
    goldbach.initialize_threads();
    //goldbach.write_cache();
}
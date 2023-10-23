//#include <immintrin.h> // SIMD AVX intel library
//#include <ia32intrin.h> // SIMD SVML intel library
#include <atomic> // atomic
#include <iostream> // io
#include <thread> // Multi-threading
#include <vector> // Vectors
#include <math.h> // Mathematics functions
#include <tuple> // tuple

/**
 * A class for calculating goldbach.
 * 
 * @author J.J.Buitenhuis
 * @version 0.2
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
            CACHE_P = new bool[max];
            CACHE_G = new int[max];
            CURRENT_NUM = MAX;
        }
        // FUNCTIONS
        void gen_primes();
        void write_cache_p();
        void write_cache_g();
        void initialize_threads();
    private:
        // VARIABLES
        int MAX;
        int THREADS = 1;
        std::atomic_int NUM = 7;
        //int SECTIONS;
        std::atomic_int CURRENT_NUM = 2;
        bool *CACHE_P;
        

        // FUNCTIONS
        void cache_primes();
        bool is_prime(int);
        void solve_goldbach(int);
        void balance_load();

        // OUTPUT
        int *CACHE_G;
};

//----------------------------------------------------// Goldbach

void Goldbach::solve_goldbach(int num){
    int i = 2;
    bool stop = (CACHE_P[i] && CACHE_P[num-i]);
    i--;
    while (!stop){
        i+=2;
        stop = (CACHE_P[i] && CACHE_P[num-i]);
    }
    CACHE_G[num] = num-i;
    CACHE_G[num + 1] = i;
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
    int n = (CURRENT_NUM -= 2);

    while (n >= 4){
        solve_goldbach(n);
        n = (CURRENT_NUM -= 2);
    }
}


//----------------------------------------------------// Primes

void Goldbach::gen_primes(){
    CACHE_P[2] = true;
    CACHE_P[3] = true;
    CACHE_P[5] = true;
    CACHE_P[7] = true;
    std::vector<std::thread> thread_vector;

    for (int thread = 0; thread < THREADS; thread++){
        thread_vector.emplace_back([&](){(cache_primes());});
    }
    for(auto& t: thread_vector){
        t.join();
    }
}

void Goldbach::cache_primes(){
    int n = (NUM += 2);
    while (NUM < MAX){
        CACHE_P[n] = is_prime(n);
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

//----------------------------------------------------------------// Debug

void Goldbach::write_cache_p(){
    int amount = 0;
    for (int i = 0; i < MAX; i++){
        if (CACHE_P[i]) {
            amount++;
        }
        std::cout << i << " = " << CACHE_P[i] << "\n";
    }
    std::cout << amount << "\n";
}

void Goldbach::write_cache_g(){
    int i_one_high = 0;
    int i_two_high = 0;
    for (int i = 4; i < MAX; i += 2){
        std::cout << i << " = " << CACHE_G[i] << " + " << CACHE_G[i+1]  << "\n";
        if (i_one_high < CACHE_G[i]){
            i_one_high = CACHE_G[i];
        }
        if (i_two_high < CACHE_G[i+1]){
            i_two_high = CACHE_G[i+1];
        }
    }
    std::cout << "i_one_h: " << i_one_high << ", i_two_h: " << i_two_high << "\n";
}

//---------------------------------------------------------------// Main

int main(){
    int num = 10000000;
    int amount_of_threads = 8;
    Goldbach goldbach(num, amount_of_threads);
    goldbach.gen_primes();
    goldbach.initialize_threads();
    goldbach.write_cache_g();
    //goldbach.write_cache_p();
}
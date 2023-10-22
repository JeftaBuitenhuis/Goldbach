//#include <immintrin.h> // SIMD AVX intel library
//#include <ia32intrin.h> // SIMD SVML intel library
#include <iostream> // io
#include <thread> // Multi-threading
#include <vector> // Vectors
#include <math.h> // Mathematics functions

/**
 * ... "documentation will come"
 * 
 * @author J.J.Buitenhuis
 * @version 0.0
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
        }
        // FUNCTIONS
        void gen_primes();
        void write_cache();

    private:
        // VARIABLES
        int MAX;
        int THREADS = 1;
        int p = 7;
        //int SECTIONS;
        //int CURRENT_SET = 2;

        std::vector<int> CACHE;

        // FUNCTIONS
        void cache_primes();
        bool prime_v(int);

        // DEBUG
};

//----------------------------------------------------// Goldbach



//----------------------------------------------------// Primes

void Goldbach::gen_primes(){
    CACHE.reserve(MAX);
    CACHE.emplace_back(2);
    CACHE.emplace_back(3);
    CACHE.emplace_back(5);
    CACHE.emplace_back(7);
    std::vector<std::thread> thread_vector;

    for (int thread = 0; thread < THREADS; thread++){
        thread_vector.emplace_back([&](){cache_primes();});
    }
    for(auto& t: thread_vector){
        t.join();
    }
}

void Goldbach::cache_primes(){
    int n = p += 2;
    while (n < MAX){
        if (prime_v(n)){
            CACHE.emplace_back(n);
        }
        n = p += 2;
    }
}

bool Goldbach::prime_v(int n){
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
    int max = CACHE.size();
    for (int i = 0; i < max; i++){
        std::cout << CACHE[i] << "\n";
    }
    std::cout << max << "\n";
}

//---------------------------------------------------------------// Main

int main(){
    int num = 10000000;
    int amount_of_threads = 8;
    Goldbach goldbach(num, amount_of_threads);
    goldbach.gen_primes();
    //goldbach.write_cache();
}
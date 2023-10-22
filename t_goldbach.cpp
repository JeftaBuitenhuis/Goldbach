#include <experimental/simd>
#include <iostream>
#include <thread>
#include <vector>
#include <math.h>

namespace stdx = std::experimental;

using int_vec8 = std::experimental::fixed_size_simd<int,8>;

class Goldbach{
    public:
        Goldbach(int max, int threads){
            THREADS = threads;
            MAX = max;
        }
        // FUNCTIONS
        void gen_primes();
        void cache_primes();
        void WriteCache();
        bool prime_v(int);

    private:
        // VARIABLES
        int MAX;
        int THREADS = 1;
        int SECTIONS;
        int CURRENT_SET = 2;
        int p = 7;

        std::vector<int> CACHE;

        // FUNCTIONS
        int next_p();
        int int_vec8_to_int(int_vec8);

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

void Goldbach::WriteCache(){
    int max = CACHE.size();
    for (int i = 0; i < max; i++){
        std::cout << CACHE[i] << "\n";
    }
    std::cout << max << "\n";
}

//---------------------------------------------------------------// Main

int main(){
    int num = 10000;
    Goldbach goldbach(num, 8);
    goldbach.gen_primes();
    goldbach.WriteCache();
}
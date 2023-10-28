// CUDA runtime
#include <cuda_runtime.h>
//#include <immintrin.h> // SIMD AVX intel library
//#include <ia32intrin.h> // SIMD SVML intel library
#include <atomic> // atomic
#include <iostream> // io
#include <thread> // Multi-threading
#include <vector> // Vectors
#include <math.h> // Mathematics functions
#include <tuple> // tuple

__global__ void atkin_conditions(unsigned long max, unsigned long x, bool *p){
    unsigned long y = threadIdx.x + blockDim.x*blockIdx.x;
    //printf("%d\n", y);

    // Condition 1
    unsigned long n = (4 * x * x) + (y * y);
    if (n <= max && (n % 12 == 1 || n % 12 == 5)){
        p[n] ^= true;
    }

    // Condition 2
    n = (3 * x * x) + (y * y);
    if (n <= max && n % 12 == 7){
        p[n] ^= true;
    }

    // Condition 3
    n = (3 * x * x) - (y * y);
    if (x > y && n <= max && n % 12 == 11){
        p[n] ^= true;
    }
}

class Goldbach{
    public:
        /**
         * Constructor for the Goldbach class
         * 
         * @param max the maximum value calculated
         * @param threads the amount of CPU threads used for calculations
        */
        Goldbach(unsigned long max, int threads, int g_threads){
            THREADS = threads;
            MAX = max;
            CACHE_P = new bool[max + 1];
            CACHE_G = new unsigned long[max];
            CURRENT_NUM = MAX;
            G_THREADS = g_threads;
        }
        // FUNCTIONS
        void gen_primes();
        void write_cache_p();
        void write_cache_g();
        void initialize_threads();
    private:
        // VARIABLES
        unsigned long MAX;
        int THREADS = 1;
        int G_THREADS = 1024;
        //int SECTIONS;
        std::atomic_long CURRENT_NUM = 2;
        //int CURRENT_NUM = 2;
        bool *CACHE_P;
        

        // FUNCTIONS
        void solve_goldbach(unsigned long);
        void balance_load();

        // OUTPUT
        unsigned long *CACHE_G;
};

//----------------------------------------------------// Goldbach

void Goldbach::solve_goldbach(unsigned long num){
    unsigned long i = 2;
    bool stop = (CACHE_P[i] && CACHE_P[num-i]);
    i--;
    while (!stop){
        i+=2;
        //std::cout << i << "\n";
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
    unsigned long n = (CURRENT_NUM -= 2);

    while (n >= 4){
        solve_goldbach(n);
        n = (CURRENT_NUM -= 2);
    }
}


//----------------------------------------------------// Primes

void Goldbach::gen_primes(){
    bool *d_x;//*h_x, *d_x;
    int nblocks = 1, nthreads=G_THREADS;

    //CACHE_P = (bool *) malloc(MAX*sizeof(bool) + sizeof(bool));
    cudaMalloc((void **) &d_x, MAX*sizeof(bool) + sizeof(bool));

    CACHE_P[2] = true;
    CACHE_P[3] = true;

    for (unsigned long i = 0; i*i < MAX; i++){
        atkin_conditions<<<nblocks,nthreads>>>(MAX, i, d_x);
    }

    cudaMemcpy(CACHE_P,d_x,MAX, cudaMemcpyDeviceToHost);


    // Mark all multiples
    // of squares as non-prime
    for (unsigned long r = 5; r * r <= MAX; r++) {
        if (CACHE_P[r]) {
            for (unsigned long i = r * r; i <= MAX; i += r * r)
                CACHE_P[i] = false;
        }
    }
    cudaFree(d_x);
}

//----------------------------------------------------------------// Debug

void Goldbach::write_cache_p(){
    unsigned long amount = 0;
    for (unsigned long i = 0; i < MAX; i++){
        if (CACHE_P[i]) {
            amount++;
        }
        std::cout << i << " = " << CACHE_P[i] << "\n";
    }
    std::cout << amount << "\n";
}

void Goldbach::write_cache_g(){
    unsigned long i_one_high = 0;
    unsigned long i_two_high = 0;
    for (unsigned long i = 4; i < MAX; i += 2){
        //std::cout << i << " = " << CACHE_G[i] << " + " << CACHE_G[i+1]  << "\n";
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

int main(int argc, char* argv[]){
    unsigned long max = std::strtoul(argv[1], NULL, 0);
    int threads = std::stoi(argv[2]);
    int g_threads = std::stoi(argv[3]);
    bool debug = std::stoi(argv[4]);
    if (debug){
        //get_device_data();
    }
    Goldbach goldbach(max, threads, g_threads);
    goldbach.gen_primes();
    goldbach.initialize_threads();
    //goldbach.write_cache_p();
    //goldbach.write_cache_g();
}
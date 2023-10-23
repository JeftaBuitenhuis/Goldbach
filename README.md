# Goldbach
Calculating numbers for the Goldbach conjecture as fast as possible.

Compile know to work:
g++ -fdiagnostics-color=always /../goldbach.cpp -O -std=c++21 -mtune=intel -mavx -ftree-vectorize -funsafe-math-optimizations -mveclibabi=svml -o /../goldbach

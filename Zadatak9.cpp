#include <iostream>
#include <cmath>
#include <vector>
#include <mpi.h>

bool is_prime(int n) { //provjerava je li broj prost
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

int main(int argc, char** argv) {
    MPI::Init(argc, argv);

    int rank = MPI::COMM_WORLD.Get_rank();
    int size = MPI::COMM_WORLD.Get_size();

    const int MAX_NUM = 1000000;
    const int chunk_size = MAX_NUM / 100; //Svaki proces dobiva dio tog raspona

    int start = rank * chunk_size + 1;
    int end = (rank + 1) * chunk_size;

    if (rank == size - 1) {
        end = MAX_NUM;
    }

    std::vector<int> primes;
    for (int i = start; i <= end; ++i) { //provjerava je li broj prost i sprema ga u primes
        if (is_prime(i)) {
            primes.push_back(i);
        }
    }

    int max_gap_local = 0;
    for (size_t i = 1; i < primes.size(); ++i) { //racuna najvecu razliku i sprema ga u varijablu
        int gap = primes[i] - primes[i - 1];
        if (gap > max_gap_local) {
            max_gap_local = gap;
        }
    }

    int max_gap_global;
    MPI::COMM_WORLD.Reduce(&max_gap_local, &max_gap_global, 1, MPI::INT, MPI::MAX, 0);

    if (rank == 0) {
        std::cout << "Najveći razmak između uzastopnih prostih brojeva manjih od "
                  << MAX_NUM << " je: " << max_gap_global << std::endl;
    }

    MPI::Finalize();
    return 0;
}
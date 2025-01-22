#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <mpi.h>

int main(int argc, char *argv[]) {
    double random_number, total_sum, mean, deviation;

    MPI::Init(argc, argv);

    int rank = MPI::COMM_WORLD.Get_rank();
    int size = MPI::COMM_WORLD.Get_size();

    srand(time(0) + rank);
    random_number = rand() % 100 + 1;

    std::cout << "Proces " << rank << " generirao broj: " << random_number << std::endl;

    MPI::COMM_WORLD.Reduce(&random_number, &total_sum, 1, MPI::DOUBLE, MPI::SUM, 0);

    if (rank == 0) {
        mean = total_sum / size;
        std::cout << "Aritmetička srednja vrijednost: " << mean << std::endl;
    }

    MPI::COMM_WORLD.Bcast(&mean, 1, MPI::DOUBLE, 0);

    deviation = (std::abs(random_number - mean) / mean) * 100;

    std::cout << "Proces " << rank << " ima odstupanje od: " << deviation << "%" << std::endl;

    MPI::Finalize();

    return 0;
}
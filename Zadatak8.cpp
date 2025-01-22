#include <mpi.h>
#include <iostream>
#include <vector>
#include <numeric>

int main(int argc, char** argv) {
    MPI::Init(argc, argv);

    int rank = MPI::COMM_WORLD.Get_rank();
    int size = MPI::COMM_WORLD.Get_size();

    const int local_vector_size = 100;
    std::vector<int> local_vector(local_vector_size);

    for (int i = 0; i < local_vector_size; ++i) {
        local_vector[i] = rank * local_vector_size + i;
    }

    std::vector<int> global_vector;
    if (rank == 0) {
        global_vector.resize(local_vector_size * size);
    }

    MPI::COMM_WORLD.Gather( //Gather prikuplja podatke od ostalih procesa i šalje ih glavnom procesu
        local_vector.data(),
        local_vector_size,
        MPI::INT,
        global_vector.data(),
        local_vector_size,
        MPI::INT,
        0
    );

    if (rank == 0) {
        int global_sum = std::accumulate(global_vector.begin(), global_vector.end(), 0);
        double global_average = static_cast<double>(global_sum) / (local_vector_size * size);

        std::cout << "Globalni prosjek: " << global_average << std::endl;
    }

    double global_average = 0.0;
    if (rank == 0) {
        global_average = static_cast<double>(std::accumulate(global_vector.begin(), global_vector.end(), 0)) / (local_vector_size * size);
    }

    MPI::COMM_WORLD.Bcast(&global_average, 1, MPI::DOUBLE, 0);

    std::cout << "Proces " << rank << " primio globalni prosjek: " << global_average << std::endl;

    MPI::Finalize();
    return 0;
}
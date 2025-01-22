#include <iostream>
#include <fstream> //Otvaranje i zatvaranje datoteka
#include <vector>
#include <mpi.h>

int main(int brArg, char* arg[]) {
    MPI::Init(brArg, arg);

    int procesID = MPI::COMM_WORLD.Get_rank();
    int ukBrProcesa = MPI::COMM_WORLD.Get_size();

    std::vector<int> data;
    int data_size = 0;

    if (procesID == 0) {
       
        std::ifstream file("data.txt");
        int br;
        while (file >> br) {
            data.push_back(br);
        }
        file.close();
        data_size = data.size();
    }

    MPI::COMM_WORLD.Bcast(&data_size, 1, MPI_INT, 0);

    std::vector<int> local_data(data_size); //Prima dio podataka za svaki proces

    MPI::COMM_WORLD.Scatter(data.data(), 1, MPI_INT, local_data.data(), 1, MPI_INT, 0);//šalje podatke iz vektora data

    if (procesID < data_size) {
        std::cout << "Proces " << procesID << " ispisuje: " << local_data[0] << std::endl;
    }

    MPI::Finalize();

    return 0;
}
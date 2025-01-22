#include <mpi.h>
#include <iostream>
#include <vector>
#include <cstring> // Funkcije s C stringovima
#include <unistd.h> // Funkcije Unix/Linux susttav

int main(int brArg, char** arg) {
 
    MPI::Init(brArg, arg);

    int procesID = MPI::COMM_WORLD.Get_rank();
    int ukBrProcesa = MPI::COMM_WORLD.Get_size();

    int poruka = 7;
    char hostname[MPI_MAX_PROCESSOR_NAME];
    int nameLength;

    MPI::Get_processor_name(hostname, nameLength);

    if (procesID == 0) {
        
        MPI::COMM_WORLD.Bcast(&poruka, 1, MPI::INT, 0);
        std::cout << "Proces 0 poslao poruku: " << poruka << std::endl;

        
        std::vector<MPI::Request> requests(ukBrProcesa - 1); //Vektor objekata za pohranu zahtjeva za asinkrono  primanje popdataka 
        std::vector<char*> received_hostnames(ukBrProcesa - 1); //Vektor pokazivača na char nizove koji pohranjuje imena rač
        
        for (int i = 1; i < ukBrProcesa; ++i) {
            received_hostnames[i - 1] = new char[MPI_MAX_PROCESSOR_NAME];
            requests[i - 1] = MPI::COMM_WORLD.Irecv(received_hostnames[i - 1], MPI_MAX_PROCESSOR_NAME, MPI::CHAR, i, 0);
        }

        //MPI::Request::Waitall(ukBrProcesa- 1, requests.data()); 

        for (int i = 1; i < ukBrProcesa; ++i) {
            std::cout << "Proces 0 primio poruku od procesa " << i << ": "
                      << received_hostnames[i - 1] << std::endl;
            delete[] received_hostnames[i - 1];
        }
    } else {
    
        MPI::COMM_WORLD.Bcast(&poruka, 1, MPI::INT, 0);
        std::cout << "Proces " << procesID << " primio poruku: " << poruka << std::endl;

        MPI::COMM_WORLD.Send(hostname, nameLength + 1, MPI::CHAR, 0, 0);
    }

    MPI::Finalize();
    return 0;
}
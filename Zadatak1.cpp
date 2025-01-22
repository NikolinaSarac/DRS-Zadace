#include <mpi.h>
#include <iostream>
#include <vector>

#define velicinaVektora 100
int main(int brArg, char** arg){
     MPI::Init(brArg, arg);

    int procesID = MPI::COMM_WORLD.Get_rank();
    int brProcesa = MPI::COMM_WORLD.Get_size();

    
    std::vector<int> vektor(velicinaVektora);

    if( procesID==0){
        for(int i=0;i<velicinaVektora;++i){
            vektor[i]=i;
        }

        for(int i=1;i<brProcesa;++i){
            MPI::COMM_WORLD.Send(vektor.data(),velicinaVektora,MPI_INT,i,0);
        }

        for(int i=1;i<brProcesa;++i){
           MPI::COMM_WORLD.Recv(vektor.data(),velicinaVektora,MPI_INT,i,0);

            std::cout <<"Primljen izmjenjeni vektor od procesa " << i << ": ";
            for(int j=0;j<10;++j){
                std::cout <<vektor[j]  << " ";
            }
            std::cout << "...\n";
        }
    }else{
        MPI::COMM_WORLD.Recv(vektor.data(),velicinaVektora,MPI_INT,0,0);
        for(int& elem : vektor){
            elem+=(brProcesa + 1);
        }
        MPI::COMM_WORLD.Send(vektor.data(),velicinaVektora,MPI_INT,0,0);
    }

    MPI::Finalize();
    return 0;
}
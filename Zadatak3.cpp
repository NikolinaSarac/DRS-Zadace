#include <iostream>
#include <cstdlib> //Funkcije za upravljanje mem. i funkcija rand()
#include <ctime> //Funkcija za upravljanje i pristup vremenu
#include <mpi.h>


void ispis(int* matrica, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            std::cout << matrica[i * cols + j] << " ";
        }
        std::cout << std::endl;
    }
}

void randomMatrica(int* matrica, int rows, int cols) {
    srand(time(0)); //postavlja seed za funkc. da se elementi razlikuju i vraća vrijeme u sekundama
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrica[i * cols + j] = rand() % 10; 
        }
    }
}

void donjaMatrica(int* matrica, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = i + 1; j < cols; ++j) {
            matrica[i * cols + j] = 0; 
        }
    }
}

int main(int brArg, char** arg) {
    
    const int rows = 5, cols = 5; 
    int* matrica = new int[rows * cols]; 
    
    MPI::Init(brArg, arg);

    int procesID = MPI::COMM_WORLD.Get_rank();
    int brProcesa = MPI::COMM_WORLD.Get_size();
    
    if (procesID == 0) {

        randomMatrica(matrica, rows, cols);
        std::cout << "Originalna matrica: \n";
        ispis(matrica, rows, cols);

        donjaMatrica(matrica, rows, cols);
    }

    int row[cols];
    MPI::COMM_WORLD.Scatter(matrica, cols, MPI_INT, row, cols, MPI_INT, 0); //Raspodjela podataka s root procesa
    
    std::cout << "Proces " << procesID << " ispisuje redak: ";
    for (int i = 0; i < cols; ++i) {
        std::cout << row[i] << " ";
    }
    std::cout << std::endl;

    MPI::Finalize();
    delete[] matrica;
    return 0;
}
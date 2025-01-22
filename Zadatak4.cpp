#include <mpi.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <cstdlib>
#include <ctime>

#define THINKING 0
#define HUNGRY 1
#define EATING 2

const int N = 5;

void print_states(const std::vector<int> &states) {
    std::cout << "Stanje filozofa: ";
    for (int state : states) {
        if (state == EATING)
            std::cout << "X ";
        else if (state == THINKING)
            std::cout << "O ";
        else if (state == HUNGRY)
            std::cout << "o ";
    }
    std::cout << std::endl;
}

void philosopher(int rank) {
    int left = rank; //lijeva vilica
    int right = (rank + 1) % N; //desna vilica
    srand(time(nullptr) + rank);//osigurava različite slučajne brojeve

    while (true) {
        std::cout << "Filozof " << rank << " razmišlja.\n";
        sleep(rand() % 3 + 1); //pauza u programu     

        std::cout << "Filozof " << rank << " je gladan.\n";
        MPI::COMM_WORLD.Send(nullptr, 0, MPI::INT, 0, HUNGRY);

        MPI::COMM_WORLD.Recv(nullptr, 0, MPI::INT, 0, EATING);

        std::cout << "Filozof " << rank << " jede.\n";
        sleep(rand() % 3 + 1); 

        MPI::COMM_WORLD.Send(nullptr, 0, MPI::INT, 0, THINKING);
    }
}

void waiter() {
    std::vector<int> states(N, THINKING); 
    std::vector<bool> chopsticks(N, true); 

    while (true) {
        MPI::Status status; //čuva dodatne informacije o porukama
        MPI::COMM_WORLD.Recv(nullptr, 0, MPI::INT, MPI::ANY_SOURCE, MPI::ANY_TAG, status);

        int philosopher = status.Get_source(); 
        int tag = status.Get_tag();          

        if (tag == HUNGRY) {
            int left = philosopher;
            int right = (philosopher + 1) % N;

            if (chopsticks[left] && chopsticks[right]) {
                chopsticks[left] = chopsticks[right] = false;
                states[philosopher] = EATING;
                MPI::COMM_WORLD.Send(nullptr, 0, MPI::INT, philosopher, EATING);
            } else {
                states[philosopher] = HUNGRY;
            }
        } else if (tag == THINKING) {
            int left = philosopher;
            int right = (philosopher + 1) % N;

            chopsticks[left] = chopsticks[right] = true;
            states[philosopher] = THINKING;
        }

        print_states(states);
    }
}

int main(int argc, char **argv) {
    MPI::Init(argc, argv);

    int rank = MPI::COMM_WORLD.Get_rank(); 
    int size = MPI::COMM_WORLD.Get_size();

    if (size != N + 1) {
        if (rank == 0) {
            std::cerr << "Potrebno je pokrenuti " << N + 1 << " procesa (1 konobar + " << N << " filozofa).\n";
        }
        MPI::Finalize();
        return EXIT_FAILURE;
    }

    if (rank == 0) {
        waiter();
    } else {
        philosopher(rank - 1);
    }

    MPI::Finalize();
    return EXIT_SUCCESS;
}
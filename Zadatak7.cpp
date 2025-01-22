#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <ctime>

using namespace std;

const int BOARD_SIZE = 4;
const int BROJ_FIGURA = 4;

struct Figura { //struktura koja predstavlja figuru na ploči
    int id, x, y, tip, brojPojedenih = 0, brojPoteza = 0;
    bool ziv = true;
};

struct polje {
    int id = -1, tip = -1;
};

polje ploca[BOARD_SIZE][BOARD_SIZE];
Figura figura[BROJ_FIGURA];

void postaviFiguru(int rank, int id, int x, int y, int tip) {
    figura[id] = {id, x, y, tip, 0, 0, true}; 
    ploca[x][y] = {id, tip};

    for (int i = 0; i < BOARD_SIZE; i++) {
        if (i != rank) {
            MPI::COMM_WORLD.Send(&figura[id], sizeof(figura[id]), MPI::BYTE, i, 1);
            MPI::COMM_WORLD.Recv(&figura[i], sizeof(figura[i]), MPI::BYTE, i, 1);
        }
    }
}

void initializeBoard(int rank) {
    if (rank == 0) postaviFiguru(rank, 0, 1, 1, 0); // pijun
    else if (rank == 1) postaviFiguru(rank, 1, 2, 2, 1); // lovac
    else if (rank == 2) postaviFiguru(rank, 2, 3, 0, 2); // top
    else if (rank == 3) postaviFiguru(rank, 3, 3, 3, 2); // top
    MPI::COMM_WORLD.Barrier(); //blokira program dok se svi procesi ne završe
}

bool pomicanje(int i, int x, int y) { //provjerava je li nova pozicija unutar granica ploče
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) return false;

    auto &f = figura[i]; //figura koja se pomiče
    auto &p = ploca[x][y];//polje na koje se želi pomaknuti

    if (p.tip == -1) {
    ploca[f.x][f.y] = {-1, -1};
    f.x = x; f.y = y;
    f.brojPoteza++;             
    p = {f.id, f.tip};
    return true;
    }

    auto &branitelj = figura[p.id];
    cout << "Borba: Napadac(tip=" << f.tip << ",id=" << f.id
         << ") vs Branitelj(tip=" << p.tip << ",id=" << p.id << ") ";

    bool napadacPobjedjuje = (f.tip > p.tip || (f.tip == 0 && p.tip == 2)) && !(p.tip == 0 && f.tip == 2); //logika borbe između branitelja i napadača
    if (napadacPobjedjuje) {
    cout << "=> napadac pobjedjuje\n";
    branitelj.ziv = false;
    ploca[f.x][f.y] = {-1, -1};
    f.x = x; f.y = y;               
    f.brojPoteza++;                 
    f.brojPojedenih++;
    p = {f.id, f.tip};
    } 
    else {
    cout << "=> branitelj pobjedjuje\n";
    f.ziv = false;                  
    branitelj.brojPojedenih++;  
    }
    return true;
}

void kretanje(int i) {
    static const int smjerovi[3][4][2] = {
        {{1, 0}, {-1, 0}, {0, 1}, {0, -1}},//pijun(gore,dole,lijevo,desno)
        {{1, 1}, {-1, -1}, {1, -1}, {-1, 1}},//lovac(dijagonalno gore-desno/lijevo,dijagon dole-desno/lijevo)
        {{2, 0}, {-3, 0}, {0, 1}, {-1, 0}}//top(ograniceno,2dolje,3gore,1desno,1lijevo)
    };

    int nRandonNumber = rand() % 4;
    int nx = figura[i].x + smjerovi[figura[i].tip][nRandonNumber][0];
    int ny = figura[i].y + smjerovi[figura[i].tip][nRandonNumber][1];
    pomicanje(i, nx, ny);
}

int main(int argc, char *argv[]) {
    MPI::Init(argc, argv);
    int rank = MPI::COMM_WORLD.Get_rank();

    initializeBoard(rank);
    srand((unsigned)time(NULL) + rank);

    while (true) { //petlja koja omogućava kretanje sve dok postoji više od 1 živa figura 
        if (rank == 0) {
            for (int i = 0; i < BROJ_FIGURA; i++) {
                if (figura[i].ziv) kretanje(i);
            }

            int brojac = 0, lastAliveIndex = -1;
            for (int i = 0; i < BROJ_FIGURA; i++) {
                if (figura[i].ziv) { brojac++; lastAliveIndex = i; }
            }

            if (brojac <= 1) {
                if (brojac == 1) {
                    cout << "\nSah mat! Pobjednik je figura ID=" << figura[lastAliveIndex].id
                         << " (Poteza=" << figura[lastAliveIndex].brojPoteza
                         << ", Pojedenih=" << figura[lastAliveIndex].brojPojedenih << ")\n";//kol el
                } else {
                    cout << "\nSah mat! Nema preživjelih figura\n";
                }
                MPI::COMM_WORLD.Abort(0);
            }
        }
        MPI::COMM_WORLD.Barrier();
    }

    MPI::Finalize();
    return 0;
}
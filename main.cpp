#include <fstream>
#include <iostream>
#include "prodCons.h"
#include "semaforo.h"

#include <thread>
#include <cstdlib>
#include <ctime>
#include <mutex>
using namespace std;

extern int jobsFinalizados;
extern ofstream logFile;

int main()
{
    srand(time(nullptr));

    // Semáforos
    init(hay_espacio, 20);
    init(hay_datos, 0);
    init(vram, 5);

    // PRODUCTORES (20 jobs total)
    std::thread p1(productor, 1, 10);
    std::thread p2(productor, 2, 10);

    // CONSUMIDORES (20 consumos total)
    std::thread w1(consumidor, 1, 5);
    std::thread w2(consumidor, 2, 5);
    std::thread w3(consumidor, 3, 5);
    std::thread w4(consumidor, 4, 5);

    p1.join();
    p2.join();

    w1.join();
    w2.join();
    w3.join();
    w4.join();



    cout << "\nTOTAL DE JOBS FINALIZADOS: "
          << jobsFinalizados
          << std::endl;

    logFile.close();
    return 0;
}

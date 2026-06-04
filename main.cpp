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
int totalJobs = 0;

int main()
{
    srand(time(nullptr));

    // ── Configuracion del escenario ──────────────────────────
    int numProductores  = 3;
    int numConsumidores = 3;
    totalJobs           = 1500;
    // ────────────────────────────────────────────────────────

    init(hay_espacio, totalJobs);
    init(hay_datos, 0);
    init(vram, 5);

    vector<thread> productores;
    for (int i = 1; i <= numProductores; i++)
        productores.emplace_back(productor, i);

    vector<thread> consumidores;
    for (int i = 1; i <= numConsumidores; i++)
        consumidores.emplace_back(consumidor, i);

    for (auto& p : productores) p.join();

    // Productores terminaron: despertar a todos los consumidores
    // que puedan estar bloqueados en wait(hay_datos)
    for (int i = 0; i < numConsumidores; i++)
        signal(hay_datos);

    for (auto& c : consumidores) c.join();

    cout << "\nTOTAL DE JOBS FINALIZADOS: " << jobsFinalizados << endl;

    logFile.close();
    return 0;
}

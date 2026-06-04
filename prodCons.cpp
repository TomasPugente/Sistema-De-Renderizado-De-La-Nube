#include "prodCons.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>
#include <ctime>

using namespace std;

// ==================== COLAS DE PRIORIDAD ====================

queue<Job> colaPremium;
queue<Job> colaFree;

// ==================== SEMAFOROS ====================

Semaforo hay_datos;
Semaforo hay_espacio;
Semaforo vram;

// ==================== MUTEXES ====================

mutex mtxCola;
mutex mtxId;
mutex mtxConsola;
mutex mtxFinalizados;
mutex mtxLog;
mutex mtxVRAM;


ofstream logFile("actividad.log");

int jobsFinalizados      = 0;
int idGlobal             = 0;
int premiumConsecutivos  = 0;
extern int totalJobs;
int jobsProducidos = 0;

Job poolVRAM[5];
bool slotOcupado[5] = { true, false, false, false, false };


// ==================== LOGGING ====================

void escribirLog(int idJob, int prioridad, const string& evento)
{
    time_t ahora = time(nullptr);
    tm* tiempoLocal = localtime(&ahora);

    char buffer[9];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", tiempoLocal);

    mtxLog.lock();
    logFile
            << "[" << buffer << "] - "
            << idJob          << " - "
            << (prioridad ? "Premium" : "Free") << " - "
            << evento
            << endl;
    mtxLog.unlock();
}


// ==================== VRAM ====================

int reservarSlotVRAM(Job job)
{
    mtxVRAM.lock();

    int slot = -1;
    for (int i = 0; i < 5; i++)
    {
        if (!slotOcupado[i])
        {
            slotOcupado[i] = true;
            poolVRAM[i]    = job;
            slot           = i;
            break;
        }
    }

    mtxVRAM.unlock();
    return slot;
}


void liberarSlotVRAM(int slot)
{
    mtxVRAM.lock();
    slotOcupado[slot] = false;
    mtxVRAM.unlock();
}


// ==================== PRODUCTOR ====================

void productor(int idProductor)
{

    while (true)
    {

        // Reservar atomicamente un job para producir
        mtxId.lock();
        if (jobsProducidos >= totalJobs)
        {
            mtxId.unlock();
            break;
        }
        int miJob = jobsProducidos++;
        int miId  = idGlobal++;
        mtxId.unlock();

        Job job;
        job.id        = miId;
        job.prioridad = (rand() % 2);

        mtxConsola.lock();
        cout << "[PRODUCTOR " << idProductor << "] "<< "CREA Job " << job.id<< " (" << (job.prioridad ? "Premium" : "Free") << ")"<< endl;
        mtxConsola.unlock();

        escribirLog(job.id, job.prioridad, "CREADO - TRABAJADOR");

        wait(hay_espacio);

        mtxCola.lock();
        if (job.prioridad)
            colaPremium.push(job);
        else
            colaFree.push(job);
        mtxCola.unlock();

        escribirLog(job.id, job.prioridad, "EN_COLA - TRABAJADOR");
        signal(hay_datos);

        this_thread::sleep_for(chrono::milliseconds(100));
    }

    mtxConsola.lock();
    cout << "[PRODUCTOR " << idProductor << "] "
         << "Sin mas jobs. Finalizando." << endl;
    mtxConsola.unlock();
}
void consumidor(int idConsumidor)
{
    while (true)
    {
        wait(hay_datos);

        mtxFinalizados.lock();
        bool terminar = (jobsFinalizados >= totalJobs);
        mtxFinalizados.unlock();

        if (terminar) break;

        Job job;

        mtxCola.lock();

        if (colaPremium.empty() && colaFree.empty())
        {
            mtxCola.unlock();
            break;
        }

        if (!colaPremium.empty() &&
                (premiumConsecutivos < 5 || colaFree.empty()))
        {
            job = colaPremium.front();
            colaPremium.pop();
            premiumConsecutivos++;
        }
        else
        {
            job = colaFree.front();
            colaFree.pop();
            premiumConsecutivos = 0;
        }

        mtxCola.unlock();
        signal(hay_espacio);

        mtxConsola.lock();
        cout << "[CONSUMIDOR " << idConsumidor << "] "<< "TOMA Job " << job.id<< " (" << (job.prioridad ? "Premium" : "Free") << ")"<< endl;
        mtxConsola.unlock();

        wait(vram);
        this_thread::sleep_for(chrono::milliseconds(450));

        int slot = reservarSlotVRAM(job);
        escribirLog(job.id, job.prioridad, "ASIGNADO_VRAM - CONSUMIDOR");

        mtxConsola.lock();
        cout << "[CONSUMIDOR " << idConsumidor << "] "<< "PROCESANDO Job " << job.id<< " en slot " << slot<< endl;
        mtxConsola.unlock();

        this_thread::sleep_for(chrono::milliseconds(600));

        liberarSlotVRAM(slot);
        this_thread::sleep_for(chrono::milliseconds(250));
        signal(vram);

        mtxFinalizados.lock();
        jobsFinalizados++;
        mtxFinalizados.unlock();

        mtxConsola.lock();
        cout << "[CONSUMIDOR " << idConsumidor << "] "<< "FINALIZA Job " << job.id<< endl;
        mtxConsola.unlock();

        escribirLog(job.id, job.prioridad, "FINALIZADO - CONSUMIDOR");
    }

    mtxConsola.lock();
    cout << "[CONSUMIDOR " << idConsumidor << "] "<< "Sin mas jobs. Finalizando." << endl;
    mtxConsola.unlock();
}

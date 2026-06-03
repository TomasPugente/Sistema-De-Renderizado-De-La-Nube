#include "prodCons.h"
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>
#include <ctime>

using namespace std;

std::queue<Job> colaPremium;
std::queue<Job> colaFree;




Semaforo hay_datos;
Semaforo hay_espacio;
Semaforo vram;

mutex mtxCola;
mutex mtxId;
mutex mtxConsola;
mutex mtxFinalizados;
mutex mtxLog;
mutex mtxVRAM;

ofstream logFile("sistema.log");
int jobsFinalizados=0;
int idGlobal = 0;
int premiumConsecutivos = 0;
Job poolVRAM[5];
bool slotOcupado[5] = {false,false,false,false,false};


// ---------------- ESCRIBIR LOG -------------
void escribirLog(int idJob,int prioridad,const  string& evento)
{
    time_t ahora = time(nullptr);

    mtxLog.lock();

    logFile
        << "[" << ahora << "] - "
        << idJob << " - "
        << (prioridad ? "Premium" : "Free")
        << " - "
        << evento
        << std::endl;

    mtxLog.unlock();
}

// ---------------- PRODUCTOR ----------------

int reservarSlotVRAM(Job job)
{
    mtxVRAM.lock();

    int slot = -1;

    for(int i = 0; i < 5; i++)
    {
        if(!slotOcupado[i])
        {
            slotOcupado[i] = true;
            poolVRAM[i] = job;
            slot = i;
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

void productor(int idProductor, int cantidad)
{
    for(int i = 0; i < cantidad; i++)
    {
        Job job;

        mtxId.lock();
        job.id = idGlobal++;
        mtxId.unlock();

        job.prioridad = (i % 2 == 0) ? 1 : 0;

        mtxConsola.lock();
        std::cout
            << "[PRODUCTOR " << idProductor << "] CREA Job "
            << job.id
            << " - "
            << (job.prioridad ? "Premium" : "Free")
            << std::endl;
        mtxConsola.unlock();

        escribirLog(job.id,job.prioridad,"CREADO");

        wait(hay_espacio);

        mtxCola.lock();

        if(job.prioridad)
            colaPremium.push(job);
        else
            colaFree.push(job);

        mtxCola.unlock();
        escribirLog(job.id,job.prioridad,"EN_COLA");

        signal(hay_datos);



        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }
}

// ---------------- CONSUMIDOR ----------------

void consumidor(int idConsumidor, int cantidad)
{
    for(int i = 0; i < cantidad; i++)
    {
        wait(hay_datos);

        Job job;

        mtxCola.lock();

        if(
            !colaPremium.empty() &&
            (premiumConsecutivos < 5 || colaFree.empty())
        )
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
        std::cout
            << "[WORKER " << idConsumidor << "] TOMA Job "
            << job.id
            << " - "
            << (job.prioridad ? "Premium" : "Free")
            << std::endl;
        mtxConsola.unlock();

        wait(vram);
        std::this_thread::sleep_for(
    std::chrono::milliseconds(450)
);
        int slot = reservarSlotVRAM(job);
        escribirLog(job.id,job.prioridad,"ASIGNADO_VRAM");

        mtxConsola.lock();
        std::cout
            << "[WORKER " << idConsumidor << "] PROCESANDO Job "
            << job.id << std::endl;
        mtxConsola.unlock();
        std::this_thread::sleep_for(
            std::chrono::milliseconds(600)
        );

        liberarSlotVRAM(slot);
        std::this_thread::sleep_for(
    std::chrono::milliseconds(250));
        signal(vram);
        mtxFinalizados.lock();
        jobsFinalizados++;
        mtxFinalizados.unlock();

        mtxConsola.lock();
        std::cout
            << "[WORKER " << idConsumidor << "] FINALIZA Job "
            << job.id << std::endl;
        mtxConsola.unlock();

        escribirLog(job.id,job.prioridad,"FINALIZADO");

    }
}

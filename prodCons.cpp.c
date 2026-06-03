#include "prodCons.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::queue<Job> colaPremium;
std::queue<Job> colaFree;

Semaforo hay_datos;
Semaforo hay_espacio;
Semaforo vram;

std::mutex mtxCola;
std::mutex mtxId;

int idGlobal = 0;

// ---------------- PRODUCTOR ----------------

void productor(int idProductor, int cantidad)
{
    for(int i = 0; i < cantidad; i++)
    {
        Job job;

        {
            std::lock_guard<std::mutex> lock(mtxId);
            job.id = idGlobal++;
        }

        job.prioridad = (i % 2 == 0) ? 1 : 0;

        std::cout
            << "[PRODUCTOR " << idProductor << "] CREA Job "
            << job.id
            << " - "
            << (job.prioridad ? "Premium" : "Free")
            << std::endl;

        wait(hay_espacio);

        {
            std::lock_guard<std::mutex> lock(mtxCola);

            if(job.prioridad)
                colaPremium.push(job);
            else
                colaFree.push(job);
        }

        signal(hay_datos);

        std::this_thread::sleep_for(
            std::chrono::milliseconds(100)
        );
    }
}

// ---------------- CONSUMIDOR ----------------

void consumidor(int idWorker, int cantidad)
{
    for(int i = 0; i < cantidad; i++)
    {
        wait(hay_datos);

        Job job;

        {
            std::lock_guard<std::mutex> lock(mtxCola);

            if(!colaPremium.empty())
            {
                job = colaPremium.front();
                colaPremium.pop();
            }
            else
            {
                job = colaFree.front();
                colaFree.pop();
            }
        }

        signal(hay_espacio);

        std::cout
            << "[WORKER " << idWorker << "] TOMA Job "
            << job.id
            << " - "
            << (job.prioridad ? "Premium" : "Free")
            << std::endl;

        wait(vram);

        std::cout
            << "[WORKER " << idWorker << "] PROCESANDO Job "
            << job.id << std::endl;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(600)
        );

        signal(vram);

        std::cout
            << "[WORKER " << idWorker << "] FINALIZA Job "
            << job.id << std::endl;
    }
}

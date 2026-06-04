#ifndef PRODCONS_H
#define PRODCONS_H

#include "semaforo.h"
#include <queue>

struct Job
{
    int id;
    int prioridad; // 1 = Premium, 0 = Free
};

extern std::queue<Job> colaPremium;
extern std::queue<Job> colaFree;

extern Semaforo hay_datos;
extern Semaforo hay_espacio;
extern Semaforo vram;


void productor(int idProductor);
void consumidor(int idConsumidor);

#endif

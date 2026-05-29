#ifndef SEMAFORO_H_INCLUDED
#define SEMAFORO_H_INCLUDED

#include <iostream>
#include "semaforo.h"
#include <condition_variable>
#include <mutex>

using namespace std;

struct Semaforo{
int contador;

std::condition_variable cv;

std::mutex mtx;

};

///typedef

void init(Semaforo& s,int n);

void wait(Semaforo& s);

void signal(Semaforo& s);






#endif // SEMAFORO_H_INCLUDED

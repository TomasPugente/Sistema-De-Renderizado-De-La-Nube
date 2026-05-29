#include <iostream>
#include "semaforo.h"
#include <condition_variable>
#include <mutex>

void init(Semaforo& s,int n)
{
    s.contador=n;
}

void wait(Semaforo& s)
{
///std::unique_lock<std::mutex> mtx;

    std::unique_lock<std::mutex> lock(s.mtx);

    while(s.contador==0)
    {

        s.cv.wait(lock);
    }

    s.contador--;

}

void signal(Semaforo& s)
{
    std::unique_lock<std::mutex> lock(s.mtx);
    s.contador++;
    s.cv.notify_one();
}




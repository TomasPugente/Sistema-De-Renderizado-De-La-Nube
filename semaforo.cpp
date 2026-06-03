#include "semaforo.h"
using namespace std;

void init(Semaforo& s, int n)
{
    s.mtx.lock();
    s.contador = n;
    s.mtx.unlock();
}

void wait(Semaforo& s)
{
    unique_lock<mutex> lock(s.mtx);
    s.cv.wait(lock, [&]{ return s.contador > 0; });
    s.contador--;
}

void signal(Semaforo& s)
{
    s.mtx.lock();
    s.contador++;
    s.mtx.unlock();

    s.cv.notify_one();
}

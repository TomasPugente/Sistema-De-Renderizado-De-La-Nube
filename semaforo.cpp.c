#include "semaforo.h"

void init(Semaforo& s, int n)
{
    std::lock_guard<std::mutex> lock(s.mtx);
    s.contador = n;
}

void wait(Semaforo& s)
{
    std::unique_lock<std::mutex> lock(s.mtx);
    s.cv.wait(lock, [&]{ return s.contador > 0; });
    s.contador--;
}

void signal(Semaforo& s)
{
    {
        std::lock_guard<std::mutex> lock(s.mtx);
        s.contador++;
    }
    s.cv.notify_one();
}

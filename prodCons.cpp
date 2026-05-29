#include <iostream>
#include "semaforo.h"
#include <condition_variable>
#include <mutex>
#include "prodCons.h"
#include <queue>

using namespace std;

///extern std::queue<int> buffer;
extern queue<int> msgQueue;///insertar acá
extern queue<int> poolVRam;///insertar acà lo que se extrajo de msgQueue
mutex mtx_consola;
mutex mtx_contador;
mutex mtx_productor;
extern mutex mtx_buffer;



const int tam=20;
int contador=0;
int producidos=0;

extern Semaforo hay_espacio;///extern sirve para no redefinir la variable
extern Semaforo hay_datos;


void productor(int id, int prioridad)///inserta datos en la queue
{
    bool condicion=true;

    while(condicion)
    {
        mtx_productor.lock();
        if(producidos<tam){


        wait(hay_espacio);

        mtx_buffer.lock();
        ///PREGUNTAR POR LA MSGQUEUE Y VRAM-----
        msgQueue.push(producidos);///inserta datos en la queue

        mtx_consola.lock();
        cout<<"hay_espacio: "<<hay_espacio.contador<<endl;
        mtx_consola.unlock();

        mtx_buffer.unlock();

        signal(hay_datos);
        signal(hay_espacio);

        mtx_consola.lock();
        cout<<"Productor "<<id<<": produjo "<<producidos<<endl;
        mtx_consola.unlock();


        producidos++;
        }else{
        condicion=false;
        }
        mtx_productor.unlock();
    }
    mtx_consola.lock();
    cout<<"\nProducidos "<<producidos<<endl;
    mtx_consola.unlock();
}

void consumidor(int id, int prioridad)///extrae datos de la queue para insertarlos en la VRam
{
    int productosConsumidos=0;
    bool condicion=true;


    while(condicion)
    {
        mtx_contador.lock();
        if(contador<tam){

        wait(hay_espacio);
        wait(hay_datos);///espera a que haya datos


        mtx_buffer.lock();
        int consumido=msgQueue.front();
        mtx_buffer.unlock();

        poolVRam.push(consumido);///inserta el dato previamente guardado en el poolVram

        signal(hay_espacio);

        mtx_consola.lock();
        cout<<"Consumidor "<<id<<": Consumio "<<consumido<<endl;
        mtx_consola.unlock();

        contador++;

        productosConsumidos++;
        }else{
            condicion=false;
        }
        mtx_contador.unlock();

    }

        mtx_consola.lock();
        cout<<"CONSUMIDOR["<<id<<"] Consumio "<<productosConsumidos<<" productos"<<endl;
        mtx_consola.unlock();
}





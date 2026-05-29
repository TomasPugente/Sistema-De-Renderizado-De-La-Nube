#include <iostream>
#include "semaforo.h"
///#include <condition_variable>
///#include <mutex>
#include "prodCons.h"
#include <thread>
#include <queue>
#include "job.h"

/*IMPLEMENTAR:
*PROGRAMACION CONCURRENTE(MÚLTIPLES HILOS)-Uso de múltiples hilos. Cada job debe ser generado y procesado con hilos independientes.
*CONDICIONES DE CARRERA-2. Race Condition Gestionar correctamente el acceso concurrente al contador global de tareas
*EXCLUSIÓN MUTUA-3. Exclusión Mutua Dos procesos no pueden acceder al mismo slot del Pool de VRAM en el mismo instante.
*PRODUCTOR-CONSUMIDOR-4. Productor-Consumidor Sincronización de hilos para la inserción en la Queue y extracción hacia el Pool de VRam límite de 5
*STARVATION-5. Starvation (Inanición) Garantizar que los jobs de prioridad "Free" se procesen incluso bajo alta demanda de usuarios "Premium".
*PERSISTENCIA-6. Persistencia (Logging) Registro sincronizado de cada cambio de estado del job en un archivo actividad.log.

gestionar correctamente el acceso concurrente al contador global de tareas finalizadas con èxito
*/

using namespace std;


Semaforo hay_espacio;
Semaforo hay_datos;

queue<int> msgQueue;///insertar acá(productor)
queue<int> poolVRam;///insertar acà lo que se extrajo de msgQueue(consumidor)

mutex mtx_buffer;

int main()
{
    ///inicilizar los semáforos
    init(hay_espacio,5);///el espacio máximo siempre es 5,o puede ser mayor?
    init(hay_datos,0);

    thread t1(productor, 1, 0);
    thread t2(productor, 2, 1);
    thread t3(consumidor,1,0);///si comento los consumidores,el programa nunca termina
    thread t4(consumidor,2,0);
    thread t5(consumidor,3,1);



    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    ///t3.join();



    if(msgQueue.empty()){
    //if(poolVRam.empty()){

        cout<<"\nEl consumidor consumio todo "<<endl;
    }
    else{
        cout<<"\nNo se pudo consumir todo"<<endl;
    }


    return 0;
}

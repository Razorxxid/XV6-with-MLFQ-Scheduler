# LAB 3: PLANIFICADOR MLFQ

### *Integrantes*: Tomás Colazo, Fracchia Joaquin, Santiago Marini

El informe se divide en tres partes:
 1. Planificador Round Robin xv6 (original): Explicación de la transición de estado que realiza el proceso. 
 2. Planificador MLFQ xv6 (implementado): Funcionamiento para esas mismas transiciones de estado.
 3. Gráficos para establecer comparaciones entre los dos planificadores anteriores.
## **Planificador Original de XV6: Round Robin**
Aveces, para cambiar el estado del proceso, el algoritmo cuenta con funciones para aplicar un candado a la tabla de procesos y evitar que sea modificada. Esto se realiza por cuestiones de protección, trabando y destrabándola secuencialmente mientras que realizamos los cambios entre medio. Las funciones de xv6 que nombraremos utilizan este método.

Los procesos empiezan con un estado llamado ```EMBRYO```, que corresponde a NEW, es decir el proceso se está creando y está por pasar a estar listo.
Para **ejecutar un proceso planificable** el algoritmo recorre una tabla de procesos, y en caso de que alguno ya esté listo para correr (```RUNNABLE```) se lo asigna al CPU, setea el stack del kernel para el proceso, se para en su page table y lo marca como ```RUNNING```. Luego de que se ejecuta lo suficiente, configura nuevamente los registros del kernel, la page table, y como último le avisa al CPU que no esta corriendo ningún proceso, para así pasar al próximo.


Al **Bloquear un proceso que está esperando entrada/salida** se ejecuta la función ```sleep``` que logra cambiar el estado del proceso de ```RUNNING``` a ```SLEEPING``` también llamado blocked. Luego se encarga de volver al ```scheduler``` hasta que el proceso sea marcado como ```RUNNABLE``` en algun momento.

Acto seguido, para hacer que un **proceso que duerme sea planificable** se le debe cambiar del estado ```SLEEPING``` al ```RUNNABLE```. De esto se encarga la funcion ```wakeup```. La misma itera la tabla de procesos y a aquellos que se encuentren en sleeping, los va a cambiar a runnable. En ```exit``` se puede ver una aplicación, al utilizarse para despertar al proceso padre en caso de que se encuentre durmiendo.

Si un  proceso se encuentra en estado ```RUNNING``` se debe poder **replanificar si es que consumió su tiempo de procesador disponible.**
De esto se encarga la función ```yield``` al marcar al estado del proceso como ```RUNNABLE``` y luego volver al scheduler. 

## **Planificador Implementado: MLFQ (Multilevel Feedback Queue)**
Para implementar MLFQ tuvimos que modificar las funciones explicadas anteriormente (para alterar la prioridad del proceso) junto con el scheduler principal.

Tuvimos que implementar lo siguiente para poder continuar:
 * Un campo ```priority``` a la estructura ```proc``` el cual denota la prioridad del proceso, que corresponde con las NPRIO colas de prioridad que tendrá el planificador.
 * Un arreglo que represente la MLFQ, donde la cantidad de elementos son las n colas

En ```init``` nos encargamos de inicializar la MLFQ con la cantidad de colas que queramos (constante definida en  param.h). Para poder almacenar procesos la misma está implementada con la estructura cbuffer dada por la cátedra.


Entonces su forma de operar sería la siguiente:
Cuando se necesita **ejecutar un proceso planificable** (función  ```scheduler```) lo que hacemos es encontrar el primer proceso que ya esté en estado  ```RUNNABLE```, para ello declaramos la función  ```first_process``` que se encarga de recorrer las colas y apenas encontramos un proceso (si la cola no estaba vacia) simplemente lo devolvemos. Por lo tanto cada vez que sea llamada, se fija desde la primera cola de más prioridad.
Una vez que sea encontrado el proceso,  cambiamos su estado a  ```RUNNING``` y lo corremos pasándoselo al CPU de la misma forma descripta en el planificador Round Robin.
 
Al querer que un **proceso que duerme sea planificable** ( estado ```SLEEPING``` a ```RUNNABLE``` ) también utilizamos la función ```wakeup```. A diferencia de RR, después de que el proceso ya haya realizado el cambio de estado, necesitamos alterar el campo ```priority``` del proceso.
En este caso, subirlo una cola ya que hay que darle prioridad una vez que sea desbloqueado. (lo que se traduce a escribir ese proceso en la MLFQ, indexada con el número de prioridad el cual queremos que se encuentre de ahora en más).

 Al **replanificar un proceso si es que consumió su tiempo de procesador disponible** (estado ```RUNNING``` a ```RUNNABLE```) también es similar a RR, pero en la función ```yield``` luego de que el proceso cambie el estado, lo bajamos una cola de prioridad ya que tiende a ser cpubound. Como siempre, al modificar el campo ```priority``` tenemos que escribir ese proceso en la cola correspondiende de la MLFQ.
 
Para **Bloquear un proceso que está esperando entrada/salida** , mantenemos al proceso en el mismo nivel,ya que está esperando hasta poder despertarse.
 
 En la función ```fork``` , para poder manejar la prioridad que tendrá el nuevo proceso creado, la igualamos a la prioridad del padre ya que el proceso hijo es un clon.
 
 * Extra: Ésta implementación de MLFQ sí genera starvation, por el hecho de que luego de elegir al proceso en ```first_process```, volvemos a fijarnos en la más alta prioridad. Lo que genera que aquellos procesos cpubound se mueran de hambre, porque el cpu está siendo consumido por aquellos de las primeras colas. Se podría solucionar elevándolos a todos a la cola de más alta prioridad cada X cantidad de tiempo. 

## ***Gráfico de comparación de RR y MLFQ***
Para construir el gràfico, tomamos muestras de los procesos ```iobench``` y ```cpubench``` corriendo a la misma vez. Fueron 10 pruebas por #veces que se ejecutan en simultaneo (EJE X), de esas 10 pruebas tomamos el promedio, y calculamos el número #CPUbench/#IObench (EJE Y). Luego, a esos datos lo mandamos a un graficador y obtuvimos el siguiente gráfico:
![](https://i.ibb.co/SN0xpRX/10aseg-graph.png)

Escala: EJE Y: de 0.0 - 0.5
        EJE X: de 0.0 - 5.0

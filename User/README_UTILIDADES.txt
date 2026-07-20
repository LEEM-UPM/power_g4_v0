UTILIDADES DEL PROYECTO (Víctor Moraleda)
==============================

Objetivo
--------
Documentar funciones utilitarias usadas por el proyecto, su comportamiento
esperado y ejemplos de uso.

Estructura recomendada por funcion
----------------------------------
Nombre:
Archivo:
Descripcion:
Parametros:
Retorno:
Efectos secundarios:
Dependencias:
Ejemplo:

--------------------

Nombre: Pyro1_ON/Pyro2_ON
Archivo: User/Src/utils_gpio.c
Descripcion: Pone en alto el pin encargado de activar la alimentación de 7V proporcionada por el AP62600SJ-7, dicha 
alimentación es la que llegará al conector del pirotecnico
Parametros: void
Retorno: void
Efectos secundarios: non
Dependencias: HAL GPIO inicializado.
Ejemplo: Pyro1_ON();

--------------------

Nombre: Pyro1_OFF/Pyro2_OFF
Archivo: User/Src/utils_gpio.c
Descripcion: Pone en bajo el pin encargado de activar la alimentación de 7V proporcionada por el AP62600SJ-7, dicha 
alimentación es la que llegará al conector del pirotecnico
Parametros: void
Retorno: void
Efectos secundarios: non
Dependencias: HAL GPIO inicializado.
Ejemplo: Pyro1_OFF();

--------------------

Nombre: Pyro1A_ON/Pyro2B_ON
Archivo: User/Src/utils_gpio.c
Descripcion: Pone en alto el pin encargado de activar el MOSFET que corta GND del pirotecnico, 
permitiendo el paso de corriente y cerrando el circuito
Parametros: void
Retorno: void
Efectos secundarios: non
Dependencias: HAL GPIO inicializado.
Ejemplo: Pyro1A_ON();

--------------------

Nombre: Pyro1A_OFF/Pyro2B_OFF
Archivo: User/Src/utils_gpio.c
Descripcion: Pone en bajo el pin encargado de activar el MOSFET que corta GND del pirotecnico, abriendo 
el circuito e impidiendo la circulación de corriente
Parametros: void
Retorno: void
Efectos secundarios: non
Dependencias: HAL GPIO inicializado.
Ejemplo: Pyro1A_OFF();

--------------------

Nombre: AllPyroCheck
Archivo: User/Src/utils_gpio.c
Descripcion: Enciende la alimentacion de ambos canales para despues en intervalos de 1 segundo ir encendiendo consecutivamente los 
pirotecnicos, despues apaga todo instantaneamente
Parametros: void
Retorno: void
Efectos secundarios: Al encender los canales pirotecnicos existe el riesgo de cortocircuito si no se toman precauciones, acuerdese de haber conectado el rbf n't
Dependencias: HAL GPIO inicializado.
Ejemplo: AllPyroCheck();

--------------------

Nombre: Pyro1A_Continuity
Archivo: User/Src/utils_gpio.c
Descripcion: Mediante un pull up de alto valor en la linea dle pirotecnico se mide el voltaje en el otro extremo para ver si hay continuidad electrica,
retornando 1 en caso de que haya y 0 en caso contrario
Parametros: void
Retorno: bool
Efectos secundarios: que haya continuidad no asegura que no haya una alta resistencia en la linea
Dependencias: HAL GPIO inicializado.
Ejemplo: bool continuidad_canal = Pyro1A_Continuity();

--------------------

Nombre: Sys_init
Archivo: User/Src/utils_gpio.c
Descripcion: Inicia todos los perifericos y configuraciones necesarias para el correcto funcionamiento
de la placa
Parametros: void
Retorno: void
Efectos secundarios: non
Dependencias: HAL GPIO inicializado.
Ejemplo: Sys_init();

--------------------

Nombre: ThreeV_Current
Archivo: User/Src/utils_gpio.c
Descripcion: Devuelve el falos en amperios de la corriente circulando por 3.3V
Parametros: void
Retorno: float
Efectos secundarios: tiene un pequeño offset interno de 0.005, con lo cual en un consumo de 0 empezará dando ese valor
hasta que el valor real de consumo lo supere
Dependencias: HAL GPIO inicializado, ADCs iniciados.
Ejemplo: float corriente3V = ThreeV_Current();

Notas
-----
- Mantener esta documentacion sincronizada con User/Inc y User/Src.
- Si una funcion cambia de firma, actualizar este archivo en el mismo commit.
- Avisar de comportamientos inesperados descubiertos y posibles bugs

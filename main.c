/*
 * File:   main.c
 * Author: josepablocruzbaas
 * Codigo principal del funcionamiento del tablero de juego.
 * 
 * Created on November 10, 2016, 1:08 PM
 */
//
//  MODIFICACIÓN 1
//  FECHA : 23-MARZO-2017
//  NUMERO  DESCRIPCION
//      1       ELIMINAR SUMA AUTOMATICA DE FALTAS
//      2       ENCENDER UN BIT DEL PUERTO B CUANDO EL
//              RELOJ MARQUE CERO HASTA QUE SE MANDE
//              COMANDO DE PAUSA = MIENTRAS ESTE DESCONTANDO.
//      3       AL CAMBIAR EL PERIODO BORRAR FALTAS POR EQUIPO
//  FECHA : 14 - Julio - 2017
//  Numero  DESCRIPCION
//      4       COMANDO PARA SOLICITAR LA HORA DEL CRONOMETRO
//
#include "Configuracion.h"
#include "Control.h"
/*
 * Codigo principal. Configura la velocidad del oscilador,
 * activa las interrupciones, inicia la pantalla,
 * y administra al interprete.
 * */
void main(void){
        ConfigClockAndInt();
    configSysCtl(&TeamA,&TeamB,&Cronometro);
    setClock(&Cronometro,10,0,0);
    setClockPeriodo(&Cronometro,1);
    SetPorts();
    //Inicializar
        Show();
    
    startSerial();
    startClock();
    puts("Welcome!\n");
    while(1){
        if(SysCtl.SysCtlIF){
            SerialEvent();
        }
    }
}
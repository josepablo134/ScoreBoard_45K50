#include "Communication.h"
#include "Control.h"

/* *
 * Salida de evento.
 * Criterio de salida para cualquier comando interpretado.
 */
void ExitEvent(void){
    //Limpiamos interrupcion
    SysCtl.SysCtlIF = 0x00;
    //Limpiamos buffer
    SysCtl.SysRC[0] = 0x00;
    SysCtl.SysRC[1] = 0x00;
    SysCtl.SysRC[2] = 0x00;
    SysCtl.SysRC[3] = 0x00;
    SysCtl.SysRC[4] = 0x00;
    SysCtl.SysRC[5] = 0x00;
    SysCtl.SysRC[6] = 0x00;
    SysCtl.SysRC[7] = 0x00;
    SysCtl.SysRC[8] = 0x00;
    SysCtl.SysRC[9] = 0x00;
    SysCtl.SysRC[10] = 0x00;
    SysCtl.SysRC[11] = 0x00;
    SysCtl.SysRC[12] = 0x00;
    SysCtl.SysRC[13] = 0x00;
    SysCtl.SysRC[14] = 0x00;
    SysCtl.SysRC[15] = 0x00;
    SysCtl.SysRC[16] = 0x00;
    SysCtl.SysRC[17] = 0x00;
    SysCtl.SysRC[18] = 0x00;
    SysCtl.SysRC[19] = 0x00;
    SysCtl.i = 0;
    SysCtl.stack = 0;
    //Activar Recepcion por modulo UART
    // RCSTAbits.CREN = 1;
}
/* *
 * Evento Serial. Primer proceso de la interpretacion.
 * Se toma la decision de que comando es
 * y cuales seran sus parametros.
 */
void SerialEvent(void){
    //Evento serial
    //Limpiamos interrupcion
    SysCtl.SysCtlIF = 0x00;
    //Cargamos el comando al buffer
    unsigned char Dato = SysCtl.SysRC[0];
    switch((Dato>>6)&0x3){
        case 0:
            //Control de equipo
                //Eliminamos el tipo de comando
            CDE(Dato & 0x3F);
            puts("done!\n");
            break;
        case 1:
            //Control de reloj
                //Eliminamos el tipo de comando
            CDR(Dato & 0x3F);
            puts("done!\n");
            break;
        case 2:
            //Control de sistema
                //Eliminamos el tipo de comando
            CDS(Dato & 0x3F);
            puts("done!\n");
            break;
        default:
            puts("Error [0x00] - Comando no reconocido\n");
    }
    Show();
    ExitEvent();
}

//
//Contorl de Equipo
//

/* *
 * Control de equipos.
 * Intepreta el comando y modifica los puntos,
 * faltas o el apuntador de saque segun el comando.
 * @param   dato    El comando recibido por el puerto
 *                  serie previamente modificado
 */
void CDE(unsigned char dato){
    Team* a;
    if(dato>>5 & 0x01){
        puts("Team B\n");
        a = &TeamB;
    }else{
        puts("Team A\n");
        a = &TeamA;
    }
    //Eliminamos el team Flag
    dato&=0x1F;
    //Obtenemos el Mod Flag
    switch(dato>>3 & 0x3){
        case 0:
            //Puntos
                //Eliminamos el flag del modificador
            puts("Mod pts\n");
            CDE_Pts(a,dato&0x07);
            break;
        case 1:
            //Faltas
                //Eliminamos el flag del modificador
            puts("Mod flts\n");
            CDE_Flts(a,dato&0x07);
            break;
        case 2:
            //Falta y Player
                //Eliminamos el flag del modificador
            puts("Mod FnP\n");
            CDE_PnF(a,dato&0x07);
            break;
        case 3:
            //Puntero
                //Eliminamos el flag del modificador
            puts("Mod ptr\n");
            CDE_Ptr(a,dato&0x07);
            break;
        default:
            //Error
            puts("Error [0x01] - Comando no reconocido\n");
    }
    ExitEvent();
}
/* *
 * Control de equipos. Puntos.
 * Interpreta el comando y modifica los puntos del equipo
 * @param   team    Referencia a la estructura del equipo
 * @param   cmd     Comando a interpretar, previamente modificado
 */
void CDE_Pts(Team* team,unsigned char cmd){
    switch(cmd>>1){
        case 1:
            //Set
                //Dato Extra Enabled
            if(cmd%2){
                unsigned long a = get4(1);
                //Settear valor
                setTeamPts(team,a);
                //Mostrar valor
                //putTeamPts(team);
            }else{
                //Dato Extra Disabled
                puts("Error [0x10] - Comando incompleto\n");
            }
            break;
        default:
            //Error
            puts("Error [0x02] - Comando no reconocido\n");
    }
}
/* *
 * Control de equipos. Faltas
 * Interpreta el comando y modifica las faltas del equipo
 * @param   team    Referencia a la estructura del equipo
 * @param   cmd     Comando a interpretar, previamente modificado
 */
void CDE_Flts(Team* team,unsigned char cmd){
    switch(cmd>>1){
        case 1:
            //Set
                //Dato Extra Enabled
            if(cmd%2){
                //Esperar valor
                unsigned long a = get4(1);
                //Settear valor
                setTeamFaults(team,a);
                //Mostrar valor
//                putTeamFaults(team);
            }else{
                //Dato Extra Disabled
                puts("Error [0x10] - Comando incompleto\n");
            }
            break;
        default:
            //Error
            puts("Error [0x02] - Comando no reconocido\n");
    }
}
/* *
 * Control de equipos. Player and Faults.
 * Interpreta el comando y modifica las faltas del equipo
 * y agrega una falta temporar que desplegar en pantalla.
 * @param   team    Referencia a la estructura del equipo
 * @param   cmd     Comando a interpretar, previamente modificado
 */
void CDE_PnF(Team* team,unsigned char cmd){
    switch(cmd>>1){
        case 1:
            //Set
                //Dato Extra Enabled
            if(cmd%2){
                //Esperar valor del jugador
                int player = get4(1);
                if(player==-1){
                    puts("Cancelado");
                    return;
                }
                //Esperar valor de la falta
                int fault = get4(2);
                //Settear valor
                    //Player + Fault
                addTeamFault(team,player,fault);
                //Mostrar valor
//                putTeamFault(team);
            }else{
                //Dato Extra Disabled
                puts("Error [0x11] - Comando incompleto\n");
            }
            break;
        default:
            //Error
            puts("Error [0x02] - Comando no reconocido\n");
    }
}
/* *
 * Control de equipos. Puntero de saque.
 * Interpreta el comando y modifica el puntero de saque
 * @param   team    Referencia a la estructura del equipo
 * @param   cmd     Comando a interpretar, previamente modificado
 */
void CDE_Ptr(Team* team,unsigned char cmd){
    switch(cmd>>1){
        case 2:
            //Add
                //Dato Extra Enabled
            if(cmd%2){
                //Esperar valor
                int a = get4(1);
                if(a==1){
                    turnOnTeamPtr(team);
                }else{
                    turnOffTeamPtr();
                }
            }else{
                turnOnTeamPtr(team);
            }
            break;
        case 3:
            //Substract
            turnOffTeamPtr();
            break;
        default:
            //Error
            puts("Error [0x02] - Comando no reconocido\n");
    }
}

//
//Contorl de Reloj
//

/* *
 * Control de reloj.
 * Interpreta el comando y modifica el cronometro segun el comando.
 * @param   dato     Comando a interpretar, previamente modificado
 */
void CDR(unsigned char dato){
    //Obtenemos el Mod Flag
    switch(dato>>3){
        case 0:
            //Minutos
                //Eliminamos el flag del modificador
            puts("Mod min");
            CDR_Min(&Cronometro,dato&0x07);
            break;
        case 1:
            //Segundos
                //Eliminamos el flag del modificador
            puts("Mod sec");
            CDR_Sec(&Cronometro,dato&0x07);
            break;
        case 2:
            //Decimas
                //Eliminamos el flag del modificador
            puts("Mod dec");
            CDR_Dec(&Cronometro,dato&0x07);
            break;
        case 3:
            //Periodo
                //Eliminamos el flag del modificador
            puts("Mod Per");
            CDR_Per(&Cronometro,dato&0x07);
            break;
        case 4:
            //Todo
                //Eliminamos el flag del modificador
            puts("Mod all time");
            CDR_ALL(&Cronometro,dato&0x07);
            break;
        default:
            //Error
            puts("Error [0x01] - Comando no reconocido\n");
    }
    ExitEvent();
}
/* *
 * Control de reloj. Todo.
 * Interpreta el comando y modifica todo el cronometro. Minuts y segundos.
 * @param   c       Referencia a la estructura del cronometro
 * @param   data    Comando a interpretar, previamente modificado
 */
void CDR_ALL(Clock* c,unsigned char data){
    switch(data>>1){
        case 0:
            //Get - Mod 4
            //No se reciben parametros
            putInASCII(c->Clock_min);
            putInASCII(c->Clock_sec);
            putInASCII(c->Clock_dec);
            putInASCII(c->Periodo);
        case 1:
            //Set
            if(!(data%2)){
                puts("Error [0x11] - Comando incompleto\n");
                return;
            }
                //Minutos
            //Obtenemos 16 bits del reloj
            unsigned long min = get4(1);
            unsigned long sec = get4(2);
                    setClockMin(c,min);
                    setClockSec(c,sec);
                    setClockDec(c,0);
            break;
        default:
            puts("Error [0x02] - Comando no reconocido\n");
    }
}
/* *
 * Control de reloj. Minutos.
 * Interpreta el comando y modifica los minutos del cronometro.
 * @param   c       Referencia a la estructura del cronometro
 * @param   data    Comando a interpretar, previamente modificado
 */
void CDR_Min(Clock* c,unsigned char data){
    switch(data>>1){
        case 0:
            //Get - Mod 4
            //No se reciben parametros
            putInASCII(c->Clock_min);
        case 1:
            //Set
            if(!(data%2)){
                puts("Error [0x11] - Comando incompleto\n");
                return;
            }
            //Obtenemos Minutos
            unsigned long a = get4(1);
                setClockMin(c,a);
            break;
        default:
            puts("Error [0x02] - Comando no reconocido\n");
    }
}
/* *
 * Control de reloj. Segundos.
 * Interpreta el comando y modifica los segundos del cronometro.
 * @param   c       Referencia a la estructura del cronometro
 * @param   data    Comando a interpretar, previamente modificado
 */
void CDR_Sec(Clock* c,unsigned char data){
    switch(data>>1){
        case 0:
            //Get - Mod 4
            //No se reciben parametros
            putInASCII(c->Clock_sec);
        case 1:
            //Set
            if(!(data%2)){
                puts("Error [0x11] - Comando incompleto\n");
                return;
            }
            //Obtenemos Segundos
            unsigned long a = get4(1);
                    setClockSec(c,a);
            break;
        default:
            puts("Error [0x02] - Comando no reconocido\n");
    }
}
/* *
 * Control de reloj. Segundos.
 * Interpreta el comando y modifica las decimas del cronometro.
 * @param   c       Referencia a la estructura del cronometro
 * @param   data    Comando a interpretar, previamente modificado
 */
void CDR_Dec(Clock* c,unsigned char data){
    switch(data>>1){
        case 0:
            //Get - Mod 4
            //No se reciben parametros
            putInASCII(c->Clock_dec);
        case 1:
            //Set
            if(!(data%2)){
                puts("Error [0x11] - Comando incompleto\n");
                return;
            }
            //Obtenemos Decimas
            unsigned long a = get4(1);
                    setClockDec(c,a);
            break;
        default:
            puts("Error [0x02] - Comando no reconocido\n");
    }
}
/* *
 * Control de reloj. Periodo.
 * Interpreta el comando y modifica el periodo del cronometro
 * @param   c       Referencia a la estructura del cronometro
 * @param   data    Comando a interpretar, previamente modificado
 */
void CDR_Per(Clock* c,unsigned char data){
    switch(data>>1){
        case 0:
            //Get - Mod 4
            //No se reciben parametros
            putInASCII(c->Periodo);
        case 1:
            //Set
            if(!(data%2)){
                puts("Error [0x11] - Comando incompleto\n");
                return;
            }
            //Obtenemos Decimas
            int a = get4(1);
                setClockPeriodo(c,a);
            break;
        default:
            puts("Error [0x02] - Comando no reconocido\n");
    }
}
//
//Control de sistema
//

/* *
 * Control de Sistema.
 * Interpreta el comando y modifica el el estado de las
 * interrupciones y el sistema.
 * @param   dato     Comando a interpretar, previamente modificado
 */
void CDS(unsigned char dato){
    switch(dato>>3){
        case 0:
            //Default
            CDS_Default();
            break;
        case 1:
            CDS_Start();
            //Start
            break;
        case 2:
            CDS_Pause();
            //Pause
            break;
        case 3:
            //Reset
            CDS_Reset();
            break;
        case 4:
            //Echo
            CDS_Echo();
            break;
        default:
            puts("Error [0x02] - Comando no reconocido\n");
    }
    ExitEvent();
}
/* *
 * Control de sistema. Por defecto
 * Restaura los datos el sistema por los datos
 * por defecto y detiene el reloj.
 */
void CDS_Default(void){
    //Colocamos reloj por defecto
    setClock(&Cronometro,10,0,0);
    //Periodo por defecto
    setClockPeriodo(&Cronometro,1);
    setTeamA(&TeamA);
    setTeamB(&TeamB);
    
    //Apagamos apuntadores
    turnOffTeamPtr();
    stopClock();
}
/* *
 * Control de sistema. Inicio
 * Reactiva el timer y las interrupciones de este.
 * 
 */
void CDS_Start(void){
    //Comenzamos el timer1
    startClock();
}
/* *
 * Control de sistema. Pausa
 * Desactiva el timer y las interrupciones de este.
 * 
 */
void CDS_Pause(void){
    //Detenemos el timer1
    stopClock();
}
/* *
 * Control de sistema. Resetear
 * Restaura los datos el sistema por los datos por defecto
 * y permite que el timer continue en funcionamiento.
 * 
 */
void CDS_Reset(void){
    //Colocamos reloj por defecto
    setClock(&Cronometro,12,0,0);
    //Periodo por defecto
    setClockPeriodo(&Cronometro,1);
    setTeamA(&TeamA);
    setTeamB(&TeamB);
    
    //Apagamos apuntadores
    turnOffTeamPtr();
    //Comenzamos cronometro
    startClock();
}
/* *
 * Control de sistema. Eco
 * Cada dato recibido lo reenvia por el puerto sin modificacion
 * 
 */
void CDS_Echo(void){
    while(1){
        unsigned char a=0;
        while(1){
                while(!(SysCtl.SysCtlIF & 0x01)){}
                    SysCtl.SysCtlIF = 0x00;
                    a = SysCtl.SysRC[0];
                    if(a==0xFE){
                        //End Flag = terminar sesion
                        return;
                    }
                    if(a==0xFF){
                        //Cancel Flag
                        puts("Cancelado");
                        return;
                    }
                    //Hacemos echo
                    putch(a);
            }
    }
}

/* *
 * Recupera los datos en el buffer serial y concatena cada palabra
 *  de 4 bits en un unico valor.
 * @param   command     Numerador del comando que se desea recuperar.
 */
unsigned long get4(unsigned char command){
    unsigned char a = 1;
    unsigned char pos = 1;
    unsigned long buffer = 0;
    while( pos<SysCtl.i ){
        if(a==command){
                break;//End flag
            }
        if( SysCtl.SysRC[ pos ] == 254 ){
            a++;
        }
        pos++;
    }
    a = pos;
    while( a<SysCtl.i ){
        if( SysCtl.SysRC[ a ] == 254 ){
            break;//End flag
        }
        buffer = buffer<<4;
// Solo de 4 en 4 bits
        buffer += (SysCtl.SysRC[ a ] & 0x0F);
        a++;
    }
    //Correr valores del buffer;
    return buffer;
}

#include "Control.h"

//
//  Control de puerto a segmentos/ Pantalla
//

/*
 * CARGA EL DIGITO MENOS SIGNIFICATIVO AL PUERTO EN
 * SU FORMA CORRESPONDIENTE PARA 7 SEGMENTOS
 * @param   digit  digito que imprimir en formato int
 * */
void putInt(unsigned char digit){
    GPIO_PORTD_WRITE(num[digit%10]);
}
/*
 * MANDA UNA SEÑAL DE ENCENDIDO
 * AL SEGMENTO SEÑALADO
 * @param   pin     Direccion en bits del segmento a encender
 * */
void turnOn(unsigned char pin){
    GPIO_PORTA_WRITE(pin);
}
/*
 * CARGA EL NUMERO DIGITO A DIGITO
 * Y ENCIENDE LOS SEGMENTOS SEÑALADOS
 * COMENZANDO POR EL MENOS SIGNIFICATIVO
 * @param   digit   Numero a convertir
 * @param   pins    Arreglo ordenado de pines a encender
 * @param   len     Cantidad de digitos a convertir
 * */
void writeInt(unsigned short digit,unsigned const char pins[],unsigned char len){
    unsigned short i=0;
    for(i=0; i<len; i++){
        turnOn( FALSE_SEGMENT );
        putInt(digit%10);   //Escribe el digito en el puerto
        turnOn( pins[i] );
        turnOn( FALSE_SEGMENT );
        digit /= 10;        //Se descarta el digito, se prepara el siguiente
    }
}
/* *
 * Envia todos los datos del sistema a su correspondiente segmento.
 * Datos del sistema: puntos, faltas, etc...
 */
void Show(void){
        putClockMin(SysCtl.SysCtlClock);
        putClockSec(SysCtl.SysCtlClock);
        putClockDec(SysCtl.SysCtlClock);
        
        putTeamFaults(&TeamA);
        putTeamFaults(&TeamB);
        putTeamFault(&TeamA);
        putTeamFault(&TeamB);
        putTeamPts(&TeamA);
        putTeamPts(&TeamB);
        
        putClockPeriodo(&Cronometro);
}

// 
//  Sistema de control
// 

/* *
 *  Configura el oscilador interno,
 *  enciende las interrupciones,
 *  activa la prioridad de interrupciones
 * 
 */
void ConfigClockAndInt(void){
    //
    //CLOCK ENABLED
    //
    OSCCON  = 0b01100110; //Oscilador interno 8Mhz
    OSCTUNE = 0b00111111; //Maxima frecuencia
    SCS1=1;
    //
    //INTERRUPT ENABLED
    //
    IPEN = 1;   //Prioridad de interrupciones activada
    GIE  = 1;   //Activar las interrupciones de alta prioridad
    GIEH = 1;
    PEIE = 1;   //Activar las interrupciones de baja prioridad
    GIEL = 1;
}
/* *
 * Configura el sistema de control,
 * inicializa los equipos y el cronometro
 * @param   TeamA   Referencia a la estructura donde se
 *                  almacenara todo lo referente al equipo A.
 * @param   TeamB   Referencia a la estructura donde se
 *                  almacenara todo lo referente al equipo B.
 * @param   clock   Referencia a la estructura donde se
 *                  almacenara todo lo referente al cronometro.
 */
void configSysCtl(Team *TeamA,Team *TeamB,Clock *clock){
    //Apuntar reset al inicio del programa
    SysCtl.SysTeamA = TeamA;
    SysCtl.SysTeamB = TeamB;
    SysCtl.SysCtlClock = clock;
    SysCtl.SysCtlIF = 0x00; //Limpiamos las interrupciones
    SysCtl.i = 0;
    SysCtl.stack = 0;
    SysCtl.SysRC[0] = 0;
    SysCtl.SysRC[1] = 0;
    SysCtl.SysRC[2] = 0;
    SysCtl.SysRC[3] = 0;
    SysCtl.SysRC[4] = 0;
    SysCtl.SysRC[5] = 0;
    SysCtl.SysRC[6] = 0;
    SysCtl.SysRC[7] = 0;
    SysCtl.SysRC[8] = 0;
    SysCtl.SysRC[9] = 0;
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
    
    clock->PERIODO = CLOCK_PER;
    
    clock->MIN[0] = CLOCK_MIN_B;
    clock->MIN[1] = CLOCK_MIN_A;
    
    clock->SEC[0] = CLOCK_SEC_B;
    clock->SEC[1] = CLOCK_SEC_A;
    
    clock->DEC    = CLOCK_DEC;
    setTeamA(SysCtl.SysTeamA);//modificacion 1-3
    setTeamB(SysCtl.SysTeamB);//modificacion 1-3
    
    INTCONbits.PEIE_GIEL = 1;
    INTCONbits.GIE_GIEH = 1;
    RCONbits.IPEN = 1;
    IPEN = 1;   //Prioridad de interrupciones activada
    GIE  = 1;   //Activar las interrupciones de alta prioridad
    PEIE = 1;   //Activar las interrupciones de baja prioridad
    TMR1IP = 1; //Timer interrupt de alta prioridad
    T1CON = 0b01001000; //Enable regi r/w in two 8-bit operations TMR1 off
}
//
// Control de interrupciones
//
/* *
 *  Configura el timer1, lo precarga e inicializa.
 */
void startClock(void){
    //T1CON  = 0b00110111;
    //Configuramos registro
    T1CON =  0b00100011;
    //Configuramos registro
    T1GCON = 0b01110010;
    
    //Configurar las interrupciones del timer1
    TMR1IE = 1;//TMR1 interrupt activada
    TMR1IP = 1;//TMR1 interrupt alta prioridad
    
    //Precargamos el timer1
    TMR1 = TMR1_INIT;
}
/* *
 * Desactiva el modulo timer1
 */
void stopClock(void){
    //Detenemos el modulo Timer1
    TMR1ON = 0;
// MOD 1-2
    //Bajar el bit del puertoB
    LATBbits.LATB2 = OFF;
}
/* *
 * Configura el modulo UART.
 * Inicializa los registros.
 * Activa el modulo UART.
 */
void startSerial(void){
    //UART INIT
    //
    //BAUDCON = 0b01000000;
    ABDOVF= 0;
    RCIDL = 0;//Active
    RXDTP = 0; //Rx no inverted
    TXCKP = 0; //Tx no inverted
    BRG16 = 0; //8bit generator
    ABDEN = 0; //Auto Baud Detect OFF
    WUE   = 0; //Receiver operating normally
    SPBRG = 13;
    
    //Activar el puerto UART
    PORTC   = 0x00;
    LATC   = 0x00;
    ADON = 0;
    ANSELC = 0x00;
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;
    
    //TXSTA   = 0b00100100;
    CSRC = 0;
    TX9  = 0;
    TXEN = 1;
    SYNC = 0;
    SENDB= 0;
    BRGH = 0;
    TRMT = 1;    //TSRR empy / full -> 0
    
    //RCSTA   = 0b10010000;
    SPEN = 1;
    RX9  = 0;
    SREN = 0;
    CREN = 1;
    ADDEN= 0;
    FERR = 0;
    OERR = 0;
    
    RCIE = 1;    //Activar interrupcion de recepcion
    RCIP = 0;    //Interrupcion de recep baja prioridad
    
    //
    //UART END
    //
}

//
//  CONTROL DE EVENTOS
//

/* *
 * Evento de alta proridad, Servicio de atencion a la interrupcion.
 * Cuando se detecta una interrupcion del timer1
 * se entiende que han pasado .1 sec desde la ultima interrupcion.
 * Se decrementa en una decima de segundo el cronometro.
 */
void interrupt HIGH_ISR(void){
    //Si es un evento del TMR1
    if(TMR1IF){
        //  PRECARGAMOS EL MODULO
            TMR1 = TMR1_INIT;
        //  LIMPIAMOS LA INTERRUPCION
            TMR1IF = 0;
        //  DESCONTAMOS EL RELOJ
            DecAndShowClock(&Cronometro);
    }
}
/* *
 * Evento de baja proridad, Servicio de atencion a la interrupcion.
 * Cuando se detecta una interrupcion de recepcion por UART
 * se valida la palabra recibida, si es valida se activa una bandera
 * para informar al interprete que puede comenzar a leer y luego se
 * desactiva la recepcion por el modulo UART.
 */
void interrupt low_priority LOW_ISR(void){
    unsigned char trash = RCREG;
    if(trash == 253){
        puts("Reloading system!\n");
        asm("RESET");
    }
    //Si hay espacio aun en el buffer
    if(SysCtl.i<BUFFER_MAX){
        SysCtl.SysRC[ SysCtl.i ] = trash;
        SysCtl.i++;
        if((SysCtl.i-1)%4==0){
            //Si es un paquete de 4 tramas o header
            SysCtl.stack++;//Push
        }
        if(trash == 254){
            //END FLAG
            if(SysCtl.stack>=1){
                SysCtl.stack--;
            }
            if(SysCtl.stack==0){
                SysCtl.SysCtlIF = 0x01;
            }
                // Desactivar el modulo UART
                // RCSTAbits.CREN = 0;
        }
        if(trash == 255){
            // Si es comando de prueba
            if(SysCtl.i == 1){
                puts("Roger that!\n");
            }
            //  Cancelar comando limpiando buffer e interrupcion
            SysCtl.SysCtlIF = 0x00;
            SysCtl.SysRC[0] = 0;
            SysCtl.i = 0;
        }
    }else{
        //
        //Si la memoria se lleno y
        //no se detecto endflag
        //
        if(!SysCtl.SysCtlIF){
            //Limpiamos interrupcion y buffer
            puts("Endflag not detected!\n");
            SysCtl.SysCtlIF = 0x00;
            SysCtl.i = 0;
        //LIMPIAR BUFFER
        }
    }
}

//
//  EVENTOS DE ATENCION A LAS INTERRUPCIONES
//

/* *
 *  Decrementa una decima de segundo el reloj.
 *  Decrementa en uno las decimas de la estructura de datos clock.
 *
 *  @param  clock   Referencia a la estrutura de datos del cronometro
 *  
 *  @return char, un numero 0, 1, 2 o 3, si no requiere decremento,
 *          hubo decremento de decimas, de segundos y decimas
 *          o de decimas, segundos y minutos
 */
unsigned char DecBitClock(Clock* clock){
    //Decrementar una decima de segundo
    // return 0 -> sin decremento
    // 1 -> decremento de decimas
    // 2 -> decremento de segundos
    // 3 -> decremento de minutos
    
//Si el cronometro ha llegado a 0
    if( (clock->Clock_min + 
            clock->Clock_sec +
            clock->Clock_dec) == 0)
    {return 0;}

//Si se deben decrementar las decimas    
    if(clock->Clock_dec > 0){
        clock->Clock_dec--;
        return 1;//Se decrementaron las decimas
    }else{
//Si se deben decrementar las decimas y los segundos
        if(clock->Clock_sec>0){
            setClockDec(clock,9);
            clock->Clock_sec--;
        }else{
//Si se deben decrementar las decimas y los segundos y los minutos
            if(clock->Clock_min>0){
                setClockDec(clock,9);
                setClockSec(clock,59);
                clock->Clock_min--;
            }
            return 3;//Se decrementaron los minutos
        }
        return 2;//Se decrementaron los segundos
    }
}
/* *
 *  Decrementa en una decima de segundos el cronometro
 *  @see DecBitClock, y valida cual segmento actualizar en pantalla.
 */
void DecAndShowClock(Clock* clock){
    switch( DecBitClock(clock) ){
        case 1:
            //Actualizar decimas de segundo
            putClockDec(clock);
            break;
        case 2:
            //Actualizar decimas de segundo y segundos
            putClockDec(clock);
            putClockSec(clock);
            break;
        case 3:
            //Actualizar decimas de segundo, segundos y minutos
            putClockDec(clock);
            putClockSec(clock);
            putClockMin(clock);
            break;
        default:
           //No actualizar nada - Conteo llego a cero
//MOD 1-2   ENCENDER BIT DEL PUERTO B
            LATBbits.LATB2 = ON;
            return;
    }
}
/* *
 *  Retraso del sistema en milisegundos
 *  @param  MILIS   milisegundos de retraso
 */

 void delay(unsigned long MILIS){
    for(unsigned long a=0;a<MILIS;a++){
        __delay_ms(1);
    }
}
/* *
 *  Retraso del sistema en microsegundos
 *  @param  MICROS   microsegundos de retraso
 */
void delay_us(unsigned long MICROS){
    for(unsigned long a=0;a<MICROS;a++){
        __delay_us(1);
    }
}
//
//  Control de reloj
//
/* *
 *  Reemplaza los valores del reloj por otros nuevos.
 * @param   clock   Referencia a la estructura del reloj
 * @param   min     Minutos a asignar
 * @param   sec     Segundos a asignar
 * @param   dec     Decimas a asignar
 */
void setClock(Clock* clock,unsigned char min,unsigned char sec,unsigned char dec){
    //Valor maximo 0xEFB9 -> 61369 = 59 min + 59 sec + 9 dec
    clock->Clock_min = min%60;
    clock->Clock_sec = sec%60;
    clock->Clock_dec = dec%10;
}
/* *
 *  Reemplaza el valor de minutos en la estructura del reloj
 * @param   clock   Referencia a la estructura del reloj
 * @param   min     Minutos a asignar
 */
void setClockMin(Clock* clock,unsigned char min){
    clock->Clock_min = min%60;
}
/* *
 *  Reemplaza el valor de minutos en la estructura del reloj
 * @param   clock   Referencia a la estructura del reloj
 * @param   sec     Segundos a asignar
 */
void setClockSec(Clock* clock,unsigned char sec){
    clock->Clock_sec = sec%60;
}
/* *
 *  Reemplaza el valor de minutos en la estructura del reloj
 * @param   clock   Referencia a la estructura del reloj
 * @param   dec     Decimans a asignar
 */
void setClockDec(Clock* clock,unsigned char dec){
    clock->Clock_dec = dec%10;   //Cargamos las decimas
}
/* *
 *  Reemplaza el valor del periodo en la estructura del reloj
 *  MOD 1-3 RESTAURA LOS PUNTOS DE LOS EQUIPOS
 * @param   clock   Referencia a la estructura del reloj
 * @param   per     Periodo a asignar
 */
void setClockPeriodo(Clock* clock,unsigned char per){
//MOD 1-3
    if(clock->Periodo == per%10){
        return;
    }
    clock->Periodo = per%10;
//MOD 1-3
//  RESTAURAMOS SOLO LOS PUNTOS DE CADA EQUIPO
    
    setTeamFaults(SysCtl.SysTeamA,0);
    setTeamFaults(SysCtl.SysTeamB,0);
}

/* *
 *  Imprime en pantalla los minutos del reloj
 * @param   clock   Referencia a la estructura del reloj
 */
void putClockMin(Clock* clock){
    //Desplegamos solo los minutos
    writeInt(clock->Clock_min
            ,clock->MIN,2);
}
/* *
 *  Imprime en pantalla los segudos del reloj
 * @param   clock   Referencia a la estructura del reloj
 */
void putClockSec(Clock* clock){
    //Desplegamos solo los segundos
    writeInt(clock->Clock_sec
            ,clock->SEC,2);
}
/* *
 *  Imprime en pantalla las decimas del reloj
 * @param   clock   Referencia a la estructura del reloj
 */
void putClockDec(Clock* clock){
    //Desplegamos solo las decimas
    //turnOn( FALSE_SEGMENT );
    turnOn(clock->DEC);
    putInt(clock->Clock_dec);
}
/* *
 *  Imprime en pantalla el periodo del reloj
 * @param   clock   Referencia a la estructura del reloj
 */
void putClockPeriodo(Clock* clock){
    //turnOn( FALSE_SEGMENT );
    turnOn(clock->PERIODO);
    putInt(clock->Periodo);
}

//
//  Control de equipos
//

/* *
 *  Inicializa los valores del equipo A
 *  @param  team    Referencia a la estructura donde
 *                  se almacenan los datos del queipo A
 */
void setTeamA(Team* team){
    team->PTS[0] = TEAM_A_PTS_C;
    team->PTS[1] = TEAM_A_PTS_B;
    team->PTS[2] = TEAM_A_PTS_A;
    team->FAULTS[0] = TEAM_A_FLTS_B;
    team->FAULTS[1] = TEAM_A_FLTS_A;
    team->TEMP_PLAYER[0] = TEAM_A_TMP_PLAYER_B;
    team->TEMP_PLAYER[1] = TEAM_A_TMP_PLAYER_A;
    team->TEMP_FAULT  = TEAM_A_TMP_FLT;
    team->PTR = TEAM_A_PTR;
    team->Pts           = 0;
    team->Flts          = 0;
    team->temp_Player   = 0;
    team->temp_Flt      = 0;
}
/* *
 *  Inicializa los valores del equipo B
 *  @param  team    Referencia a la estructura donde
 *                  se almacenan los datos del queipo B
 */
void setTeamB(Team* team){
    team->PTS[0] = TEAM_B_PTS_C;
    team->PTS[1] = TEAM_B_PTS_B;
    team->PTS[2] = TEAM_B_PTS_A;
    team->FAULTS[0] = TEAM_B_FLTS_B;
    team->FAULTS[1] = TEAM_B_FLTS_A;
    team->TEMP_PLAYER[0] = TEAM_B_TMP_PLAYER_B;
    team->TEMP_PLAYER[1] = TEAM_B_TMP_PLAYER_A;
    team->TEMP_FAULT  = TEAM_B_TMP_FLT;
    team->PTR = TEAM_B_PTR;
    team->Pts           = 0;
    team->Flts          = 0;
    team->temp_Player   = 0;
    team->temp_Flt      = 0;    
}
/* *
 * Valida y Escribe los puntos en la estructura de datos del equipo
 * @param   team    Referencia a la estructura de destino
 * @param   pts     Puntos que asignar
 */
void setTeamPts(Team* team,unsigned long pts){
    if(pts <= 999){
        team->Pts = pts;
    }
}
/* *
 * Valida y Escribe las faltas en la estructura de datos del equipo
 * @param   team      Referencia a la estructura de destino
 * @param   fault     Faltas que asignar
 */
void setTeamFaults(Team* team,unsigned char faults){
    if(faults <= 99){
        team->Flts = faults;
    }
}
/* *
 * Valida y Escribe las faltas compuestas en la estructura de datos del equipo
 * @param   team    Referencia a la estructura de destino
 * @param   player  Jugador que cometio la falta
 * @param   fault   Falta cometida por el jugador
 */
void addTeamFault(Team* team,unsigned char player,unsigned char fault){
    team->temp_Player = player;
    team->temp_Flt = fault;
    //team->Flts += fault; MODIFICACION 1-1
}
/* *
 *  Imprime en pantalla los puntos del equipo
 * @param   team    Referencia al equipo.
 */
void putTeamPts(Team* team){
    writeInt(team->Pts,team->PTS,3);
}
/* *
 *  Enciende el apuntador de saque del equipo.
 *  @param   team    Referencia al equipo.
 */
void turnOnTeamPtr(Team* team){
    // Puerto B enciende el bit
    //  correspondiente apagando los demas.
//MOD 1-2
    if(team->PTR-1){//Equipo B 2-1 = 1
        LATBbits.LATB0 = OFF;
        LATBbits.LATB1 = ON;
    }else{//Equipo A 1-1 = 0
        LATBbits.LATB0 = ON;
        LATBbits.LATB1 = OFF;
    }
}
/* *
 *  Apaga los apuntadores de saque
 *
 */
void turnOffTeamPtr(void){
    LATBbits.LATB0 = OFF;
    LATBbits.LATB1 = OFF;
}
/* *
 * Imprime en pantalla las faltas del equipo
 * @param   team    Referencia al equipo
 */
void putTeamFaults(Team* team){
    //Imprimir faltas
    writeInt(team->Flts,team->FAULTS,2);
}
/* *
 * Imprime en pantalla la falta compuesta del equipo.
 * Jugador y falta.
 * @param   team    Referencia al equipo
 */
void putTeamFault(Team* team){
    //Imprimir jugador
    writeInt(team->temp_Player,team->TEMP_PLAYER,2);
    //Imprimir falta
    writeInt(team->temp_Flt,&team->TEMP_FAULT,1);
}
//
//  Funciones extra
//
/* *
 *  Envia una palabra de 8bits por el puerto serie.
 *  @param  c   Palabra de 8 bits
 */
void putch(unsigned char c){
    TXREG = c;
    delay(2);
}
/* *
 *  Envia un arreglo de palabras de 8bits por el puerto serie.
 *  @param  str   Referencia a la primer palabra del arreglo.
 */
void puts(const char* str){
    while(*str){
        TXREG = *str;
        delay(2);
        str++;
    }
}
/**
 * Envia bytes en formato ascii
 * @param  data 2 bytes en forma de short int
 */
void putInASCII(unsigned short data){
    unsigned const char Hex_To_ASCII[16] = {'0','1','2','3',\
                         '4','5','6','7',\
                         '8','9','A','B',\
                         'C','D','E','F'};
    //Enviamos los bits mas segnificativos en formato hex ascii
    putch( Hex_To_ASCII[(data>>4) & 0xF]);
    //Enviamos los bits menos significativos hex ascii
    putch( Hex_To_ASCII[data & 0xF] );
    //Enviamos caracter de fin de linea
    putch('\n');
}
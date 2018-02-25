/* *
 *  Definiciones de funciones del sistema de control
 * @autor Josepablo Cruz Baas
 */
#ifndef CONTROL_H
#define	CONTROL_H
//
//Local A - Visitante B
//
#include "Configuracion.h"

//TMR1 -> FOSC/4/8
//TMR1 -> FOSC/4/4
#define TMR1_INIT       15536//40535    //
//BAUDCON -> baud = FOSC/[64 (n+1)] -> n = [FOSC/(baud(64))]-1
#define BAUDCON_CONFIG    22      //
#define BUFFER_MAX        20
//<editor-fold defaultstate="collapsed" desc="Direcciones para el selector">
#define FALSE_SEGMENT     24
#define ON      1
#define OFF     0

//CRONOMETRO
#define CLOCK_MIN_A     0//Decenas
#define CLOCK_MIN_B     1//Unidades
#define CLOCK_SEC_A     2//Decenas
#define CLOCK_SEC_B     3//Unidades
#define CLOCK_DEC       4//Unidades
#define CLOCK_PER       5//Unidades
//EQUIPO A
#define TEAM_A_PTS_A            6//Centenas
#define TEAM_A_PTS_B            7//Decenas
#define TEAM_A_PTS_C            8//Unidades
#define TEAM_A_FLTS_A           9//Decenas
#define TEAM_A_FLTS_B           10//Unidades
#define TEAM_A_TMP_PLAYER_A     11//Decenas
#define TEAM_A_TMP_PLAYER_B     12//Unidades
#define TEAM_A_TMP_FLT          13
    //Corresponde al puerto B
    #define TEAM_A_PTR              0x01
//EQUIPO B
#define TEAM_B_PTS_A            14
#define TEAM_B_PTS_B            15
#define TEAM_B_PTS_C            16
#define TEAM_B_FLTS_A           17
#define TEAM_B_FLTS_B           18
#define TEAM_B_TMP_PLAYER_A     19
#define TEAM_B_TMP_PLAYER_B     20
#define TEAM_B_TMP_FLT          21
    //Corresponde al puerto B
    #define TEAM_B_PTR              0x02
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc="Macros">
#define SetPorts(){ANSELA=0x00;ANSELD = 0x00;ADCON1=0x0F;PORTA=0;PORTB=0x00;TRISA=0x000; \
                        TRISB=0x00;TRISD=0;LATA=0x1F;LATB=0x0;LATD=0;}
#define GPIO_PORTA_WRITE(DATA)  {LATA=DATA;}
#define GPIO_PORTD_WRITE(DATA)  {LATD=DATA;}
//</editor-fold>

//<editor-fold defaultstate="collapsed" desc="Estructuras de control y datos">
volatile unsigned char num[10]={
    0b1110111,//0
    0b0010010,//1
    0b0111101,//2
    0b0111011,//3
    0b1011010,//4
    0b1101011,//5
    0b1101111,//6
    0b0010011,//7
    0b1111111,//8
    0b1011011//9
    };
struct CLOCK{
    /*
     * 
     * DIRECCIONES DE SEGMENTOS
     * 
     * */
    unsigned char PERIODO;
    unsigned char MIN[2];
    unsigned char SEC[2];
    unsigned char DEC;
    /*
     * DATOS
     * */
    unsigned short int Clock_min;
    unsigned short int Clock_sec;
    unsigned short int Clock_dec;
    unsigned char    Periodo;
};
struct TEAM{
    /*
     *  DIRECCIONES DE SEGMENTOS
     * */
    unsigned char PTS[3];
    unsigned char FAULTS[2];
    unsigned char TEMP_PLAYER[2];
    unsigned char TEMP_FAULT;
    unsigned char PTR;
    /*
     * DATOS
     * */
    unsigned long Pts;
    unsigned char Flts;
    unsigned char temp_Player;
    unsigned char temp_Flt;
};
struct CONTROL{
    struct CLOCK *SysCtlClock;
    struct TEAM  *SysTeamA;
    struct TEAM  *SysTeamB;
    unsigned char SysCtlIF;
    unsigned char SysRC[BUFFER_MAX];
    unsigned char i;
    unsigned char stack;
};

typedef struct CLOCK    Clock;
typedef struct TEAM     Team;
typedef struct CONTROL  Control;
//</editor-fold>


Control   SysCtl;
Clock Cronometro;
Team TeamA,TeamB;

/*
 *  CONTROL DEL PUERTO
 * 
 * */
void putInt(unsigned char);
void turnOn(unsigned char);
void writeInt(unsigned short,unsigned const char[],unsigned char);
void Show();
/*
 * SISTEMA DE CONTROL
 * 
 * */
void ConfigClockAndInt(void);
void resetBluetooth(void);
void configSysCtl(Team*,Team*,Clock*);

void startClock(void);
void stopClock(void);
void startSerial(void);
void stopSerial(void);

void interrupt HIGH_ISR(void);
void interrupt low_priority LOW_ISR(void);

//void ClockEvent(void);
void SerialEvent(void);


unsigned char DecBitClock(Clock*);
void DecPeriodo(Clock*);
void addPeriodo(Clock*);
void DecAndShowClock(Clock*);
void setTeamA(Team*);
void setTeamB(Team*);

void delay(unsigned long);
void delay_us(unsigned long);


/*
 *  CONTROL DEL RELOJ
 * 
 * */
void setClock(Clock*,unsigned char,unsigned char,unsigned char);
void setClockMin(Clock*,unsigned char);
void setClockSec(Clock*,unsigned char);
void setClockDec(Clock*,unsigned char);
void setClockPeriodo(Clock*,unsigned char);
void putClockMin(Clock*);
void putClockSec(Clock*);
void putClockDec(Clock*);
void putClockPeriodo(Clock*);

/*
 *  CONTROL DE EQUIPOS
 * 
 * */
void setTeamPts(Team*,unsigned long);
void setTeamFaults(Team*,unsigned char);
void addTeamFault(Team*,unsigned char,unsigned char);
void putTeamPts(Team*);
void turnOnTeamPtr(Team*);
void turnOffTeamPtr(void);
void putTeamFaults(Team*);
void putTeamFault(Team*);

void putch(const char);
void puts(const char*);
void putInASCII(unsigned short);

#endif
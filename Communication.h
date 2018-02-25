#ifndef COMMUNICATION_H
#define COMMUNICATION_H
#include "Control.h"
#include <stdlib.h>

void ExitEvent(void);
void SerialEvent(void);
//
//Control de Estados
//
void CDE(unsigned char);
void CDE_Pts(Team*,unsigned char);
void CDE_Flts(Team*,unsigned char);
void CDE_PnF(Team*,unsigned char);
void CDE_Ptr(Team*,unsigned char);
//
//Control de relog
//
void CDR(unsigned char);
void CDR_ALL(Clock*,unsigned char);
void CDR_Min(Clock*,unsigned char);
void CDR_Sec(Clock*,unsigned char);
void CDR_Dec(Clock*,unsigned char);
void CDR_Per(Clock*,unsigned char);
//
//Control de sistema
//
void CDS(unsigned char);
void CDS_Default(void);
void CDS_Start(void);
void CDS_Pause(void);
void CDS_Reset(void);
void CDS_Echo(void);
//
//  Control de flujo de datos
//
//unsigned long get(unsigned char);
unsigned long get4(unsigned char);

#endif

#ifndef _MOTOROLA_H_DEFINED_
#define _MOTOROLA_H_DEFINED_

#include <stdio.h>
#include "wintypes.h"

extern short ReadMotoralaShort(FILE* fp);
extern int ReadMotoralaInt(FILE* fp);
extern long ReadMotoralaLong(FILE* fp);
extern float ReadMotoralaFloat(FILE* fp);
extern double ReadMotoralaDouble(FILE* fp);
extern WORD ReadMotoralaWord(FILE* fp);
extern DWORD ReadMotoralaDWord(FILE* fp);
extern QWORD ReadMotoralaQWord(FILE* fp);

extern void WriteMotoralaShort(FILE* fp, short n);
extern void WriteMotoralaInt(FILE* fp, int n);
extern void WriteMotoralaLong(FILE* fp, long n);
extern void WriteMotoralaFloat(FILE* fp, float n);
extern void WriteMotoralaDouble(FILE* fp, double n);
extern void WriteMotoralaWord(FILE* fp, WORD n);
extern void WriteMotoralaDWord(FILE* fp, DWORD n);
extern void WriteMotoralaQWord(FILE* fp, QWORD n);


#endif // !_MOTOROLA_H_DEFINED_

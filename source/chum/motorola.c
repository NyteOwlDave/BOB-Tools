
/*

    motorola.c

    File i/o in motorola order

*/

#include "inc/stdafx.h"
#include "inc/motorola.h"

short ReadMotoralaShort(FILE* fp)
{
    return (short)ReadMotoralaWord(fp);
}

int ReadMotoralaInt(FILE* fp)
{
    return (int)ReadMotoralaDWord(fp);
}

long ReadMotoralaLong(FILE* fp)
{
    return (long)ReadMotoralaQWord(fp);
}

float ReadMotoralaFloat(FILE* fp)
{
    DWORD n = ReadMotoralaDWord(fp);
    return *((float *)&n);
}

double ReadMotoralaDouble(FILE* fp)
{
    QWORD n = ReadMotoralaQWord(fp);
    return *((double *)&n);
}

WORD ReadMotoralaWord(FILE* fp)
{
    WORD result;
    BYTE *p = (BYTE*)&result;
    p[1] = fgetc(fp);
    p[0] = fgetc(fp);
    return result;
}

DWORD ReadMotoralaDWord(FILE* fp)
{
    DWORD result;
    BYTE *p = (BYTE*)&result;
    p[3] = fgetc(fp);
    p[2] = fgetc(fp);
    p[1] = fgetc(fp);
    p[0] = fgetc(fp);
    return result;
}

QWORD ReadMotoralaQWord(FILE* fp)
{
    QWORD result;
    BYTE *p = (BYTE*)&result;
    p[7] = fgetc(fp);
    p[6] = fgetc(fp);
    p[5] = fgetc(fp);
    p[4] = fgetc(fp);
    p[3] = fgetc(fp);
    p[2] = fgetc(fp);
    p[1] = fgetc(fp);
    p[0] = fgetc(fp);
    return result;
}

void WriteMotoralaShort(FILE* fp, short n)
{
    BYTE* p = (BYTE*)&n;
    fputc(p[1], fp);
    fputc(p[0], fp);
}

void WriteMotoralaInt(FILE* fp, int n)
{
    BYTE* p = (BYTE*)&n;
    fputc(p[3], fp);
    fputc(p[2], fp);
    fputc(p[1], fp);
    fputc(p[0], fp);
}

void WriteMotoralaLong(FILE* fp, long n)
{
    BYTE* p = (BYTE*)&n;
    fputc(p[7], fp);
    fputc(p[6], fp);
    fputc(p[5], fp);
    fputc(p[4], fp);
    fputc(p[3], fp);
    fputc(p[2], fp);
    fputc(p[1], fp);
    fputc(p[0], fp);
}

void WriteMotoralaFloat(FILE* fp, float n)
{
    BYTE* p = (BYTE*)&n;
    fputc(p[3], fp);
    fputc(p[2], fp);
    fputc(p[1], fp);
    fputc(p[0], fp);
}

void WriteMotoralaDouble(FILE* fp, double n)
{
    BYTE* p = (BYTE*)&n;
    fputc(p[7], fp);
    fputc(p[6], fp);
    fputc(p[5], fp);
    fputc(p[4], fp);
    fputc(p[3], fp);
    fputc(p[2], fp);
    fputc(p[1], fp);
    fputc(p[0], fp);

}

void WriteMotoralaWord(FILE* fp, WORD n)
{
    BYTE* p = (BYTE*)&n;
    fputc(p[1], fp);
    fputc(p[0], fp);
}

void WriteMotoralaDWord(FILE* fp, DWORD n)
{
    BYTE* p = (BYTE*)&n;
    fputc(p[3], fp);
    fputc(p[2], fp);
    fputc(p[1], fp);
    fputc(p[0], fp);
}


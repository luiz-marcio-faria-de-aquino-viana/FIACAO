
/*
/* FILUTIL.cc
/* Copyright (C) 1996 by Luiz Marcio F A Viana, 7/10/96
*/

#ifndef __FILUTIL_CC
#define __FILUTIL_CC

#include<string.h>
#include"strutil.cc"

/* fileReg_t: estrutura de dados dos elementos de entrada na forma do registro
*/
typedef struct fileReg {
  char hnd[17];
  char hnd1[17];
  char idx1[2];
  char tip1[12];
  char qdr1[21];
  char org1[21];
  char des1[21];
  char cir1[4];
  char cmd1[31];
  char fas1[7];
  char hnd2[17];
  char idx2[2];
  char tip2[12];
  char qdr2[21];
  char org2[21];
  char des2[21];
  char cir2[4];
  char cmd2[31];
  char fas2[7];
} fileReg_t;

/* tempReg_t: estrutura de dados dos elementos de entrada na forma do registro
*/
typedef struct tempReg {
  char hnd[17];
  int  idx;
  int  tip;
  char qdr[21];
  char org[21];
  char des[21];
  char cir[4];
  char cmd[31];
  int  fas;
} tempReg_t;

/* declaracao da rotina de conversao da string de entrada em registro
*/
// str2fileReg: rotina de conversao da string de entrada em registro
//  s   - string de entrada
//  reg - registro de entrada
void str2fileReg(char *s, fileReg_t *reg);

/* implementacao da rotina de conversao da string de entrada em registro
*/
void str2fileReg(char *s, fileReg_t *reg) {
  str_t s1;

  /* caracteristicas do eletroduto */
  strpiece(s1, s, 1, '@');   // handle do eletroduto
  strncpy(reg->hnd, s1, 16);
  reg->hnd[16] = '\0';

  /* caracteristicas do primeiro ponto conectado ao eletroduto */
  strpiece(s1, s, 1, '@');   // handle
  strncpy(reg->hnd1, s1, 16);
  reg->hnd1[16] = '\0';
  strpiece(s1, s, 1, '@');   // indice
  strncpy(reg->idx1, s1, 1);
  reg->idx1[1] = '\0';
  strpiece(s1, s, 1, '@');   // tipo de elemento
  strncpy(reg->tip1, s1, 11);
  reg->tip1[11] = '\0';
  strpiece(s1, s, 1, '@');   // nome do quadro (se tipo = EQUADRO)
  strncpy(reg->qdr1, s1, 20);
  reg->qdr1[20] = '\0';
  strpiece(s1, s, 1, '@');   // quadro de origem
  strncpy(reg->org1, s1, 20);
  reg->org1[20] = '\0';
  strpiece(s1, s, 1, '@');   // identificacao do desvio (se tipo = EDESVIO)
  strncpy(reg->des1, s1, 20);
  reg->des1[20] = '\0';
  strpiece(s1, s, 1, '@');   // identificacao do circuito (se tipo = ECARGA)
  strncpy(reg->cir1, s1, 3);
  reg->cir1[3] = '\0';
  strpiece(s1, s, 1, '@');   // identificacao dos comandos (se tipo != EQUADRO)
  strncpy(reg->cmd1, s1, 30);
  reg->cmd1[30] = '\0';
  strpiece(s1, s, 1, '@');   // sistema de fases
  strncpy(reg->fas1, s1, 6);
  reg->fas1[6] = '\0';

  /* caracteristicas do segundo ponto conectado ao eletroduto */
  strpiece(s1, s, 1, '@');   // handle
  strncpy(reg->hnd2, s1, 16);
  reg->hnd2[16] = '\0';
  strpiece(s1, s, 1, '@');   // indice
  strncpy(reg->idx2, s1, 1);
  reg->idx2[1] = '\0';
  strpiece(s1, s, 1, '@');   // tipo de elemento
  strncpy(reg->tip2, s1, 11);
  reg->tip2[11] = '\0';
  strpiece(s1, s, 1, '@');   // nome do quadro (se tipo = EQUADRO)
  strncpy(reg->qdr2, s1, 20);
  reg->qdr2[20] = '\0';
  strpiece(s1, s, 1, '@');   // quadro de origem
  strncpy(reg->org2, s1, 20);
  reg->org2[20] = '\0';
  strpiece(s1, s, 1, '@');   // identificacao do circuito (se tipo = EDESVIO)
  strncpy(reg->des2, s1, 20);
  reg->des2[20] = '\0';
  strpiece(s1, s, 1, '@');   // identificacao do circuito (se tipo = ECARGA)
  strncpy(reg->cir2, s1, 3);
  reg->cir2[3] = '\0';
  strpiece(s1, s, 1, '@');   // identificacao dos comandos (se tipo != EQUADRO)
  strncpy(reg->cmd2, s1, 30);
  reg->cmd2[30] = '\0';
  strpiece(s1, s, 1, '@');   // sistema de fases
  strncpy(reg->fas2, s1, 6);
  reg->fas2[6] = '\0';
}

#endif  /* __FILUTIL_CC */

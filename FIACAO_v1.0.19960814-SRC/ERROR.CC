
/*
/* ERROR.cc
/* Copyright (C) 1996 by Luiz Marcio F A Viana, 7/8/96
*/

#ifndef __ERROR_CC
#define __ERROR_CC

#include<stdio.h>
#include<stdlib.h>

/* declaracao das constantes para controle de erro nos dados de entrada
*/
enum {
  F_SUCCESS,
  F_WARNING,
  F_FATALERROR
};

/* declaracao da tabela de descricao de erros
*/
char *ERR[] = {
  "Nao foi possivel alocar memoria",
  "Nao foi possivel abrir o arquivo",
  "Numero de parametros invalido",
  "Dados inconcistentes",
  "Nao foi possivel escrever no arquivo",
  "Falta algum parametro excencial"
};

/* declaracao dos identificadores de erros
*/
enum {
  ERR_ALLOCMEM,
  ERR_OPENFILE,
  ERR_NUMPARM,
  ERR_INVALIDATA,
  ERR_CANTWRITE,
  ERR_NOVALIDPARM
};

/* declaracao da variavel global de gerenciamento de erros
*/
int _err;

/* declaracao das rotinas de gerenciamento de erros
*/
void errmsg(int lin, int err);
void errset(int val);

/* implementacao das rotinas de gerenciamento de erros
*/
void errmsg(int lin, int err) {
  printf("ERR(%02d): %s.\n", lin, ERR[err]);
  printf("Tchau!\n");
  exit(err);
}

void errset(int val)
{
  if(val > _err)
    _err = val;
}

#endif  /* __ERROR_C */

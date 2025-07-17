
/*
/* PARMUTIL.cc
/* Copyright (C) 1996 by Luiz Marcio Faria Viana, 9/4/96
*/

#ifndef __PARMUTIL_CC
#define __PARMUTIL_CC

#include<alloc.h>
#include<string.h>
#include<dos.h>
#include"fiacao.h"
#include"error.cc"

/* definicao da macro que fornece o comprimento de uma string
*/
#define SZ(t) (strlen(t))

/* definicao dos identificadores dos tipos de parametros
*/
#define PARM_SOURCEFILE "/F="
#define PARM_TARGETFILE "/O="
#define PARM_DEBUGMODE  "/D"

/* estrutura de dados dos parametros de entrada
*/
typedef struct tagParm {
  char *sourceFile;
  char *targetFile; 
  int  debugMode;
} Parm_t;

/* declaracao da rotina para obtencao dos parametros de entrada
*/
void getparm(Parm_t *parm);

/* implementacao da rotina para obtencao dos parametros de entrada
*/
void getparm(Parm_t *parm)
{
  int i;
  if(_argc <= 1) errmsg(43, ERR_NUMPARM);

  parm->sourceFile = NULL;
  parm->targetFile = NULL;
  parm->debugMode = 0;

  for(i = 0; i < _argc; i++) {
    strupr(_argv[i]);
    if(!strncmp(PARM_SOURCEFILE, _argv[i], SZ(PARM_SOURCEFILE))) {
      if((parm->sourceFile = (char *) malloc(SZ(_argv[i]))) == NULL)
	errmsg(41, ERR_ALLOCMEM);
      strcpy(parm->sourceFile, & _argv[i][SZ(PARM_SOURCEFILE)]);
    }
    if(!strncmp(PARM_TARGETFILE, _argv[i], SZ(PARM_TARGETFILE))) {
      if((parm->targetFile = (char *) malloc(SZ(_argv[i]))) == NULL)
        errmsg(56, ERR_ALLOCMEM);
      strcpy(parm->targetFile, & _argv[i][SZ(PARM_TARGETFILE)]);
    }
    if(!strncmp(PARM_DEBUGMODE, _argv[i], SZ(PARM_DEBUGMODE))) {
      parm->debugMode = -1;
    }
  }
  if(parm->sourceFile == NULL)
    errmsg(40, ERR_NOVALIDPARM);
}

#endif __PARMUTIL_CC

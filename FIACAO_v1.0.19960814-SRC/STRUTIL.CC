
/*
/* STRUTIL.cc
/* Copyright (C) 1996 by Luiz Marcio F A Viana, 7/10/96
*/

#ifndef __STRUTIL_CC
#define __STRUTIL_CC

#include<string.h>

typedef char str_t [256];

/* declaracao das rotinas de manipulacao de subcadeias de strings
*/
// strhead: funcao que retorna o tamanho da cadeia (dst) formada pelos
// elementos mais a esquerda da primeira ocorrencia do delimitador (d)
//  dst - string que recebera o valor da cadeia (destino)
//  src - string que sera analisada (fonte)
//  d   - caracter delimitador da cadeia
int strhead(char *dst, char *src, char d);

// strtail: funcao que retorna o tamanho da cadeia (dst) formada pelos
// elementos mais a direita da primeira ocorrencia do delimitador (d)
//  dst - string que recebera o valor da cadeia (destino)
//  src - string que sera analisada (fonte)
//  d   - caracter delimitador da cadeia
int strtail(char *dst, char *src, char d);

// strpiece: funcao que retorna o tamanho da cadeia (dst) formada pelos
// elementos contidos entre a ocorrencias (n-1) e (n) do delimitador (d)
//  dst - string que recebera o valor da cadeia (destino)
//  src - string que sera analisada (fonte)
//  n   - numero de ocorrencias do delimitador
//  d   - caracter delimitador da cadeia
int strpiece(char *dst, char *src, size_t n, char d);

/* implementacao das rotinas de manipulacao de subcadeias de strings
*/
int strhead(char *dst, char *src, char d) {
  char *p;
  size_t n = 0;
  p = src;
  while((*p != '\0') && (*p != d)) {
    *dst++ = *p++;
    n += 1;
  }
  *dst = '\0';
  return n;
}

int strtail(char *dst, char *src, char d) {
  char *p;
  size_t n;
  p = src;
  while(*p != '\0') {
    if(*p++ == d) break;
  }
  strcpy(dst, p);
  return strlen(dst);
}

int strpiece(char *dst, char *src, size_t n, char d) {
  size_t len = 0;
  *dst = '\0';
  while(n > 0) {
    len = strhead(dst, src, d);
    strtail(src, src, d);
    n -= 1;
  }
  return len;
}

#endif  /* __STRUTIL_CC */

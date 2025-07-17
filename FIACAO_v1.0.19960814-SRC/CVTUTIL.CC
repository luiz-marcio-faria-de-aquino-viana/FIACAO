
/*
/* CVTUTIL.cc
/* Copyright (C) 1996 by Luiz Marcio F A Viana, 7/10/96
*/

#ifndef __CVTUTIL_CC
#define __CVTUTIL_CC

#include<string.h>

/* declaracao das rotinas matematicas auxiliares
*/
// pow16: funcao que retorna o valor de 16 elevado n-esima potencia
//  n - valor do expoente desejado
long pow16(long n);

/* declaracao das rotinas de convercao de dados
*/
// hex: funcao que converte uma string hexadecimal em um numero
//  s - string a ser convertida
long hex(char *s);

/* implementacao das rotinas matematicas auxiliares
*/
long pow16(long n)
{
  long mult = 1;
  while(n > 0) {
    mult = mult * 16;
    n -= 1;
  }
  return(mult);
}

/* implementacao das rotinas de conversao de dados
*/

long hex(char *s)
{
  int idx, max;
  long num = 0;

  max = strlen(s);
  for(idx = 0; idx < max; idx++)
    if( (s[idx] >= '0') && (s[idx] <= '9') )
      num += (s[idx] - '0') * pow16((max - idx) - 1);
    else if( (s[idx] >= 'A') && (s[idx] <= 'F') )
	   num += (s[idx] - 'A' + 10) * pow16((max - idx) - 1);
	 else break;

  return num;
}

#endif  /* __CVTUTIL_CC */

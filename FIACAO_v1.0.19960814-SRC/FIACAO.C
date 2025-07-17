
/*
/* FIACAO.c
/* Copyright (C) 1996 by Luiz Marcio F A Viana, 8/14/96
*/

#include<stdio.h>
#include<stdlib.h>
#include<conio.h>
#include<alloc.h>
#include<string.h>
#include"fiacao.h"
#include"error.cc"
#include"parmutil.cc"
#include"cvtutil.cc"
#include"strutil.cc"
#include"filutil.cc"

#define STRSZ(s) (strlen(s) + 1)

/* estrutura de dados do item da lista de handles de elementos
*/
struct itemHnd {
  char *hnd;
  int  idx;
  struct itemHnd *prox;
};

typedef struct itemHnd itemHnd_t;

/* estrutura de dados do item da lista de fios
*/
struct itemFios {
  char *cirOrCmd;
  int  fios;
  struct itemFios *prox;
};

typedef struct itemFios itemFios_t;

/* estrutura de dados do item da lista de quadros
*/
struct itemQdr {
  char *hnd;
  int  idx;
  char *qdr;
  char *org;
  char *cir;
  int  fas;
  struct itemQdr *prox;
};

typedef struct itemQdr itemQdr_t;

/* estrutura de dados do item da lista de eletrodutos
*/
struct itemEletr {
  int  flg;
  char *hnd;
  char *hnd1;
  char *hnd2;
  itemFios_t *primFios;       // ponteiro para o topo da lista de fios
  struct itemEletr *prox;
};

typedef struct itemEletr itemEletr_t;

/* estrutura de dados do item da lista de elementos do comando
*/
struct itemElemCmd {
  char *hnd;
  int  idx;
  int  itip;
  struct itemElemCmd *prox;
};

typedef struct itemElemCmd itemElemCmd_t;

/* estrutura de dados do item da lista de comandos
*/
struct itemCmd {
  char *cmd;
  char *cir;
  int  fas;
  itemElemCmd_t *primElemCmd; // identifica os elementos deste comando
			      // interruptores three-ways e four-ways
  struct itemCmd *prox;
};

typedef struct itemCmd itemCmd_t;

/* estrutura de dados do item da lista de comandos do elemento
*/
struct itemCmdElem {
  itemCmd_t *pCmd;
  struct itemCmdElem *prox;
};

typedef struct itemCmdElem itemCmdElem_t;

/* estrutura de dados do item da lista de elementos
*/
struct itemElem {
  char *hnd;
  int  idx;
  int  tip;
  char *org;
  char *des;
  char *cir;
  itemCmdElem_t *primCmdElem;    // ponteiro para o topo da lista de
				 // comandos do elemento
  int fas;
  struct itemElem *prox;
};

typedef struct itemElem itemElem_t;

/* estrutura de dados do item da lista de elementos de desvios
*/
struct itemElemDes {
  int flg;
  itemElem_t *pElem;
  struct itemElemDes *prox;
};

typedef struct itemElemDes itemElemDes_t;

/* estrutura de dados do item da lista de desvios
*/
struct itemDes {
  int flg;
  char *des;
  itemElemDes_t *primElemDes;
  struct itemDes *prox;
};

typedef struct itemDes itemDes_t;

/* estrutura de dados do item da arvore da malha eletrica
*/
struct itemMalha {
  itemElem_t  *pElem;
  itemEletr_t *pEletr;
  struct itemMalha *primMalha;
  struct itemMalha *prox;
};

typedef struct itemMalha itemMalha_t;

/* estrutura de dados do item da pilha de eletrodutos
*/
struct itemPilha {
  itemEletr_t *pEletr;
  struct itemPilha *prox;
};

typedef struct itemPilha itemPilha_t;

/* declaracao dos ponteiros para topo das listas
*/
itemHnd_t *primHnd = NULL;       // ponteiro para o topo da lista de handles
itemQdr_t *primQdr = NULL;       // ponteiro para o topo da lista de quadros
itemEletr_t *primEletr = NULL;   // ponteiro para o topo da lista de eletrodutos
itemElem_t * primElem = NULL;    // ponteiro para o topo da lista de elementos
itemCmd_t *primCmd = NULL;       // ponteiro para o topo da lista de comandos
itemDes_t *primDes = NULL;       // ponteiro para o topo da lista de desvios
itemMalha_t *primMalha = NULL;   // ponteiro para o topo da arvore da malha eletrica

itemPilha_t *topoPilha = NULL;   // ponteiro para o topo da pilha de eletrodutos

/* declaracao das funcoes de manipulacao das listas de handles
*/
itemHnd_t* lhnd_insereItem(itemHnd_t item);
int  lhnd_procuraItem(char* hnd, int idx, itemHnd_t **iItem);
void lhnd_desaloca(void);

/* declaracao das funcoes de manipulacao das listas de fios
*/
itemFios_t* lfios_insereItem(itemFios_t **primFios, itemFios_t item);
int  lfios_procuraItem(itemFios_t **primFios, char *cirOrCmd, itemFios_t **iItem);
void lfios_desaloca(itemFios_t **primFios);

/* declaracao das funcoes de manipulacao da lista de quadros
*/
itemQdr_t* lqdr_insereItem(itemQdr_t item);
int  lqdr_procuraItem(char *hnd, int idx, itemQdr_t **iItem);
void lqdr_desaloca(void);

/* declaracao das funcoes de manipulacao da lista de eletrodutos
*/
itemEletr_t* leltr_insereItem(itemEletr_t item);
int  leltr_procuraItem(char *hnd, itemEletr_t **iItem);
void leltr_limpaSinalizadores(void);
void leltr_desaloca(void);

/* declaracao das funcoes de manipulacao da lista dos elementos do comando
*/
itemElemCmd_t* lecmd_insereItem(itemElemCmd_t **primElemCmd, itemElemCmd_t item);
int  lecmd_procuraItem(itemElemCmd_t **primElemCmd, char *hnd, int idx, itemElemCmd_t **iItem);
void lecmd_desaloca(itemElemCmd_t **primElemCmd);

/* declaracao das funcoes de manipulacao das listas de comandos
*/
itemCmd_t* lcmd_insereItem(itemCmd_t item);
itemCmd_t* lcmd_alteraItem(itemCmd_t item);
int  lcmd_procuraItem(char *cmd, itemCmd_t **iItem);
void lcmd_desaloca(void);

/* declaracao das funcoes de manipulacao das listas de comandos do elemento
*/
itemCmdElem_t* lcmde_insereItem(itemCmdElem_t **primCmdElem, itemCmdElem_t item);
int  lcmde_procuraItem(itemCmdElem_t **primCmdElem, char *cmd, itemCmdElem_t **iItem);
void lcmde_desaloca(itemCmdElem_t **primCmdElem);

/* declaracao das funcoes de manipulacao da lista de desvios
*/
itemDes_t* ldes_insereItem(itemDes_t item);
int ldes_procuraItem(char *des, itemDes_t **iItem);
void ldes_limpaSinalizadores(void);
void ldes_desaloca(void);

/* declaracao das funcoes de manipulacao da lista de elementos de desvio
*/
itemElemDes_t* ldese_insereItem(itemElemDes_t** primElemDes, itemElemDes_t item);
int ldese_procuraItem(itemElemDes_t** primElemDes, char *hnd, int idx, itemElemDes_t** iItem);
void ldese_desaloca(itemElemDes_t** primElemDes);

/* declaracao das funcoes de manipulacao da lista de elementos
*/
itemElem_t* lelem_insereItem(itemElem_t item);
int  lelem_procuraItem(char *hnd, int idx, itemElem_t **iItem);
void lelem_desaloca(void);

/* declaracao das funcoes de manipulacao da arvore de malha eletrica
*/
itemMalha_t* lmlh_insereItem(itemMalha_t **primMalha, itemMalha_t item);
void lmlh_desaloca(itemMalha_t **primMalha);

/* declaracao das funcoes de manipulacao da pilha de eletrodutos
*/
itemPilha_t* pilha_insereItem(itemEletr_t *pEletr);
itemEletr_t* pilha_retiraItem(void);
void pilha_desaloca(void);

/* declaracao da funcao para desalocar todas as estruturas criadas
*/
void desalocall(void);

/* declaracao da rotina de acao sobre erro fatal
*/
void fatalerr(int lin, int err);

/* declaracao das funcoes e rotinas auxiliares na manipulacao dos dados
*/
int ctip(char *tip);   // converte o valor do tipo de elemento
int cfas(char *fas);   // converte o valor do sistema de fase do elemento
int ctip(char *cmd);   // converte o valor do tipo de interruptor
int ccmd(char *cmd, itemElem_t *pElem);  // converte a string de comandos em lista

/* declaracao da rotina responsavel pela leitura do arquivo de
/* entrada e carga das listas de eletrodutos e quadros eletricos
*/
int ler_arqventr(char *arqv);

/* declaracao da rotina responsavel pela construcao da lista de
/* elementos de origem igual a (org) a partir do arquivo temporario
*/
int ler_arqvtemp(itemQdr_t *pQdr);

/* declaracao da rotina de construcao da lista de desvios
*/
void cons_desvios(void);
void adiciona_desvios(itemElem_t* pElem);

/* declaracao das rotinas de ajuste da lista de comandos
*/
int ajusta_lista(void);
int ajusta_comando(itemElemCmd_t **primElemCmd);
void move_comando(itemElemCmd_t **pFonte, itemElemCmd_t **pDest);

/* declaracao da rotina responsavel pela construcao da arvore
/* que caracteriza a malha eletrica
*/
void cons_malha(char *hnd, itemMalha_t **pItem);

/* declaracao da rotina responsavel pela construcao da lista
/* dos elementos ligados a um no da malha eletrica
*/
void cons_lista(char *hnd, itemMalha_t **pItem);

/* implementacao da rotina responsavel por conectar os desvios
*/
void proc_desvios(itemMalha_t** iItem, itemMalha_t Item);

/* declaracao da rotina responsavel por inserir a fiacao
/* eletrica nas arvores com raiz do tipo quadro
*/
void proc_quadro(itemQdr_t *pRaiz, itemMalha_t *pItem);

/* declaracao da rotina responsavel por inserir a fiacao
/* eletrica nas arvores com raiz do tipo comando
*/
void proc_comando(itemCmd_t *pCmd, itemElemCmd_t *pRaiz, itemMalha_t *pItem);

/* declaracao da rotina responsavel por processar os itens
/* das arvores com raiz do tipo quadro
*/
void proc_itemDoQuadro(itemQdr_t *pRaiz, itemElem_t *pElem);

/* declaracao da rotina responsavel por processar os itens
/* das arvores com raiz do tipo comando
*/
void proc_itemDoComando(itemCmd_t *pCmd, itemElemCmd_t *pRaiz, itemElem_t *pElem);

/* declaracao da rotina responsavel por inserir os fios
/* nos eletrodutos listados na pilha
*/
void proc_fios(char *cirOrCmd, int fios);

/* declaracao da rotina responsavel por gerar o arquivo de saida
*/
void gerar_arqvsaida(char *arqv);

/* declaracao da rotina de apresentacao do resultado no video
*/
void debug_eletr(void);
void debug_elem(void);
void debug_desvios(void);
void debug_comandos(void);
void debug_malha(itemMalha_t *primMalha);
void debug_result(void);


/* *** rotina principal *** */

main()
{
  itemQdr_t *pQdr;

  itemCmd_t *pCmd;
  itemElemCmd_t *pECmd;

  Parm_t parm;

  getparm(&parm);

  printf("\nFIACAO - Processamento da Fiacao Eletrica");
  printf("\npor Luiz Marcio Faria Viana, 9/5/96");

  printf("\n\nProcessando os dados de entrada...");
  ler_arqventr(parm.sourceFile);
  if(parm.debugMode == -1) {
    printf("\nLista de eletrodutos:");
    debug_eletr();
  }
  pQdr = primQdr;
  while(pQdr != NULL) {
    printf("\nProcessando fiacao eletrica do quadro = %s...", pQdr->qdr);
    ler_arqvtemp(pQdr);
    cons_desvios();
    if(parm.debugMode == -1) {
    }
    if(parm.debugMode == -1) {
      printf("\nLista de elementos:");
      debug_elem();
      printf("\nLista de desvios:");
      debug_desvios();
      printf("\nLista de comandos ANTES do ajuste:");
      debug_comandos();
    }
    ajusta_lista();
    if(parm.debugMode == -1) {
      printf("\nLista de comandos APOS o ajuste:");
      debug_comandos();
    }
    leltr_limpaSinalizadores();
    cons_malha(pQdr->hnd, & primMalha);
    if(parm.debugMode == -1) {
      printf("\nTopo da arvore = %s\n", pQdr->qdr);
      debug_malha(primMalha);
    }
    proc_quadro(pQdr, primMalha);
    lmlh_desaloca(& primMalha);
    pCmd = primCmd;
    while(pCmd != NULL) {
      pECmd = pCmd->primElemCmd;
      while(pECmd != NULL) {
	ldes_limpaSinalizadores();
	leltr_limpaSinalizadores();
	cons_malha(pECmd->hnd, & primMalha);
	if(parm.debugMode == -1) {
	  printf("\nTopo da arvore = comando(%s)\n", pECmd->hnd);
	  debug_malha(primMalha);
	}
	proc_comando(pCmd, pECmd, primMalha);
	lmlh_desaloca(& primMalha);
	pECmd = pECmd->prox;
      }
      pCmd = pCmd->prox;
    }
    printf("\n  (Processamento do quadro %s concluido!)", pQdr->qdr);
    if(parm.debugMode == -1)
      debug_result();
    lcmd_desaloca();
    lelem_desaloca();
    pQdr = pQdr->prox;
  }
  if(parm.targetFile == NULL)
    gerar_arqvsaida(FILE_TARGET);
  else
    gerar_arqvsaida(parm.targetFile);
  printf("\n\nProcessamento concluido!");
  desalocall();
  return(0);
}


/* *** implementacoes *** */

/* implementacao das funcoes de manipulacao das listas de handles
*/
itemHnd_t* lhnd_insereItem(itemHnd_t item)
{
  itemHnd_t *ip;
  if((ip = (itemHnd_t *) malloc(sizeof(itemHnd_t))) == NULL)
    fatalerr(161, ERR_ALLOCMEM);
  if((ip->hnd = (char *) malloc(STRSZ(item.hnd))) == NULL) {
    free(ip);
    fatalerr(429, ERR_ALLOCMEM);
  }
  strcpy(ip->hnd, item.hnd);
  ip->idx  = item.idx;
  ip->prox = primHnd;
  primHnd = ip;
  return ip;
}

int  lhnd_procuraItem(char *hnd, int idx, itemHnd_t **iItem)
{
  (*iItem) = primHnd;
  while((*iItem) != NULL) {
    if( ( !strcmp((*iItem)->hnd, hnd) ) && ((*iItem)->idx == idx) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void lhnd_desaloca(void)
{
  itemHnd_t *ip, *p;
  ip = primHnd;
  while(ip != NULL) {
    if(ip->hnd != NULL)
      free(ip->hnd);
    p = ip; ip = ip->prox;
    free(p);
  }
  primHnd = NULL;
}

/* implementacao das funcoes de manipulacao das listas de fios
*/
itemFios_t* lfios_insereItem(itemFios_t **primFios, itemFios_t item)
{
  itemFios_t *ip;
  if((ip = (itemFios_t *) malloc(sizeof(itemFios_t))) == NULL)
    fatalerr(196, ERR_ALLOCMEM);
  if((ip->cirOrCmd = (char *) malloc(STRSZ(item.cirOrCmd))) == NULL) {
    free(ip);
    fatalerr(198, ERR_ALLOCMEM);
  }
  strcpy(ip->cirOrCmd, item.cirOrCmd);
  ip->fios = item.fios;
  ip->prox = (*primFios);
  (*primFios) = ip;
  return ip;
}

int  lfios_procuraItem(itemFios_t **primFios, char *cirOrCmd, itemFios_t **iItem)
{
  (*iItem) = (*primFios);
  while((*iItem) != NULL) {
    if( !strcmp((*iItem)->cirOrCmd, cirOrCmd) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void lfios_desaloca(itemFios_t **primFios)
{
  itemFios_t *ip, *p;
  ip = (*primFios);
  while(ip != NULL) {
    if(ip->cirOrCmd != NULL)
      free(ip->cirOrCmd);
    p = ip; ip = ip->prox;
    free(p);
  }
  (*primFios) = NULL;
}

/* implementacao das funcoes de manipulacao da lista de quadros
*/
itemQdr_t* lqdr_insereItem(itemQdr_t item)
{
  itemQdr_t *ip;
  if((ip = (itemQdr_t *) malloc(sizeof(itemQdr_t))) == NULL)
    fatalerr(238, ERR_ALLOCMEM);
  if((ip->qdr = (char *) malloc(STRSZ(item.qdr))) == NULL) {
    free(ip);
    fatalerr(240, ERR_ALLOCMEM);
  }
  if((ip->org = (char *) malloc(STRSZ(item.org))) == NULL) {
    free(ip->qdr);
    free(ip);
    fatalerr(245, ERR_ALLOCMEM);
  }
  if((ip->cir = (char *) malloc(STRSZ(item.cir))) == NULL) {
    free(ip->org);
    free(ip->qdr);
    free(ip);
    fatalerr(339, ERR_ALLOCMEM);
  }
  if((ip->hnd = (char *) malloc(STRSZ(item.hnd))) == NULL) {
    free(ip->org);
    free(ip->qdr);
    free(ip->cir);
    free(ip);
    fatalerr(530, ERR_ALLOCMEM);
  }
  strcpy(ip->hnd, item.hnd);
  ip->idx = item.idx;
  strcpy(ip->qdr, item.qdr);
  strcpy(ip->org, item.org);
  strcpy(ip->cir, item.cir);
  ip->fas = item.fas;
  ip->prox = primQdr;
  primQdr = ip;
  return ip;
}

int lqdr_procuraItem(char *hnd, int idx, itemQdr_t **iItem)
{
  (*iItem) = primQdr;
  while((*iItem) != NULL) {
    if( ( !strcmp((*iItem)->hnd, hnd) ) && ((*iItem)->idx == idx) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void lqdr_desaloca(void)
{
  itemQdr_t *ip, *p;
  ip = primQdr;
  while(ip != NULL) {
    if(ip->qdr != NULL)
      free(ip->qdr);
    if(ip->org != NULL)
      free(ip->org);
    if(ip->cir != NULL)
      free(ip->cir);
    if(ip->hnd != NULL)
      free(ip->hnd);
    p = ip; ip = ip->prox;
    free(p);
  }
  primQdr = NULL;
}

/* implementacao das funcoes de manipulacao da lista de eletrodutos
*/
itemEletr_t* leltr_insereItem(itemEletr_t item)
{
  itemEletr_t *ip;
  if((ip = (itemEletr_t *) malloc(sizeof(itemEletr_t))) == NULL)
    fatalerr(289, ERR_ALLOCMEM);
  ip->flg  = item.flg;
  if((ip->hnd = (char *) malloc(STRSZ(item.hnd))) == NULL) {
    free(ip);
    fatalerr(584, ERR_ALLOCMEM);
  }
  if((ip->hnd1 = (char *) malloc(STRSZ(item.hnd1))) == NULL) {
    free(ip->hnd);
    free(ip);
    fatalerr(589, ERR_ALLOCMEM);
  }
  if((ip->hnd2 = (char *) malloc(STRSZ(item.hnd2))) == NULL) {
    free(ip->hnd);
    free(ip->hnd1);
    free(ip);
    fatalerr(595, ERR_ALLOCMEM);
  }
  strcpy(ip->hnd, item.hnd);
  strcpy(ip->hnd1, item.hnd1);
  strcpy(ip->hnd2, item.hnd2);
  ip->primFios = NULL;
  ip->prox = primEletr;
  primEletr = ip;
  return ip;
}

int  leltr_procuraItem(char *hnd, itemEletr_t **iItem)
{
  (*iItem) = primEletr;
  while((*iItem) != NULL) {
    if( !strcmp((*iItem)->hnd, hnd) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void leltr_limpaSinalizadores(void)
{
  itemEletr_t *pEletr;
  pEletr = primEletr;
  while(pEletr != NULL) {
    pEletr->flg = 0;
    pEletr = pEletr->prox;
  }
}

void leltr_desaloca(void)
{
  itemEletr_t *ip, *p;
  ip = primEletr;
  while(ip != NULL) {
    if(ip->hnd != NULL)
      free(ip->hnd);
    if(ip->hnd1 != NULL)
      free(ip->hnd1);
    if(ip->hnd2 != NULL)
      free(ip->hnd2);
    lfios_desaloca(&ip->primFios);
    p = ip; ip = ip->prox;
    free(p);
  }
  primEletr = NULL;
}

/* implementacao das funcoes de manipulacao das listas de elementos de comandos
*/
itemElemCmd_t* lecmd_insereItem(itemElemCmd_t **primElemCmd, itemElemCmd_t item)
{
  itemElemCmd_t *ip;
  if((ip = (itemElemCmd_t *) malloc(sizeof(itemElemCmd_t))) == NULL)
    fatalerr(508, ERR_ALLOCMEM);
  if((ip->hnd = (char *) malloc(STRSZ(item.hnd))) == NULL) {
    free(ip);
    fatalerr(654, ERR_ALLOCMEM);
  }
  strcpy(ip->hnd, item.hnd);
  ip->idx = item.idx;
  ip->itip = item.itip;
  ip->prox = (*primElemCmd);
  (*primElemCmd) = ip;
  return ip;
}

int  lecmd_procuraItem(itemElemCmd_t **primElemCmd, char *hnd, int idx, itemElemCmd_t **iItem)
{
  (*iItem) = (*primElemCmd);
  while((*iItem) != NULL) {
    if( ( !strcmp((*iItem)->hnd, hnd) ) && ((*iItem)->idx == idx) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void lecmd_desaloca(itemElemCmd_t **primElemCmd)
{
  itemElemCmd_t *ip, *p;
  ip = (*primElemCmd);
  while(ip != NULL) {
    if(ip->hnd != NULL)
      free(ip->hnd);
    p = ip; ip = ip->prox;
    free(p);
  }
  (*primElemCmd) = NULL;
}

/* implementacao das funcoes de manipulacao das listas de comandos
*/
itemCmd_t* lcmd_insereItem(itemCmd_t item)
{
  itemCmd_t *ip;
  if((ip = (itemCmd_t *) malloc(sizeof(itemCmd_t))) == NULL)
    fatalerr(324, ERR_ALLOCMEM);
  if((ip->cmd = (char *) malloc(STRSZ(item.cmd))) == NULL) {
    free(ip);
    fatalerr(327, ERR_ALLOCMEM);
  }
  if(item.cir == NULL) {
    ip->cir = NULL;
  }
  else {
    if((ip->cir = (char *) malloc(STRSZ(item.cir))) == NULL) {
      free(ip->cmd);
      free(ip);
      fatalerr(333, ERR_ALLOCMEM);
    }
  }
  strcpy(ip->cmd, item.cmd);
  if(item.cir != NULL)
    strcpy(ip->cir, item.cir);
  ip->fas = item.fas;
  ip->primElemCmd = item.primElemCmd;
  ip->prox = primCmd;
  primCmd = ip;
  return ip;
}

itemCmd_t* lcmd_alteraItem(itemCmd_t item)
{
  itemCmd_t *ip;

  if(lcmd_procuraItem(item.cmd, &ip) == RTERR) return RTERR;

  if(ip->cir != NULL)
     free(ip->cir);

  if(item.cir == NULL)
    ip->cir = NULL;
  else if((ip->cir = (char *) malloc(STRSZ(item.cir))) == NULL)
	 fatalerr(333, ERR_ALLOCMEM);

  if(item.cir != NULL)
    strcpy(ip->cir, item.cir);
  ip->fas = item.fas;
  ip->primElemCmd = item.primElemCmd;

  return ip;
}

int  lcmd_procuraItem(char *cmd, itemCmd_t **iItem)
{
  (*iItem) = primCmd;
  while((*iItem) != NULL) {
    if( !strcmp((*iItem)->cmd, cmd) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void lcmd_desaloca(void)
{
  itemCmd_t *ip, *p;
  ip = primCmd;
  while(ip != NULL) {
    if(ip->cmd != NULL)
      free(ip->cmd);
    if(ip->cir != NULL)
      free(ip->cir);
    if(ip->primElemCmd != NULL)
      lecmd_desaloca(&ip->primElemCmd);
    p = ip; ip = ip->prox;
    free(p);
  }
  primCmd = NULL;
}

/* implementacao das funcoes de manipulacao das listas de comandos do elemento
*/
itemCmdElem_t* lcmde_insereItem(itemCmdElem_t **primCmdElem, itemCmdElem_t item)
{
  itemCmdElem_t *ip;
  if((ip = (itemCmdElem_t *) malloc(sizeof(itemCmdElem_t))) == NULL)
    fatalerr(508, ERR_ALLOCMEM);
  ip->pCmd = item.pCmd;
  ip->prox = (*primCmdElem);
  (*primCmdElem) = ip;
  return ip;
}

int  lcmde_procuraItem(itemCmdElem_t **primCmdElem, char *cmd, itemCmdElem_t **iItem)
{
  (*iItem) = (*primCmdElem);
  while((*iItem) != NULL) {
    if( !strcmp((*iItem)->pCmd->cmd, cmd) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void lcmde_desaloca(itemCmdElem_t **primCmdElem)
{
  itemCmdElem_t *ip, *p;
  ip = (*primCmdElem);
  while(ip != NULL) {
    p = ip; ip = ip->prox;
    free(p);
  }
  (*primCmdElem) = NULL;
}

/* declaracao das funcoes de manipulacao da lista de desvios
*/
itemDes_t* ldes_insereItem(itemDes_t item)
{
  itemDes_t* ip;
  if((ip = (itemDes_t*) malloc(sizeof(itemDes_t))) == NULL)
    fatalerr(746, ERR_ALLOCMEM);
  if((ip->des = (char*) malloc(STRSZ(item.des))) == NULL) {
    free(ip);
    fatalerr(749, ERR_ALLOCMEM);
  }
  strcpy(ip->des, item.des);
  ip->flg = 0;
  ip->primElemDes = NULL;
  ip->prox = primDes;
  primDes = ip;
  return ip;
}

int ldes_procuraItem(char *des, itemDes_t **iItem)
{
  (*iItem) = primDes;
  while((*iItem) != NULL) {
    if( !strcmp((*iItem)->des, des) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void ldes_limpaSinalizadores(void)
{
  itemDes_t *pDes;
  itemElemDes_t *pEDes;

  pDes = primDes;
  while(pDes != NULL) {
    pDes->flg = 0;
    pEDes = pDes->primElemDes;
    while(pEDes != NULL) {
      pEDes->flg = 0;
      pEDes = pEDes->prox;
    }
    pDes = pDes->prox;
  }
}

void ldes_desaloca(void)
{
  itemDes_t* ip;
  while(primDes != NULL) {
    if(primDes->des != NULL)
      free(primDes->des);
    if(primDes->primElemDes != NULL)
      ldese_desaloca(&primDes->primElemDes);
    ip = primDes->prox;
    free(primDes);
    primDes = ip;
  }
}

/* declaracao das funcoes de manipulacao da lista de elementos de desvio
*/
itemElemDes_t* ldese_insereItem(itemElemDes_t** primElemDes, itemElemDes_t item)
{
  itemElemDes_t* ip;
  if((ip = (itemElemDes_t*) malloc(sizeof(itemElemDes_t))) == NULL)
    fatalerr(788, ERR_ALLOCMEM);
  ip->flg = 0;
  ip->pElem = item.pElem;
  ip->prox = (*primElemDes);
  (*primElemDes) = ip;
  return ip;
}

int ldese_procuraItem(itemElemDes_t** primElemDes, char* hnd, int idx, itemElemDes_t** iItem)
{

  (*iItem) = (*primElemDes);
  while((*iItem) != NULL) {
    if( ( !strcmp((*iItem)->pElem->hnd, hnd) ) && ((*iItem)->pElem->idx == idx) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void ldese_desaloca(itemElemDes_t** primElemDes)
{
  itemElemDes_t* ip;
  while((*primElemDes) != NULL) {
    ip = (*primElemDes);
    (*primElemDes) = (*primElemDes)->prox;
    free(ip);
  }
}

/* implementacao das funcoes de manipulacao da lista de elementos
*/
itemElem_t* lelem_insereItem(itemElem_t item)
{
  itemElem_t *ip;
  if((ip = (itemElem_t *) malloc(sizeof(itemElem_t))) == NULL)
    fatalerr(376, ERR_ALLOCMEM);
  if((ip->org = (char *) malloc(STRSZ(item.org))) == NULL) {
    free(ip);
    fatalerr(379, ERR_ALLOCMEM);
  }
  if((ip->des = (char *) malloc(STRSZ(item.des))) == NULL) {
    free(ip->org);
    free(ip);
    fatalerr(722, ERR_ALLOCMEM);
  }
  if((ip->cir = (char *) malloc(STRSZ(item.cir))) == NULL) {
    free(ip->org);
    free(ip->des);
    free(ip);
    fatalerr(384, ERR_ALLOCMEM);
  }
  if((ip->hnd = (char *) malloc(STRSZ(item.hnd))) == NULL) {
    free(ip->org);
    free(ip->des);
    free(ip->cir);
    free(ip);
    fatalerr(927, ERR_ALLOCMEM);
  }
  strcpy(ip->hnd, item.hnd);
  ip->idx     = item.idx;
  ip->tip     = item.tip;
  strcpy(ip->org, item.org);
  strcpy(ip->des, item.des);
  strcpy(ip->cir, item.cir);
  ip->primCmdElem = item.primCmdElem;
  ip->fas     = item.fas;
  ip->prox    = primElem;
  primElem    = ip;
  return ip;
}

int  lelem_procuraItem(char* hnd, int idx, itemElem_t **iItem)
{
  (*iItem) = primElem;
  while((*iItem) != NULL) {
    if( ( !strcmp((*iItem)->hnd, hnd) ) && ((*iItem)->idx == idx) )
      return RTNORM;
    (*iItem) = (*iItem)->prox;
  }
  return RTERR;
}

void lelem_desaloca(void)
{
  itemElem_t *ip, *p;
  ip = primElem;
  while(ip != NULL) {
    if(ip->org != NULL)
      free(ip->org);
    if(ip->cir != NULL)
      free(ip->cir);
    if(ip->hnd != NULL)
      free(ip->hnd);
    if(ip->primCmdElem != NULL)
      lcmde_desaloca(&ip->primCmdElem);
    p = ip; ip = ip->prox;
    free(p);
  }
  primElem = NULL;
}

/* implementacao das funcoes de manipulacao da arvore de malha eletrica
*/
itemMalha_t* lmlh_insereItem(itemMalha_t **primMalha, itemMalha_t item)
{
  itemMalha_t *ip;
  if((ip = (itemMalha_t *) malloc(sizeof(itemMalha_t))) == NULL)
    fatalerr(473, ERR_ALLOCMEM);
  ip->pElem = item.pElem;
  ip->pEletr = item.pEletr;
  ip->primMalha = item.primMalha;
  ip->prox = (*primMalha);
  (*primMalha) = ip;
  return ip;
}

void lmlh_desaloca(itemMalha_t **primMalha)
{
  itemMalha_t *ip, *p;
  ip = (*primMalha);
  while(ip != NULL) {
    if(ip->primMalha != NULL)
      lmlh_desaloca(& ip->primMalha);
    p = ip; ip = ip->prox;
    free(p);
  }
  (*primMalha) = NULL;
}

/* declaracao das funcoes de manipulacao da pilha de eletrodutos
*/
itemPilha_t* pilha_insereItem(itemEletr_t *pEletr)
{
  itemPilha_t *ip;
  if((ip = (itemPilha_t *) malloc(sizeof(itemPilha_t))) == NULL)
    fatalerr(527, ERR_ALLOCMEM);
  ip->pEletr = pEletr;
  ip->prox = topoPilha;
  topoPilha = ip;
  return ip;
}

itemEletr_t* pilha_retiraItem(void)
{
  itemPilha_t *pPilha;
  itemEletr_t *pEletr = NULL;
  if(topoPilha != NULL) {
    pEletr = topoPilha->pEletr;
    pPilha = topoPilha;
    topoPilha = topoPilha->prox;
    free(pPilha);
  }
  return pEletr;
}

void pilha_desaloca(void)
{
  itemPilha_t *pPilha;
  while(topoPilha != NULL) {
    pPilha = topoPilha;
    topoPilha = topoPilha->prox;
    free(pPilha);
  }
}

/* implementacao da rotina p/ desalocar todas as estruturas
*/
void desalocall(void)
{
  lhnd_desaloca();
  leltr_desaloca();
  lqdr_desaloca();
  lhnd_desaloca();
  ldes_desaloca();
  lmlh_desaloca(& primMalha);
  pilha_desaloca();
}

/* implementacao da rotina de acao sobre erro fatal
*/
void fatalerr(int lin, int err)
{
  desalocall();
  errmsg(lin, err);
}

/* implementacao das funcoes e rotinas auxiliares na manipulacao de dados
*/
int ctip(char *tip)
{
  if( !strcmp(tip, "ECARGA") )
    return T_CARGA;
  else if( !strcmp(tip, "ECOMANDO") )
    return T_COMANDO;
  else if( !strcmp(tip, "EQUADRO") )
    return T_QUADRO;
  else if( !strcmp(tip, "ECAMPAINHA") )
    return T_CAMPAINHA;
  else if( !strcmp(tip, "EILUMINACAO") )
    return T_ILUMINACAO;
  else if( !strcmp(tip, "ECAIXA") )
    return T_CAIXA;
  else if( !strcmp(tip, "EDESVIO") )
    return T_DESVIO;
  else if( !strcmp(tip, "ECALHA") )
    return T_CALHA;
  return RTERR;
}

int cfas(char *fas)
{
  if( !strcmp(fas, "F+N") )
    return _FN;
  else if( !strcmp(fas, "2F") )
    return _2F;
  else if( !strcmp(fas, "2F+N") )
    return _2FN;
  else if( !strcmp(fas, "3F") )
    return _3F;
  else if( !strcmp(fas, "3F+N") )
    return _3FN;
  else if( !strcmp(fas, "F+N+T") )
    return _FNT;
  else if( !strcmp(fas, "2F+T") )
    return _2FT;
  else if( !strcmp(fas, "2F+N+T") )
    return _2FNT;
  else if( !strcmp(fas, "3F+T") )
    return _3FT;
  else if( !strcmp(fas, "3F+N+T") )
    return _3FNT;
  return RTERR;
}

int  citip(char *cmd)
{
  if( !strnicmp(cmd, ID_3W, strlen(ID_3W)) ) {
    strcpy(cmd, &cmd[2]);
    return _I3W;
  }
  else if( !strnicmp(cmd, ID_4W, strlen(ID_4W)) ) {
    strcpy(cmd, &cmd[2]);
    return _I4W;
  }
  return _IS;
}

int ccmd(char *cmd, itemElem_t *pElem)
{
  itemElemCmd_t iECmd, *pECmd;
  itemCmd_t iCmd, *pCmd;
  itemCmdElem_t iCElem, *pCElem;

  char s[31], s1[31];  // variaveis auxiliares
  int itip;

  strcpy(s, cmd);
  while((*s) != '\0') {
    strhead(s1, s, ',');
    itip = citip(s1);
    if(lcmd_procuraItem(s1, &pCmd) == RTERR) {
      iCmd.cmd = s1;
      switch(pElem->tip) {
	case T_CARGA :
	case T_CAMPAINHA :
	case T_ILUMINACAO :
	  iCmd.cir = pElem->cir;
	  iCmd.fas = pElem->fas;
	  break;
	case T_COMANDO :
	  iCmd.cir = NULL;
	  iCmd.fas = 0;
      };
      iCmd.primElemCmd = NULL;
      pCmd = lcmd_insereItem(iCmd);
    }
    switch(pElem->tip) {
      case T_CARGA :
      case T_CAMPAINHA :
      case T_ILUMINACAO :
	iCmd.cmd = s1;
	iCmd.cir = pElem->cir;
	iCmd.fas = pElem->fas;
	iCmd.primElemCmd = pCmd->primElemCmd;
	lcmd_alteraItem(iCmd);
	break;
      case T_COMANDO :
	if(lecmd_procuraItem(&pCmd->primElemCmd, pElem->hnd, pElem->idx, &pECmd) == RTERR)
	{
	  iECmd.hnd = pElem->hnd;
	  iECmd.idx = pElem->idx;
	  iECmd.itip = itip;
	  lecmd_insereItem(&pCmd->primElemCmd, iECmd);
	}
    };
    if(lcmde_procuraItem(&pElem->primCmdElem, s1, &pCElem) == RTERR) {
      iCElem.pCmd = pCmd;
      lcmde_insereItem(&pElem->primCmdElem, iCElem);
    }
    strtail(s, s, ',');
  }
  return RTNORM;
}

/* implementacao da rotina responsavel pela leitura do arquivo de
/* entrada e carga das listas de eletrodutos e quadros eletricos
*/
int ler_arqventr(char *arqv)
{
  FILE *f_in;
  FILE *f_tmp;

  fileReg_t R_in;
  tempReg_t R_tmp;

  itemHnd_t   iHnd,  *pHnd;
  itemEletr_t iElem, *pElem;
  itemQdr_t   iQdr,  *pQdr;

  int idx1, idx2;

  str_t buff;

  if((f_in = fopen(arqv, "r")) == NULL)
    fatalerr(555, ERR_OPENFILE);

  if((f_tmp = fopen(FILE_TEMP, "wb")) == NULL)
    fatalerr(558, ERR_OPENFILE);

  while(fgets(buff, sizeof(str_t), f_in) != NULL) {
    buff[STRSZ(buff) - 2] = '\0';
    str2fileReg(buff, &R_in);

    iElem.flg = 0;
    iElem.hnd = R_in.hnd;
    iElem.hnd1 = R_in.hnd1;
    idx1 = atoi(R_in.idx1);
    iElem.hnd2 = R_in.hnd2;
    idx2 = atoi(R_in.idx2);
    if(leltr_procuraItem(iElem.hnd, &pElem) == RTERR)
      leltr_insereItem(iElem);

    if(lhnd_procuraItem(iElem.hnd1, idx1, &pHnd) == RTERR) {

      strcpy(R_tmp.hnd, iElem.hnd1);
      R_tmp.idx = idx1;
      R_tmp.tip = ctip(R_in.tip1);
      strcpy(R_tmp.qdr, R_in.qdr1);
      strcpy(R_tmp.org, R_in.org1);
      strcpy(R_tmp.des, R_in.des1);
      strcpy(R_tmp.cir, R_in.cir1);
      strcpy(R_tmp.cmd, R_in.cmd1);
      R_tmp.fas = cfas(R_in.fas1);

      if(fwrite(&R_tmp, sizeof(tempReg_t), 1, f_tmp) == 0)
	fatalerr(579, ERR_CANTWRITE);

      if(R_tmp.tip == T_QUADRO) {
	iQdr.hnd = R_tmp.hnd;
	iQdr.idx = R_tmp.idx;
	iQdr.qdr = R_tmp.qdr;
	iQdr.org = R_tmp.org;
	iQdr.cir = R_tmp.cir;
	iQdr.fas = R_tmp.fas;
	lqdr_insereItem(iQdr);
      }

      iHnd.hnd = R_tmp.hnd;
      iHnd.idx = R_tmp.idx;
      lhnd_insereItem(iHnd);
    }

    if(lhnd_procuraItem(iElem.hnd2, idx2, &pHnd) == RTERR) {

      strcpy(R_tmp.hnd, iElem.hnd2);
      R_tmp.idx = idx2;
      R_tmp.tip = ctip(R_in.tip2);
      strcpy(R_tmp.qdr, R_in.qdr2);
      strcpy(R_tmp.org, R_in.org2);
      strcpy(R_tmp.des, R_in.des2);
      strcpy(R_tmp.cir, R_in.cir2);
      strcpy(R_tmp.cmd, R_in.cmd2);
      R_tmp.fas = cfas(R_in.fas2);

      if(fwrite(&R_tmp, sizeof(tempReg_t), 1, f_tmp) == 0)
	fatalerr(604, ERR_CANTWRITE);

      if(R_tmp.tip == T_QUADRO) {
	iQdr.hnd = R_tmp.hnd;
	iQdr.idx = R_tmp.idx;
	iQdr.qdr = R_tmp.qdr;
	iQdr.org = R_tmp.org;
	iQdr.cir = R_tmp.cir;
	iQdr.fas = R_tmp.fas;
	lqdr_insereItem(iQdr);
      }

      iHnd.hnd = R_tmp.hnd;
      iHnd.idx = R_tmp.idx;
      lhnd_insereItem(iHnd);
    }
  }
  fclose(f_tmp);
  fclose(f_in);

  lhnd_desaloca();

  return RTNORM;
}

/* implementacao da rotina responsavel pela construcao da lista de
/* elementos de origem igual a (org) a partir do arquivo temporario
*/
int ler_arqvtemp(itemQdr_t *pQdr)
{
  FILE *f_tmp;
  tempReg_t R_tmp;

  itemElem_t iElem, *pElem;

  str_t buff;

  if((f_tmp = fopen(FILE_TEMP, "rb")) == NULL)
    fatalerr(637, ERR_OPENFILE);

  iElem.hnd = pQdr->hnd;
  iElem.idx = pQdr->idx;
  iElem.tip = T_QUADRO;
  iElem.org = pQdr->org;
  iElem.des = "";
  iElem.cir = pQdr->cir;
  iElem.primCmdElem = NULL;
  iElem.fas = pQdr->fas;
  lelem_insereItem(iElem);

  while(fread(&R_tmp, sizeof(tempReg_t), 1, f_tmp) != 0) {
    if( !strcmp(R_tmp.org, pQdr->qdr) ) {
      iElem.hnd     = R_tmp.hnd;
      iElem.idx     = R_tmp.idx;
      iElem.tip     = R_tmp.tip;
      iElem.org     = R_tmp.org;
      iElem.des     = R_tmp.des;
      iElem.cir     = R_tmp.cir;
      iElem.primCmdElem = NULL;
      iElem.fas     = R_tmp.fas;
      ccmd(R_tmp.cmd, &iElem);
      lelem_insereItem(iElem);
    }
  }
  fclose(f_tmp);

  return RTNORM;
}

/* declaracao da rotina de construcao da lista de desvios
*/
void cons_desvios(void)
{
  itemElem_t *p;
  p = primElem;
  while(p != NULL) {
    if(p->tip == T_DESVIO)
      adiciona_desvios(p);
    p = p->prox;
  }
}

void adiciona_desvios(itemElem_t* pElem)
{
  itemElemDes_t iEDes;
  itemDes_t iDes, *pDes;
  if(ldes_procuraItem(pElem->des, &pDes) == RTERR) {
    iDes.des = pElem->des;
    pDes = ldes_insereItem(iDes);
  }
  iEDes.pElem = pElem;
  ldese_insereItem(&pDes->primElemDes, iEDes);
}

/* declaracao das rotinas de ajuste da lista de comandos
*/
int ajusta_lista(void)
{
  itemCmd_t iCmd, *pCmd;
  pCmd = primCmd;
  while(pCmd != NULL) {
    if(pCmd->cir == NULL) {
      iCmd.cmd = pCmd->cmd;
      if((iCmd.cir = (char *) malloc(STRSZ(pCmd->cmd) + 1)) == NULL)
	fatalerr(1117, ERR_ALLOCMEM);
      strcat(strcpy(iCmd.cir, "?"), pCmd->cmd);
      iCmd.fas = pCmd->fas;
      iCmd.primElemCmd = pCmd->primElemCmd;
      lcmd_alteraItem(iCmd);
    }
    ajusta_comando(&pCmd->primElemCmd);
    pCmd = pCmd->prox;
  }
  return RTNORM;
}

int ajusta_comando(itemElemCmd_t **primElemCmd)
{
  itemElemCmd_t *pFonte, *pDest;

  pFonte = (*primElemCmd);
  pDest = NULL;

  while(pFonte != NULL)
    move_comando(&pFonte, &pDest);
  (*primElemCmd) = pDest;
  return RTNORM;
}

void move_comando(itemElemCmd_t **pFonte, itemElemCmd_t **pDest)
{
  itemElemCmd_t *ip, *cip, *p;

  if((*pFonte) == NULL) return;

  ip = (*pFonte);
  (*pFonte) = ip->prox;

  if((*pDest) == NULL) {
    ip->prox = (*pDest);
    (*pDest) = ip;
  }
  else {
    p = cip = (*pDest);
    while(cip != NULL) {
      if(strcmp(ip->hnd, cip->hnd) < 0) break;
      p = cip; cip = cip->prox;
    }
    if(cip == NULL) {
      ip->prox = NULL;
      p->prox = ip;
    }
    else {
      ip->prox = cip;
      if(cip == (*pDest))
	(*pDest) = ip;
      else
	p->prox = ip;
    }
  }
}

/* implementacao da rotina responsavel pela construcao da arvore
/* que caracteriza a malha eletrica
*/
void cons_malha(char *hnd, itemMalha_t **pItem)
{
  if((*pItem) == NULL) {
    if(((*pItem) = (itemMalha_t *) malloc(sizeof(itemMalha_t))) == NULL)
      fatalerr(744, ERR_ALLOCMEM);
    (*pItem)->pElem     = NULL;
    (*pItem)->pEletr    = NULL;
    (*pItem)->primMalha = NULL;
    (*pItem)->prox      = NULL;
  }
  cons_lista(hnd, & (*pItem)->primMalha);
  if((*pItem)->prox != NULL)
    cons_malha(((*pItem)->prox->pElem)->hnd, & (*pItem)->prox);
  if((*pItem)->primMalha != NULL)
    cons_malha(((*pItem)->primMalha->pElem)->hnd, & (*pItem)->primMalha);
}

/* implementacao da rotina responsavel pela construcao da lista
/* dos elementos ligados a um no da malha eletrica
*/
void cons_lista(char *hnd, itemMalha_t **pItem)
{
  itemMalha_t item;
  int idx1, idx2;
  item.pEletr = primEletr;
  while(item.pEletr != NULL) {
    if( !item.pEletr->flg ) {
      if( !strcmp(hnd, item.pEletr->hnd1) ) {
	idx2 = 0;
	while(lelem_procuraItem(item.pEletr->hnd2, idx2, & item.pElem) == RTNORM) {
	  item.primMalha = NULL;
	  lmlh_insereItem(pItem, item);
	  if((item.pElem)->tip == T_DESVIO)
	    proc_desvios(pItem, item);
	  idx2 += 1;
	}
	item.pEletr->flg = -1;
      }
      else if( !strcmp(hnd, item.pEletr->hnd2) ) {
	idx1 = 0;
	while(lelem_procuraItem(item.pEletr->hnd1, idx1, & item.pElem) == RTNORM) {
	  item.primMalha = NULL;
	  lmlh_insereItem(pItem, item);
	  if((item.pElem)->tip == T_DESVIO)
	    proc_desvios(pItem, item);
	  idx1 += 1;
	}
	item.pEletr->flg = -1;
      }
    }
    item.pEletr = item.pEletr->prox;
  }
}

/* implementacao da rotina responsavel por conectar os desvios
*/
void proc_desvios(itemMalha_t** iItem, itemMalha_t Item)
{
  itemDes_t *pDes;
  itemElemDes_t *pEDes;

  itemElem_t *pElem;
  int idx;

  if(ldes_procuraItem((Item.pElem)->des, &pDes) == RTERR) return;

  if(pDes->flg != 0) return;
  pDes->flg = -1;

  if(ldese_procuraItem(&(pDes->primElemDes), (Item.pElem)->hnd, (Item.pElem)->idx, &pEDes) == RTNORM)
    pEDes->flg = -1;

  pEDes = pDes->primElemDes;
  while(pEDes != NULL) {
    if(pEDes->flg == 0) {
      pEDes->flg = -1;

      idx = 0;
      while(lelem_procuraItem((pEDes->pElem)->hnd, idx, &pElem) == RTNORM) {
	Item.pElem = pElem;
	lmlh_insereItem(iItem, Item);
	if((Item.pElem)->tip == T_DESVIO)
	  proc_desvios(iItem, Item);
	idx += 1;
      }

    }
    pEDes = pEDes->prox;
  }
}

/* implementacao da rotina responsavel por inserir a fiacao
/* eletrica nas arvores com raiz do tipo quadro
*/
void proc_quadro(itemQdr_t *pRaiz, itemMalha_t *pItem)
{
  pilha_insereItem(pItem->pEletr);
  if(pItem->primMalha != NULL)
    proc_quadro(pRaiz, pItem->primMalha);
  if(pItem->pElem != NULL)
    proc_itemDoQuadro(pRaiz, pItem->pElem);
  pilha_retiraItem();
  if(pItem->prox != NULL)
    proc_quadro(pRaiz, pItem->prox);
}

/* implementacao da rotina responsavel por inserir a fiacao
/* eletrica nas arvores com raiz do tipo comando
*/
void proc_comando(itemCmd_t *pCmd, itemElemCmd_t *pRaiz, itemMalha_t *pItem)
{
  pilha_insereItem(pItem->pEletr);
  if(pItem->primMalha != NULL)
    proc_comando(pCmd, pRaiz, pItem->primMalha);
  if(pItem->pElem != NULL)
    proc_itemDoComando(pCmd, pRaiz, pItem->pElem);
  pilha_retiraItem();
  if(pItem->prox != NULL)
    proc_comando(pCmd, pRaiz, pItem->prox);
}

/* implementacao da rotina responsavel por processar os itens
/* das arvores com raiz do tipo quadro
*/
#pragma argsused  // desabilita a verificacao dos parm usados na prox funcao

void proc_itemDoQuadro(itemQdr_t *pRaiz, itemElem_t *pElem)
{
  itemCmdElem_t *pCElem;

  char *cirOrCmd;
  int fas;

  switch(pElem->tip) {
    case T_CAMPAINHA:
    case T_CARGA:
    case T_ILUMINACAO :
      cirOrCmd = pElem->cir;
      if(pElem->primCmdElem != NULL)
	fas = pElem->fas & (__N);
      else
	fas = pElem->fas & (__F1 + __F2 + __F3 + __N);
      proc_fios(cirOrCmd, fas);
      if((fas = pElem->fas & __T) != 0)
	proc_fios("", fas);
      break;
    case T_QUADRO:
      cirOrCmd = pElem->cir;
      fas = pElem->fas & (__F1 + __F2 + __F3 + __N);
      proc_fios(cirOrCmd, fas);
      if((fas = pElem->fas & __T) != 0)
	proc_fios("", fas);
      break;
    case T_COMANDO:
      pCElem = pElem->primCmdElem;
      while(pCElem != NULL) {
	if( !strcmp(pCElem->pCmd->primElemCmd->hnd, pElem->hnd) ) {
	  cirOrCmd = pCElem->pCmd->cir;
	  fas = pCElem->pCmd->fas & (__F1 + __F2 + __F3);
	  proc_fios(cirOrCmd, fas);
	}
	pCElem = pCElem->prox;
      }
      break;
  };
}

/* implementacao da rotina responsavel por processar os itens
/* das arvores com raiz do tipo comando
*/
void proc_itemDoComando(itemCmd_t *pCmd, itemElemCmd_t *pRaiz, itemElem_t *pElem)
{
  char *cirOrCmd;
  int fas;

  if(pRaiz->prox == NULL) {
    /*
    /* alimenta as cargas com retorno
    */
    switch(pElem->tip) {
      case T_CAMPAINHA:
	if(pElem->primCmdElem != NULL)
	  if(pElem->primCmdElem->pCmd == pCmd) {
	    cirOrCmd = pCmd->cmd;
	    fas = (pCmd->fas & __F1) << 3;
	    proc_fios(cirOrCmd, fas);
	  }
	break;
      case T_CARGA:
      case T_ILUMINACAO :
	if(pElem->primCmdElem != NULL)
	  if(pElem->primCmdElem->pCmd == pCmd) {
	    cirOrCmd = pCmd->cmd;
	    fas = (pCmd->fas & (__F1 + __F2 + __F3)) << 4;
	    proc_fios(cirOrCmd, fas);
	  }
	break;
      case T_QUADRO:
	break;
      case T_COMANDO:
	break;
    }
  }
  else if( ( !strcmp(pElem->hnd, pRaiz->prox->hnd) ) && (pElem->idx == pRaiz->prox->idx)) {
    /*
    /* alimenta os interruptores three-way e four-way
    */
    switch(pRaiz->prox->itip) {
      case _IS :
	break;
      case _I3W :
      case _I4W :
	cirOrCmd = pCmd->cmd;
	if((pCmd->fas & __F1) != 0) {
	  fas = __R4 + __R5;
	  if((pCmd->fas & __F2) != 0) {
	    fas += __R6 + __R7;
	    if((pCmd->fas & __F3) != 0)
	      fas += __R8 + __R9;
	  }
	}
	proc_fios(cirOrCmd, fas);
    }
  }
}

/* implementacao da rotina responsavel por inserir os fios
/* nos eletrodutos listados na pilha
*/
void proc_fios(char *cirOrCmd, int fios)
{
  itemPilha_t *pPilha;
  itemFios_t iFios, *pFios;

  int aux1, aux2;
  int cnt;

  pPilha = topoPilha;
  while(pPilha != NULL) {
    if(pPilha->pEletr != NULL)
      if(lfios_procuraItem(& (pPilha->pEletr)->primFios, cirOrCmd, & pFios) == RTNORM) {
	aux1 = (pFios->fios & 0x1FF) | (fios & 0x1FF);
	aux2 = pFios->fios & 0xFE00;
	cnt = (fios >> 9) & 0x7F;
	while(cnt != 0) {
	  aux2 = aux2 << 1;
	  if((cnt & 0x01) != 0)
	    aux2 += __R4;
	  cnt = cnt >> 1;
	}
	pFios->fios = aux1 + aux2;
      }
      else {
	iFios.cirOrCmd = cirOrCmd;
	iFios.fios = fios;
	lfios_insereItem(& (pPilha->pEletr)->primFios, iFios);
      }
    pPilha = pPilha->prox;
  }
}

/* declaracao da rotina responsavel por gerar o arquivo de saida
*/
void gerar_arqvsaida(char *arqv)
{
  itemEletr_t *pEletr;
  itemFios_t *pFios;
  FILE *f_out;
  if((f_out = fopen(arqv, "w")) == NULL)
    fatalerr(1008, ERR_OPENFILE);
  pEletr = primEletr;
  while(pEletr != NULL) {
    fprintf(f_out, "(\"%s\" (", pEletr->hnd);
    pFios = pEletr->primFios;
    while(pFios != NULL) {
      fprintf(f_out, "(\"%s\" %d)", pFios->cirOrCmd, pFios->fios);
      pFios = pFios->prox;
    }
    fprintf(f_out, "))\n");
    pEletr = pEletr->prox;
  }
  fclose(f_out);
}

/* declaracao das rotinas de depuracao dos dados
*/
void debug_eletr(void)
{
  itemEletr_t *pEletr;
  pEletr = primEletr;
  while(pEletr != NULL) {
    printf("\n %s *** %s - %s", pEletr->hnd, pEletr->hnd1, pEletr->hnd2);
    pEletr = pEletr->prox;
  }
}

void debug_elem(void)
{
  itemElem_t *pElem;
  itemCmdElem_t *pCElem;
  pElem = primElem;
  while(pElem != NULL) {
    printf("\nhnd = %s(%d)", pElem->hnd, pElem->idx);
    printf("\n..tip = %d", pElem->tip);
    printf("\n..org = %s", pElem->org);
    printf("\n..des = %s", pElem->des);
    printf("\n..cir = %s", pElem->cir);
    printf("\n..cmd = ");
    pCElem = pElem->primCmdElem;
    while(pCElem != NULL) {
      printf("%s, ", (pCElem->pCmd)->cmd);
      pCElem = pCElem->prox;
    }
    printf("\n..fas = %d", pElem->fas);
    pElem = pElem->prox;
  }
}

void debug_desvios(void)
{
  itemDes_t *pDes;
  itemElemDes_t *pEDes;

  pDes = primDes;
  while(pDes != NULL) {
    printf("\ndes = %s", pDes->des);
    pEDes = pDes->primElemDes;
    while(pEDes != NULL) {
      printf("\n...%s(%d)", (pEDes->pElem)->hnd, (pEDes->pElem)->idx);
      pEDes = pEDes->prox;
    }
    pDes = pDes->prox;
  }
  getch();
}

void debug_comandos(void)
{
  itemCmd_t *pCmd;
  itemElemCmd_t *pECmd;

  pCmd = primCmd;
  while(pCmd != NULL) {
    printf("\ncmd = %s, cir = %s, fas = %d", pCmd->cmd, pCmd->cir, pCmd->fas);
    pECmd = pCmd->primElemCmd;
    while(pECmd != NULL) {
      printf("\n...hnd = %s(%d), itip = %d", pECmd->hnd, pECmd->idx, pECmd->itip);
      pECmd = pECmd->prox;
    }
    pCmd = pCmd->prox;
  }
  getch();
}

void debug_malha(itemMalha_t *primMalha)
{
  if(primMalha == NULL) {
    printf("NULL");
    return;
  }
  if(primMalha->pElem == NULL)
    printf("nil");
  else if(primMalha->pElem->hnd[0] == '\0')
	 printf("nul,");
       else
	 printf("%s(%d),", primMalha->pElem->hnd, primMalha->pElem->idx);
  if(primMalha->pEletr == NULL)
    printf("nil");
  else if(primMalha->pEletr->hnd[0] == '\0')
	 printf("nul");
       else
	 printf("%s", primMalha->pEletr->hnd);
  if(primMalha->primMalha != NULL) {
    printf("\n->");
    debug_malha(primMalha->primMalha);
    printf("<-");
  }
  if(primMalha->prox != NULL) {
    printf("\n..");
    debug_malha(primMalha->prox);
    printf("..");
  }
}

void debug_result(void)
{
  itemEletr_t *pEletr;
  itemFios_t *pFios;

  printf("\nRESULTADO:");

  pEletr = primEletr;
  while(pEletr != NULL) {
    printf("\n%s = ", pEletr->hnd);
    pFios = pEletr->primFios;
    while(pFios != NULL) {
      printf("%s, %d / ", pFios->cirOrCmd, pFios->fios);
      pFios = pFios->prox;
    }
    pEletr = pEletr->prox;
  }
}

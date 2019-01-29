/*************************/
/*************************/
// HFMANN.C
/*************************/
/*************************/

//#include <stdio.h>
//#include <conio.h>
//#include <stdlib.h>
#include "common.h"
#include "input.h"

#define MAXBITS 32
#define MAXNODES 512
#define MAXSYMBS 256

int hmin(void);
void hinsert(int,int);

struct codetype{
   int bits[MAXBITS];
   int startpos;
   };

struct nodetype{
   int freq;
   int father;
   int isleft;
   };

typedef struct hlist{
   int pos;
   int hfreq;
   struct hlist	*next;
   }hnode;


hnode *hroot=NULL,*traversal;

extern struct list a[256];
extern int n;

int hmin()
{
 int p;
 p = hroot->pos;
 traversal=hroot;
 hroot = traversal->next;
 free(traversal);
 return(p);
}

void hinsert(int p,int freq)
{
  hnode* new1=(hnode *)malloc(sizeof(hnode));
  new1->pos = p;
  new1->hfreq = freq;
  traversal = hroot;
  if(hroot == NULL)
  {
	hroot = new1;
	hroot->next = NULL;
	return;
  }
  if(hroot->next == NULL)
  {
	if(hroot->hfreq > new1->hfreq)
	{
		new1->next = hroot;
		hroot =new1;
		traversal->next =NULL;
		return;
	}
	else
	{
	   hroot->next =new1;
	   new1->next =NULL;
	   return;
	}
  }

  if(hroot->hfreq >= new1->hfreq)
  {
	new1->next =hroot;
	hroot = new1;
	return;
  }

  while(traversal->next->hfreq < new1->hfreq)
  {
	traversal=traversal->next;
	if(traversal->next==NULL)
	break;
  }

  if(traversal->next->hfreq>=new1->hfreq)
  {
	new1->next = traversal->next;
	traversal->next = new1;
	return;
  }

  new1->next = NULL;
  traversal->next = new1;

 }
int main(){
  struct codetype cd,code[MAXSYMBS];
  struct nodetype node[MAXNODES];
  int i,k,p,p1,p2,root;

  char symb,alph[MAXSYMBS];

  for(i=0;i<MAXSYMBS;i++)
	  alph[i]=' ';
  //scanf("%d",&n);
  input();
// cria o heap binario
  for(i=0;i<256;i++){
	 //flushall();
	// scanf("%c %d",&symb,&node[i].freq);
	 symb = a[i].alph;
	 node[i].freq = a[i].freq;
	 hinsert(i,node[i].freq);

	 alph[i]=symb;
	 }
// monta a arvore com o codigo de huffman
  for(p=0;p<(2*256-1);p++){

	 p1 =hmin();

	 p2 =hmin();

	 node[p1].father = p;
	 node[p1].isleft = 1;
	 node[p2].father = p;
	 node[p2].isleft = 0;
	 node[p].freq =node[p1].freq+node[p2].freq;
	 hinsert(p,node[p].freq);
  }
	 root = hmin();
  for(i=0;i<256;i++){
	 cd.startpos = MAXBITS;
	 p=i;
	 while(p!=root){
		--cd.startpos ;
		if(node[p].isleft)
		   cd.bits[cd.startpos] =0;
		else
		   cd.bits[cd.startpos] =1;
		p =node[p].father;
	 }
	 for(k=cd.startpos;k<MAXBITS;k++)
		code[i].bits[k]=cd.bits[k];
	 code[i].startpos =cd.startpos;
  }

  for(i=0;i<256;i++){
	 printf("\n%c  %d",alph[i],node[i].freq);
	 for(k=code[i].startpos;k<MAXBITS;k++)
		printf(" %d",code[i].bits[k]);
	 printf("\n");
	 }
	 return(0);
  }



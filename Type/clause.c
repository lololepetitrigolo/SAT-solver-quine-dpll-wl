#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <assert.h>
#include "types.h"
#include "clause.h"

//libère la mémoire allouer à une clause
void free_clause(clause c){
  free(c->l);
  free(c);
}

//créer une clause à partir d'une liste de littéraux
clause create_clause(litteral*list,int size){
  clause c = (clause)malloc(sizeof(clause));
  assert(c!=NULL);
  c->l = list;
  c->len = size;
  return c;
}

//affiche une clause
void print_clause(clause c) {
  if(c->len == 0) printf("CLAUSE VIDE\n");
  else{
    for(int i = 0; i < c->len; i++){
      printf("%d ",c->l[i]);
    }
    printf("\n");
  }
}

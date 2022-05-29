#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <assert.h>
#include "types.h"
#include "clause.h"
#include "fnc.h"

//libère la mémoire allouer à une fnc
void free_fnc(fnc f){
  if(f->len != 0){
    for (int i = 0; i < f->len; i++) {
      free_clause(f->c[i]);
    }
  }
  free(f->c);
  free(f);
}

//créer une fnc à partir d'une liste de clause
fnc create_fnc(clause*c,int len,int nb_lit){
  fnc f = (fnc)malloc(sizeof(fnc));
  assert(f!=NULL);
  f->c = c;
  f->len = len;
  f->nb_lit = nb_lit;
  return f;
}

//affiche une fnc
void print_fnc(fnc f) {
  if(f->len == 0) printf("FNC VIDE\n");
  else{
    for (int i = 0; i < f->len; i++) {
      print_clause(f->c[i]);
    }
  }
}

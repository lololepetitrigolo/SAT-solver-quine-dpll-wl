#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

#include "types.h"

//creer une liste avec un entier x
liste create_liste(int x){
  liste l = (liste)malloc(sizeof(struct maillon));
  assert(l!=NULL);
  l->val = x;
  l->next = NULL;
  return l;
}

//ajoute un élémement à une liste (peut aussi servir pour créer une liste si on lui donne un le pointeur NULL)
liste add(int x,liste l){
  liste h = create_liste(x);
  h->next = l;
  return h;
}

//supprime l'élémement x de la liste
liste del(int x, liste l){
  liste prev=NULL,mem = l;
  while(l!=NULL){
    if(l->val == x){
      if(prev==NULL){
        prev = l;
        l=l->next;
        free(prev);
        return l;
      }else{
        prev->next = l->next;
        free(l);
        return mem;
      }
    }
    prev = l;
    l = l->next;
  }
  return mem;
}

//libère la memoire allouer à une liste
void free_liste(liste l){
  liste next = NULL;
  while(l!=NULL) {
    next = l->next;
    free(l);
    l = next;
  }
}

//affiche une liste
void print_liste(liste l){
  if(l==NULL) printf("Liste vide\n");
  while(l!=NULL) {
    if(l->next == NULL) printf("%d\n",l->val);
    else printf("%d-",l->val);
    l = l->next;
  }
}
